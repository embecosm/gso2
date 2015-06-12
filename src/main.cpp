#include <iostream>
#include <algorithm>
#include <memory>

#include "frontends/avr.hpp"
#include "algorithms/canonical.hpp"
#include "algorithms/bruteforce.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // Parse options

    // Initialise for particular target

    // Initialise backend

    AvrMachine mach, mach_expected;
    uint8_t vals[32];

    for(int i = 0; i < 32; ++i)
    {
        vals[i] = rand() & 0xFF;
        mach_expected.setRegisterValue(i, vals[i]);
    }

    mach_expected.setRegisterValue(0, mach_expected.getRegisterValue(0)*4+1);

    // Get a list of functions which will construct an instruction
    auto insn_factories = mach.getInstructionFactories();

    vector<decltype(insn_factories)::iterator> current_factories;

    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());

    do {
        vector<Instruction*> insns;

        for(auto factory: current_factories)
            insns.push_back((*factory)());

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

            slot->setValue(*min_element(va.begin(), va.end()));
        }

        auto canonical_skips = initialiseCanonical(slots);

        do
        {
            for(int i = 0; i < 32; ++i)
                mach.setRegisterValue(i, vals[i]);

            auto current_slot = &slots[0];

            for(auto insn: insns)
            {
                auto n = insn->execute(&mach, current_slot);
                current_slot += n;
            }

            if(mach == mach_expected)
            {
                AvrMachine mach_test;

                bool correct = true;
                int i;
                for(i = 0; i < 100; ++i)
                {
                    for(int j = 0; j < 32; ++j)
                    {
                        uint8_t v = rand() & 0xFF;
                        mach.setRegisterValue(j, v);
                        mach_test.setRegisterValue(j, v);
                    }
                    mach_test.setRegisterValue(0, mach_test.getRegisterValue(0)*4+1);

                    auto current_slot = &slots[0];

                    for(auto insn: insns)
                    {
                        auto n = insn->execute(&mach, current_slot);
                        current_slot += n;
                    }

                    if(!(mach == mach_test))
                    {
                        correct = false;
                        break;
                    }
                }

                if(correct)
                {
                    cout << "Found" << endl;
                    auto current_slot = &slots[0];

                    for(auto insn: insns)
                    {
                        cout << "    " << insn->toString(current_slot) << endl;

                        auto n = insn->execute(&mach, current_slot);
                        current_slot += n;
                    }
                    cout << endl;
                    // return 0;
                }
            }
            // break;
        } while(nextCanonical(slots, canonical_skips));

    } while(bruteforceIterate(insn_factories, current_factories));


    // vector<Slot*> slots;

    // for(auto insn: insns)
    // {
    //     auto s1 = insn->getSlots();
    //     slots.insert(slots.end(), s1.begin(), s1.end());
    // }

    // int n = 0;
    // for(auto slot: slots)
    // {
    //     auto va = static_cast<RegisterSlot*>(slot)->getValidArguments();

    //     cout << *va.begin() << endl;
    //     slot->setValue(*min_element(va.begin(), va.end()));
    // }

    // auto canonical_skips = initialiseCanonical(slots);

    // do
    // {
    //     for(int i = 0; i < 32; ++i)
    //         mach.setRegisterValue(i, i+1);

    //     auto current_slot = &slots[0];

    //     for(auto insn: insns)
    //     {
    //         cout << insn->toString(current_slot) << endl;

    //         auto n = insn->execute(&mach, current_slot);
    //         current_slot += n;
    //     }
    //     cout << endl;
    // } while(nextCanonical(slots, canonical_skips));

    // cout << mach.toString();

    return 0;
}
