#include <algorithm>
#include <vector>
#include <unordered_set>
#include <iostream>

#include "canonical.hpp"

using namespace std;

canonicalIterator::canonicalIterator(vector<Slot*> &slotlist_)
: canonicalIteratorBasic(slotlist_), canonical(slotlist_.size())
{
    max_class_size = 0;
    for(auto slot: slotlist)
    {
        unsigned c_len = ((RegisterSlot*)slot)->getValidArguments().size();

        if(c_len > max_class_size)
            max_class_size = c_len;
    }
}

bool canonicalIterator::next()
{
    while(true)
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

            if(next > max + 1 || next >= max_class_size)
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

        if(!found_next)
        {
            for(unsigned i = 0; i < slotlist.size(); ++i)
                slotlist[i]->setValue(((RegisterSlot*)slotlist[i])->getValidArguments().front());
            return false;
        }

        // // TODO update canonical mapping to take a values parameter, and just
        // // get the register classes from the slotlist.
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
            slotlist.push_back(slot);
    }
}

bool canonicalIteratorBasic::next()
{
    for(int i = slotlist.size()-1; i > 0; i--)
    {
        auto rs_i   = (RegisterSlot *) slotlist[i];
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

vector<unsigned> possibleRegisters(vector<Slot*> &slotlist, vector<unsigned> &values, unsigned val)
{
    vector<unsigned> possibles;
    bool set = false;

    for(unsigned i = 0; i < slotlist.size(); ++i)
    {
        if(values[i] == val)
        {
            if(!set)
            {
                set = true;
                possibles = ((RegisterSlot*)slotlist[i])->getValidArguments();
            }
            else
            {
                auto va = ((RegisterSlot*)slotlist[i])->getValidArguments();

                vector<unsigned> intersect;
                set_intersection(possibles.begin(),possibles.end(),va.begin(),va.end(), back_inserter(intersect));

                possibles = intersect;
            }

        }
    }

    return possibles;
}

// TODO: precompute register class intersections
pair<vector<unsigned>,bool> canonicalMapping(vector<Slot*> &slotlist,
        vector<unsigned> values)
{
    unsigned i = 0;
    int possibilities[slotlist.size()] = {0};
    vector<unsigned> mapping(slotlist.size());

    if(values.size() != slotlist.size())
    {
        cout << "resetting"<<endl;
        values.clear();
        values.resize(slotlist.size());

        for(unsigned j = 0; j < slotlist.size(); ++j)
            values[j] = slotlist[j]->getValue();
    }

    bool work = false;

    while(i < slotlist.size())
    {
        // cout << i << endl;
        auto mv = find(values.begin(), values.begin()+i, values[i]);
        if(i > 0 && mv != values.begin()+i)
        {
            if(possibilities[i] != -1)
            {
                int n = mv - values.begin();
                mapping[i] = mapping[n];
                possibilities[i] = -1;
                i++;
                if(i >= slotlist.size())
                {
                    work = true;
                }
                // cout << "p, forward: " << mapping[n] << endl;
                continue;
            }
            else
            {
                // cout << "p, backwards" << endl;
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

        // cout << "possibles_: ";
        // for(auto p: possibles_)
        //     cout << p << " ";
        // cout << endl;

        auto pend = remove_if(possibles_.begin(), possibles_.end(),
            [mapping,i] (unsigned p) {
                return (find(mapping.begin(), mapping.begin()+i, p) != mapping.begin()+i);
            });
        vector<unsigned> possibles(possibles_.begin(), pend);

        // cout << "possibles: ";
        // for(auto p: possibles)
        //     cout << p << " ";
        // cout << endl;

        if(possibles.size() == 0)
        {
            // cout << "no possibles" << endl;
            possibilities[i] = 0;
            i--;
            continue;
        }

        if(possibilities[i] >= (int) possibles.size())
        {
            // cout << "run out of possibles" << endl;
            possibilities[i] = 0;
            i--;
            continue;
        }

        // cout << "mapped " << possibles[possibilities[i]] << endl;
        mapping[i] = possibles[possibilities[i]];
        possibilities[i]++;
        i++;
        if(i >= slotlist.size())
        {
            work = true;
        }
    }

    return make_pair(mapping, work);
}
