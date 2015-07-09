#include <iostream>
#include <algorithm>
#include <memory>

#include "frontends/avr.hpp"
#include "algorithms/canonical.hpp"
#include "algorithms/bruteforce.hpp"
#include "algorithms/test.hpp"
#include "utility.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // TODO: options

    // An example goal function. Eventually we should read in a sequence of
    // instructions and use this as a goal.
    function<void(AvrMachine&)> goal(
        [](AvrMachine &mach){
            // mach.setRegisterValue(1, (mach.getRegisterValue(1)+37)&0xFF);
            mach.setRegisterValue(0, 0x10);
        }
    );

    AvrMachine mach, mach_expected, mach_initial;

    // Compute a test state - for quick discarding of invalid sequences.
    mach_expected.initialiseRandom();
    mach_initial = mach_expected;
    goal(mach_expected);

    // Get a list of functions which will construct an instruction
    auto insn_factories = mach.getInstructionFactories();

    vector<decltype(insn_factories)::iterator> current_factories;

    // The number of instructions should eventually be parameterised
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());
    // current_factories.push_back(insn_factories.begin());
    // current_factories.push_back(insn_factories.begin());

    do {
        vector<Instruction*> insns;

        // Create the instructions, as per the current iterator
        for(auto &factory: current_factories)
            insns.push_back((*factory)());

        vector<Slot*> slots;
        vector<ConstantSlot*> constant_slots;

        // Get a list of all of the slots in the instruction list
        for(auto insn: insns)
        {
            auto s1 = insn->getSlots();
            slots.insert(slots.end(), s1.begin(), s1.end());
        }

        // Work out what types of slots we need to iterate over
        for(auto slot: slots)
        {
            if(dynamic_cast<RegisterSlot*>(slot) != 0)
            {
                auto va = static_cast<RegisterSlot*>(slot)->getValidArguments();
                slot->setValue(*min_element(va.begin(), va.end()));
            }
            else if(dynamic_cast<ConstantSlot*>(slot) != 0)
            {
                constant_slots.push_back((ConstantSlot*)slot);
                slot->setValue(0);
            }
            else
                slot->setValue(0);
        }

        canonicalIterator c_iter(slots);

        // Bruteforce over the possible constants, and canonically iterate
        // over the register slots.
        do
        {
            do
            {
                mach = mach_initial;

                // Perform an equivalence test, and if it succeeds, run
                // multiple tests to work out if the sequence is likely to be
                // correct.
                if(testEquivalence(insns, slots, mach_initial, mach_expected))
                {
                    AvrMachine mach_test;

                    bool correct = testEquivalenceMultiple(insns, slots, goal);

                    if(correct)
                    {
                        cout << "Found" << endl;
                        cout << print(insns, slots) << endl;
                    }
                }
            } while(c_iter.next());
        } while(bruteforceIterate(constant_slots));

        // Free the instructions and slots
        for(auto slot: slots)
            delete slot;
        for(auto insn: insns)
            delete insn;

    } while(bruteforceIterate(insn_factories, current_factories));

    return 0;
}
