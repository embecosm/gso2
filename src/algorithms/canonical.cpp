#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

#include "canonical.hpp"

using namespace std;

canonicalIterator::canonicalIterator(vector<Slot*> &slotlist_)
: canonicalIteratorBasic(slotlist_), canonical(slotlist.size())
{
    // Find number of different register values possible
    std::set<int> all;

    for(auto slot: slotlist)
    {
        auto va = slot->getValidArguments();
        all.insert(va.begin(), va.end());
    }

    max_class_size = all.size();

    // Since canonical is initialised to all zeroes and this may not map onto
    // the sequences of register classes we try to map, and then iterate
    // through canonical values until we find a valid mapping.
    while(true)
    {
        auto mapping = canonicalMapping(slotlist, canonical);

        if(mapping.second)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(mapping.first[i]);
            return;
        }

        bool found_next = canonicalStep();

        if(!found_next)
        {
            // This case should never happen, since we will have iterated
            // through all the canonical register sequences and not found a
            // mapping. This probably indicates a bug somewhere.
            cerr << "Unable to find a canonical mapping for the following"
                    " register classes:\n";

            for(unsigned i = 0; i < slotlist.size(); ++i)
            {
                cout << "    Slot " << i << ": ";
                for(auto reg: slotlist[i]->getValidArguments())
                    cout << reg << " ";
                cout << endl;
            }

            // Lets continue anyway
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(slotlist[i]->getValidArguments().front());
            return;
        }
    }
}

bool canonicalIterator::canonicalStep()
{
    bool found_next = false;

    for(unsigned i = canonical.size()-1; i > 0; i--)
    {
        unsigned next = canonical[i] + 1;

        unsigned max = 0;
        for(unsigned j = 0; j < i; ++j)
        {
            auto val = canonical[j];
            if(val > max)
                max = val;
        }

        if(next > max + 1 || next > max_class_size)
        {
            canonical[i] = 0;
            continue;
        }
        else
        {
            canonical[i] = next;
            found_next = true;
            break;
        }
    }

    return found_next;
}

bool canonicalIterator::next()
{
    while(true)
    {
        bool found_next = canonicalStep();

        if(!found_next)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(slotlist[i]->getValidArguments().front());
            return false;
        }

        auto mapping = canonicalMapping(slotlist, canonical);

        if(mapping.second)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(mapping.first[i]);
            return true;
        }
    }
}

canonicalIteratorBasic::canonicalIteratorBasic(vector<Slot*> &slotlist_)
{
    for(auto slot: slotlist_)
    {
        if(dynamic_cast<RegisterSlot*>(slot) != 0)
            slotlist.push_back((RegisterSlot*)slot);
    }
}

bool canonicalIteratorBasic::next()
{
    for(int i = slotlist.size()-1; i > 0; i--)
    {
        auto rs_i   = slotlist[i];
        auto va_i   = rs_i->getValidArguments();

        unsigned next = rs_i->getValue() + 1;

        unsigned max = 0;
        for(int j = 0; j < i; ++j)
        {
            auto val = slotlist[j]->getValue();
            if(val > max)
                max = val;
        }

        if(next > max + 1)
        {
            rs_i->setValue(0);
            continue;
        }
        else
        {
            rs_i->setValue(next);
            return true;
        }
    }

    return false;
}

