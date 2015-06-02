#include <iostream>
#include <memory>

#include "frontends/avr.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // Parse options

    // Initialise for particular target

    // Initialise backend

    AvrMachine mach;

    vector<Instruction*> insns;

    insns.push_back(new Avr_add());
    insns.push_back(new Avr_eor());
    insns.push_back(new Avr_mul());

    vector<Slot*> slots;

    for(auto insn: insns)
    {
        auto s1 = insn->getSlots();
        slots.insert(slots.end(), s1.begin(), s1.end());
    }

    int n = 0;
    for(auto slot: slots)
    {
        auto va = static_cast<RegisterSlot*>(slot)->getValidArguments();

        cout << *va.begin() << endl;
        slot->setValue(*va.begin());
    }

    // slots[0].setValue(0);
    // slots[1].setValue(2);

    // slots[2].setValue(2);
    // slots[3].setValue(1);

    for(int i = 0; i < 32; ++i)
        mach.setRegisterValue(i, i+1);

    auto current_slot = &slots[0];

    for(auto insn: insns)
    {
        cout << insn->toString(current_slot) << endl;

        auto n = insn->execute(&mach, current_slot);
        current_slot += n;
    }
    // add.execute(&mach, &slots[0]);
    // add.execute(&mach, &slots[2]);

    cout << mach.toString();

    return 0;
}
