#include <sstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "frontend.hpp"
#include "utility.hpp"

using namespace std;

Combinations::Combinations(unsigned size_, unsigned n_)
{
    // TODO check values.size() <= 64
    n = n_;
    size = size_;
    current = (1 << n) - 1;
    max = (current << (size - n));
}

// TODO: should we return the actual values here, rather than the index of
// the value?
void Combinations::getSelection(std::vector<unsigned> &sel)
{
    uint64_t val = current;
    unsigned current_idx = 0;

    sel.resize(n);

    for(unsigned i = 0; i < size; ++i, val >>= 1)
    {
        if(val & 1)
            sel[current_idx++] = i;
    }
}

bool Combinations::next()
{
    uint64_t smallest, ripple, ones, x = current;

    // snoob function. Next unsigned integer with the same number of one
    // bits set.
    smallest = x & -x;
    ripple = x + smallest;
    ones = x ^ ripple;
    ones = (ones >> 2) /smallest;

    current = ripple | ones;
    // End snoob function.

    if(current > max || (max == 1 && current == 1))
    {
        current = (1<<n)-1;
        return false;
    }
    else
        return true;
}

string print(const vector<Instruction *> &insns,
    vector<Slot*> &slots)
{
    auto slot = &slots[0];
    stringstream ss;

    for(auto insn: insns)
    {
        ss << insn->toString(slot) << endl;
        slot += insn->getNumberOfSlots();
    }
    ss << endl;

    return ss.str();
}


pair<Instruction*, vector<Slot*>> parseInstruction(string input, vector<function<Instruction *()>> factories)
{
    vector<Instruction*> insns;
    vector<Slot*> slots;

    for(auto &factory: factories)
        insns.push_back(factory());

    for(unsigned int i = 0; i < insns.size(); ++i)
    {
        bool success = insns[i]->parse(input, slots);

        if(success)
        {
            Instruction *insn = factories[i]();

            return make_pair(insn, slots);
        }
    }

    return make_pair(nullptr, vector<Slot*>());
}

bool parseInstructionList(string input, vector<function<Instruction *()>> factories,
    vector<Instruction*> &insns, vector<Slot*> &slots)
{
    vector<string> lines;

    boost::split(lines, input, boost::is_any_of("\n"));

    for(auto line: lines)
    {
        auto insn_data = parseInstruction(line, factories);

        if(insn_data.first == nullptr)
        {
            cout << "Could not recognise line of input:\n\t" << line << endl;
            return false;
        }

        auto insn_slots = insn_data.second;
        slots.insert(slots.end(), insn_slots.begin(), insn_slots.end());

        insns.push_back(insn_data.first);
    }

    return true;
}