/*! Find the possible registers that a value, val, could be remapped to. This
    satisfies the constraints of every slot.

    For example,
        Register value,    Register class
        1                  {  1,2,3}
        0                  {0,1,2,3}
        1                  {0,1,2}
        0                  {0,1,2,3}

    With val=1, the list of possibles is {1,2}, since these are the valid values
    for a remapping of the register 1.

    @param slotlist  The list of register slots which contain the register
                     classes.
    @param values    The list of values which correspond to the slotlist. Note
                     that these do not necessarily have to be valid (as per
                     the register classes), but is to specify the constraints
                     of the remapping (i.e. which registers must have the same
                     label).
    @param val       The register for which to find remappings.
    @return          The list of valid register that val can be remapped to.
*/
vector<unsigned> possibleRegisters(vector<RegisterSlot*> &slotlist, vector<unsigned> &values, unsigned val)
{
    vector<unsigned> possibles;
    bool isset = false;
    set<int> classes;

    for(unsigned i = 0; i < slotlist.size(); ++i)
    {
        if(values[i] == val)
        {
            int classid = slotlist[i]->getRegisterClassID();

            if(classid != -1)
            {
                auto result = classes.insert(classid);

                if(result.second == false)
                    continue;
            }

            if(!isset)
            {
                isset = true;
                possibles = slotlist[i]->getValidArguments();
            }
            else
            {
                auto va = slotlist[i]->getValidArguments();

                vector<unsigned> intersect;
                set_intersection(possibles.begin(),possibles.end(),va.begin(),va.end(), back_inserter(intersect));

                possibles = intersect;
            }

        }
    }

    return possibles;
}

// TODO: precompute register class intersections
// TODO: explaination, comments
// TODO: Investigate why this slows down with regular, but restricted classes 
//           e.g {0-3}x8 slower than {0-7}x8
pair<vector<unsigned>,bool> canonicalMapping(vector<RegisterSlot*> &slotlist,
        vector<unsigned> values)
{
    unsigned i = 0;
    vector<int> possibilities(slotlist.size());
    vector<unsigned> mapping(slotlist.size());

    // max of all classes
    vector<int> mapping_count;

    unsigned largest_reg=0;
    for(auto slot: slotlist)
    {
        unsigned m = slot->getValidArguments().back();

        if(m > largest_reg)
            largest_reg = m;
    }
    mapping_count.resize(largest_reg+1);

    if(values.size() != slotlist.size())
    {
        values.clear();
        values.resize(slotlist.size());

        for(unsigned j = 0; j < slotlist.size(); ++j)
            values[j] = slotlist[j]->getValue();
    }

    bool work = false;

    while(i < slotlist.size())
    {
        auto mv = find(values.begin(), values.begin()+i, values[i]);
        if(i > 0 && mv != values.begin()+i)
        {
            if(possibilities[i] != -1)
            {
                int n = mv - values.begin();
                mapping[i] = mapping[n];
                mapping_count[mapping[i]]++;
                possibilities[i] = -1;
                i++;
                if(i >= slotlist.size())
                {
                    work = true;
                }
                continue;
            }
            else
            {
                if(i == 0)
                    break;
                mapping_count[mapping[i-1]]--;
                possibilities[i] = 0;
                i--;
                continue;
            }
        }

        // For all slots with the same value as the current, calculate the
        // intersection of register classes, to compute a list of possible
        // remappings. Possible remappings are those an intersection of all
        // classes with the same register, minus the registers already
        // remapped.

        auto possibles_ = possibleRegisters(slotlist, values, values[i]);

        // TODO: optimize so we aren't doing so many allocations
        vector<unsigned> possibles;
        possibles.reserve(possibles_.size());

        // TODO optimisation: flags for size>0, possiblities[i]>=size,  var for possibilities[i]-th val, then break
        for(unsigned k=0; k < possibles_.size(); ++k)
            if(mapping_count[possibles_[k]] == 0)
                possibles.push_back(possibles_[k]);

        if(possibles.size() == 0)
        {
            if(i == 0)
                break;
            mapping_count[mapping[i-1]]--;
            possibilities[i] = 0;
            i--;
            continue;
        }

        // We return to the previous value if we have exceeded the number of
        // possibles, or if we have tested more values than there are
        // positions left. The latter condition is a speed up - if it is not
        // there, large classes will be explored full, making this function
        // take up to seconds (!) to execute.
        if(possibilities[i] >= (int) possibles.size() || possibilities[i] > possibilities.size() - i)
        {
            if(i == 0)
                break;
            mapping_count[mapping[i-1]]--;
            possibilities[i] = 0;
            i--;
            continue;
        }

        mapping[i] = possibles[possibilities[i]];
        mapping_count[mapping[i]]++;
        possibilities[i]++;
        i++;
        if(i >= slotlist.size())
        {
            work = true;
        }
    }

    return make_pair(mapping, work);
}
