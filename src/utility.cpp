#include <sstream>
#include <iostream>

#include <boost/algorithm/string.hpp>

#include "frontend.hpp"
#include "utility.hpp"

using namespace std;

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
