#include <iostream>
#include <algorithm>
#include <memory>

#include "frontends/avr.hpp"
#include "algorithms/canonical.hpp"

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
    insns.push_back(new Avr_eor());
    // insns.push_back(new Avr_mul());

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
        slot->setValue(*min_element(va.begin(), va.end()));
    }

    auto canonical_skips = initialiseCanonical(slots);

    do
    {
        for(int i = 0; i < 32; ++i)
            mach.setRegisterValue(i, i+1);

        auto current_slot = &slots[0];

        for(auto insn: insns)
        {
            cout << insn->toString(current_slot) << endl;

            auto n = insn->execute(&mach, current_slot);
            current_slot += n;
        }
        cout << endl;
    } while(nextCanonical(slots, canonical_skips));

    cout << mach.toString();

    return 0;
}
