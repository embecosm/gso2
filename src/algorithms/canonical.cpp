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
    int largest_classid=0;

    computed_intersections = true;
    for(auto slot: slotlist)
    {
        auto va = slot->getValidArguments();
        all.insert(va.begin(), va.end());

        int cid = slot->getRegisterClassID();

        if(cid == -1)
            computed_intersections = false;

        if(cid > largest_classid)
            largest_classid = cid;
    }

    max_class_size = all.size();

    // If we have at least one class which is not set, then do not compute the
    // intersection and rely on the slower way of doing it.
    if(computed_intersections)
    {
        vector<vector<unsigned>> classes(largest_classid+1);

        for(auto slot: slotlist)
        {
            int cid = slot->getRegisterClassID();
            classes[cid] = slot->getValidArguments();
        }

        precomputePossibleRegisters(classes);
    }

    if(slotlist.size() == 0)    // All slots have been excluded.
    {
        return;
    }

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

void canonicalIterator::precomputePossibleRegisters(vector<vector<unsigned>> classes)
{
    unsigned n_classes = classes.size();

    class_intersections.resize(1 << n_classes);

    // Only support up to a few register classes. TODO handle edge case for
    // processors with many register classes?
    for(unsigned class_set = 0; class_set < (1U<<n_classes); ++class_set)
    {
        bool isset = false;
        vector<unsigned> class_intersection;

        for(unsigned i = 0; i < n_classes; ++i)
        {
            if(class_set&(1<<i))
            {
                if(!isset)
                {
                    isset = true;
                    class_intersection = classes[i];
                }
                else
                {
                    vector<unsigned> intersect;

                    set_intersection(class_intersection.begin(), class_intersection.end(),
                        classes[i].begin(), classes[i].end(), back_inserter(intersect));

                    class_intersection = move(intersect);
                }
            }
        }

        class_intersections[class_set] = class_intersection;
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
    if(slotlist.size() == 0)    // All slots have been excluded.
    {
        return false;
    }

    while(true)
    {
        bool found_next = canonicalStep();

        if(!found_next)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(slotlist[i]->getValidArguments().front());
            return false;
        }

        pair<vector<unsigned>,bool> mapping;

        // Pass the class_intersections if we had well defined register classes.
        if(computed_intersections)
            mapping = canonicalMapping(slotlist, canonical, &class_intersections);
        else
            mapping = canonicalMapping(slotlist, canonical, nullptr);

        if(mapping.second)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(mapping.first[i]);
            return true;
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

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
    @param loc       The location in the sequence of the register for which to
                     find remappings.
    @return          The list of valid register that val can be remapped to.
*/
vector<unsigned> possibleRegisters(vector<RegisterSlot*> &slotlist, vector<unsigned> &values, unsigned loc)
{
    vector<unsigned> possibles;
    bool isset = false;
    set<int> classes;
    unsigned val = values[loc];

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
        vector<unsigned> values, vector<vector<unsigned>> *class_intersections)
{
    unsigned i = 0;
    vector<int> possibilities(slotlist.size());
    vector<unsigned> mapping(slotlist.size());
    vector<unsigned> possibles;

    bool have_intersections=false;

    if(class_intersections != nullptr)
        have_intersections = true;

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
        // remappings. The intersections are either precomputed, if the
        // register classes of each slot were known. Otherwise they are
        // computed on the fly (slower).

        if(have_intersections)
        {
            unsigned v = values[i];
            unsigned idx=0;
            vector<unsigned> *possibles_ptr;

            // Each class id is represented by a single bit, bitwise-oring the
            // bits together forms the index into the array.
            for(unsigned j = 0; j < slotlist.size(); ++j)
            {
                if(values[j] == v)
                {
                    idx |= 1 << slotlist[j]->getRegisterClassID();
                }
            }

            possibles_ptr = &(*class_intersections)[idx];
            possibles.clear();

            const unsigned intersect_size = possibles_ptr->size();
            possibles.reserve(intersect_size);

            // Once we have the intersections of the classes, we do not want
            // the registers which we have already remapped. Copy them into a
            // new vector, so we can leave the precomputed values untouched.

            // TODO optimisation: flags for size>0, possiblities[i]>=size,  var for possibilities[i]-th val, then break
            for(unsigned k=0; k < intersect_size; ++k)
                if(mapping_count[(*possibles_ptr)[k]] == 0)
                    possibles.push_back((*possibles_ptr)[k]);
        }
        else
        {
            vector<unsigned> possibles_;
            possibles_ = possibleRegisters(slotlist, values, i);

            // This code is mostly similar to above, but the above takes a
            // pointer to the precomputed vector - about 20% faster than copying.
            possibles.clear();
            possibles.reserve(possibles_.size());

            // TODO optimisation: flags for size>0, possiblities[i]>=size,  var for possibilities[i]-th val, then break
            for(unsigned k=0; k < possibles_.size(); ++k)
                if(mapping_count[possibles_[k]] == 0)
                    possibles.push_back(possibles_[k]);
        }

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
        if(possibilities[i] >= (int) possibles.size() || possibilities[i] > (int)(possibilities.size() - i))
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

//////////////////////////////////////////////////////////////////////////////

canonicalIteratorLiveness::canonicalIteratorLiveness(vector<Slot*> &slotlist_, unsigned n_live_)
: canonicalIterator(slotlist_), n_live(n_live_)
{
}

bool canonicalIteratorLiveness::next()
{
    if(slotlist.size() == 0)    // All slots have been excluded.
    {
        return false;
    }

    while(true)
    {
        bool found_next = canonicalStep();

        if(!found_next)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(slotlist[i]->getValidArguments().front());
            return false;
        }

        pair<vector<unsigned>,bool> mapping;

        // Pass the class_intersections if we had well defined register classes.
        if(computed_intersections)
            mapping = canonicalMappingLive(slotlist, canonical, n_live, &class_intersections);
        else
            mapping = canonicalMappingLive(slotlist, canonical, n_live, nullptr);

        if(mapping.second)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(mapping.first[i]);
            return true;
        }
    }
}



// TODO: precompute register class intersections
// TODO: explaination, comments
// TODO: Investigate why this slows down with regular, but restricted classes
//           e.g {0-3}x8 slower than {0-7}x8
pair<vector<unsigned>,bool> canonicalMappingLive(vector<RegisterSlot*> &slotlist,
        vector<unsigned> values, unsigned n_live,
        vector<vector<unsigned>> *class_intersections)
{
    unsigned i = 0;
    vector<int> possibilities(slotlist.size());
    vector<unsigned> mapping(slotlist.size());
    vector<unsigned> possibles;

    bool have_intersections=false;

    if(class_intersections != nullptr)
        have_intersections = true;

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

    vector<bool> current_live_reg, current_read_reg;

    current_live_reg.resize(largest_reg+1);
    current_read_reg.resize(largest_reg+1);

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
        // remappings. The intersections are either precomputed, if the
        // register classes of each slot were known. Otherwise they are
        // computed on the fly (slower).

        if(have_intersections)
        {
            unsigned v = values[i];
            unsigned idx=0;
            vector<unsigned> *possibles_ptr;

            // Each class id is represented by a single bit, bitwise-oring the
            // bits together forms the index into the array.
            for(unsigned j = 0; j < slotlist.size(); ++j)
            {
                if(values[j] == v)
                {
                    idx |= 1 << slotlist[j]->getRegisterClassID();
                }
            }

            possibles_ptr = &(*class_intersections)[idx];
            possibles.clear();

            const unsigned intersect_size = possibles_ptr->size();
            possibles.reserve(intersect_size);

            // Once we have the intersections of the classes, we do not want
            // the registers which we have already remapped. Copy them into a
            // new vector, so we can leave the precomputed values untouched.

            // Calculate which registers are live, and which are read from,
            // this is a representation of the use and def sets.
            for(unsigned j = 0; j < i; ++j)
                if(slotlist[j]->isWriteSlot() && !slotlist[j]->isReadSlot())
                    current_live_reg[mapping[j]] = true;
                else
                    current_read_reg[mapping[j]] = true;

            // From these two sets, calculate read_set - live_set. If the size
            // of this set is bigger than the number of live registers in,
            // then we have used too many registers and the sequence cannot be
            // valid.
            unsigned unlive = 0;
            for(unsigned j = 0; j < largest_reg+1; ++j)
            {
                if(current_read_reg[j] and not current_live_reg[j])
                    unlive++;
            }

            for(unsigned k=0; k < possibles_ptr->size(); ++k)
            {
                unsigned current_possible = (*possibles_ptr)[k];

                if(mapping_count[current_possible] == 0)
                {
                    bool is_live = false;

                    // If the register is a write only slot, then we can always map it.
                    if(slotlist[i]->isWriteSlot() && !slotlist[i]->isReadSlot())
                        is_live = true;
                    // If the possible register is not live, then this
                    // increases the number of live registers from the input
                    // set that we use (the !current_live_reg[current_possible]
                    // part). Check that this doesn't exceed the number of
                    // possible live registers.
                    else if(unlive + !current_live_reg[current_possible] <= n_live)
                        is_live = true;

                    if(is_live)
                        possibles.push_back(current_possible);
                }
            }

        }
        else
        {
            vector<unsigned> possibles_;
            possibles_ = possibleRegisters(slotlist, values, i);

            // This code is mostly similar to above, but the above takes a
            // pointer to the precomputed vector - about 20% faster than copying.
            possibles.clear();
            possibles.reserve(possibles_.size());

            // TODO optimisation: flags for size>0, possiblities[i]>=size,  var for possibilities[i]-th val, then break

            vector<bool> current_live_reg, current_read_reg;

            current_live_reg.resize(largest_reg+1);
            current_read_reg.resize(largest_reg+1);

            // Calculate which registers are live, and which are read from,
            // this is a representation of the use and def sets.
            for(unsigned j = 0; j < i; ++j)
                if(slotlist[j]->isWriteSlot() && !slotlist[j]->isReadSlot())
                    current_live_reg[mapping[j]] = true;
                else
                    current_read_reg[mapping[j]] = true;

            // From these two sets, calculate read_set - live_set. If the size
            // of this set is bigger than the number of live registers in,
            // then we have used too many registers and the sequence cannot be
            // valid.
            unsigned unlive = 0;
            for(unsigned j = 0; j < largest_reg+1; ++j)
            {
                if(current_read_reg[j] and not current_live_reg[j])
                    unlive++;
            }

            for(unsigned k=0; k < possibles_.size(); ++k)
                if(mapping_count[possibles_[k]] == 0)
                {
                    bool is_live = false;

                    // If the register is a write only slot, then we can always map it.
                    if(slotlist[i]->isWriteSlot() && !slotlist[i]->isReadSlot())
                        is_live = true;
                    // If the possible register is not live, then this
                    // increases the number of live registers from the input
                    // set that we use (the !current_live_reg[possibles_[k]]
                    // part). Check that this doesn't exceed the number of
                    // possible live registers.
                    else if(unlive + !current_live_reg[possibles_[k]] <= n_live)
                        is_live = true;

                    if(is_live)
                        possibles.push_back(possibles_[k]);
                }
        }

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
        if(possibilities[i] >= (int) possibles.size() || possibilities[i] > (int)(possibilities.size() - i))
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
