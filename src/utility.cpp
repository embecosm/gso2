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

pair<vector<Instruction*>, vector<Slot*>> parseInstructionList(string input, vector<function<Instruction *()>> factories)
{
    vector<string> lines;
    vector<Instruction*> insns;
    vector<Slot*> slots;

    boost::split(lines, input, boost::is_any_of("\n"));

    for(auto line: lines)
    {
        auto insn_data = parseInstruction(line, factories);

        cout << line << endl;

        if(insn_data.first == nullptr)
        {
            cout << "Could not recognise line of input:\n\t" << line << endl;
            continue;
        }

        cout << insn_data.first->getName() << endl;

        auto insn_slots = insn_data.second;
        slots.insert(slots.end(), insn_slots.begin(), insn_slots.end());

        insns.push_back(insn_data.first);
    }

    cout << insns.size() << " " << slots.size() << endl;

    cout << print(insns, slots) << endl;

    return make_pair(insns, slots);
}
