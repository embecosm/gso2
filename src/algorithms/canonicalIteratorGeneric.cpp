#include <algorithm>
#include <vector>
#include <set>
#include <iostream>

#include "canonicalIteratorGeneric.hpp"

using namespace std;

canonicalIteratorGeneric::canonicalIteratorGeneric(vector<Slot*> &slotlist_)
: canonicalIteratorBasic(slotlist_), canonical(slotlist.size())
{
    // Sort our list of slots by valid arguments size. This can massively
    // speed up the enumeration of some register classes, based on the
    // observation that the remapping function can 'early out' if it runs out
    // of register classes. Therefore the smallest classes should go first.
    stable_sort(slotlist.begin(), slotlist.end(),
        [] (RegisterSlot *a, RegisterSlot *b) {
            return a->getValidArguments().size() < b->getValidArguments().size();
        }
        );

    // Find number of different register values possible. Computed as the
    // union of all classes.
    std::set<int> all;
    int largest_classid=0;

    computed_intersections = true;
    for(auto slot: slotlist)
    {
        auto va = slot->getValidArguments();
        all.insert(va.begin(), va.end());

        int cid = slot->getRegisterClassID();

        // Set this to false if there is atleast one class without a set
        // register class ID.
        if(cid == -1)
            computed_intersections = false;

        if(cid > largest_classid)
            largest_classid = cid;
    }

    max_class_size = all.size();

    // If we have at least one class with its ID not set, then do not compute
    // the intersection and rely on the slower way of doing it.
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

    // All slots have been excluded, i.e. no register slots
    if(slotlist.size() == 0)
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

void canonicalIteratorGeneric::precomputePossibleRegisters(vector<vector<unsigned>> classes)
{
    unsigned n_classes = classes.size();

    class_intersections.resize(1U << n_classes);

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

bool canonicalIteratorGeneric::canonicalStep()
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

bool canonicalIteratorGeneric::next()
{
    if(slotlist.size() == 0)    // All slots have been excluded.
    {
        return false;
    }

    // Iterate through possible mappings (canonicalStep), and test whether the
    // registers can be remapped into values consistent with their register class.
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


/*! Find the possible registers that a value, val, could be remapped to. This
    satisfies the constraints of every slot.

    For example,
    \code
        Register value,    Register class
        1                  {  1,2,3}
        0                  {0,1,2,3}
        1                  {0,1,2}
        0                  {0,1,2,3}
    \endcode

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

// Accept a generic slots list.
pair<vector<unsigned>,bool> canonicalMapping(vector<Slot*> &slotlist,
        vector<unsigned> values, vector<vector<unsigned>> *class_intersections)
{
    vector<RegisterSlot*> rs;

    rs.reserve(slotlist.size());

    // TODO: should be only extract the values which correspond to the slots.
    for(auto slot: slotlist)
        if(dynamic_cast<RegisterSlot*>(slot) != 0)
            rs.push_back((RegisterSlot*)slot);

    return canonicalMapping(rs, values, class_intersections);
}

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

    // TODO this should probably be precomputed, since 30% of this functions
    // time is spent in this for loop.
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

    // Flag for whether the remapping is successful.
    bool work = false;

    // This loop iterates through possible remappings of the registers. For
    // each slot, a list of "possibles" is computed - representing which
    // registers could be remapped to the current slot. An overall index into
    // each possibles list is kept (possibilities). The value possibilities[i]
    // is the index into the current possibles list.
    //
    // The variable, mapping, stores the remapped list of registers, with
    // mapping_count storing the number of times each register has been
    // remapped. I.e. mapping_count[k] is the number of times register k
    // appears in mapping.

    while(i < slotlist.size())
    {
        // Find out if the current value has been seen earlier in the
        // sequence.
        auto mv = find(values.begin(), values.begin()+i, values[i]);
        if(i > 0 && mv != values.begin()+i)
        {
            // If we have seen the same value before, we assign the same value
            // to it. We then mark is as "done" in the possibilities list.
            // This is so we can skip over the assignment if we are winding an
            // assignment (i.e. going backwards).
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

        // We have found a potential remapping, therefore store this, and
        // increment the number of times this register has been mapped.
        // Advance to the next slot. If we are at the last slot, then we have
        // successfully remapped.
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
