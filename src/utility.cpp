#include <sstream>

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
