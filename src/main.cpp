#include <iostream>
#include <algorithm>
#include <memory>

#include "frontends/avr.hpp"
#include "algorithms/canonical.hpp"
#include "algorithms/constantIterator.hpp"
#include "algorithms/bruteforce.hpp"
#include "algorithms/test.hpp"
#include "utility.hpp"

using namespace std;

typedef AvrMachine MachineType;

int main(int argc, char *argv[])
{
    // TODO: options

    std::string instruction_str =
        "add r0, r0\n"
        "add r0, r0\n"
        "add r0, r0\n"
        "add r0, r0\n"
        "add r0, r0";


    MachineType mach, mach_expected, mach_initial;

    // Compute a test state - for quick discarding of invalid sequences.
    mach_expected.initialiseRandom();
    mach_initial = mach_expected;

    // Get a list of functions which will construct an instruction
    auto insn_factories = mach.getInstructionFactories();

    vector<decltype(insn_factories)::iterator> current_factories;

    // The number of instructions should eventually be parameterised
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());
    current_factories.push_back(insn_factories.begin());

    // Get a list of instructions and slots from the string
    vector<Instruction*> goal_insns;
    vector<Slot*> goal_slots;

    parseInstructionList(instruction_str, insn_factories, goal_insns, goal_slots);

    // Create a quick test state
    executeSequence(goal_insns, goal_slots, mach_expected);

    cout << "Goal sequence:\n";
    cout << print(goal_insns, goal_slots) << endl;


    do {
        vector<Instruction*> insns;
        vector<Slot*> slots;

        // Create the instructions, as per the current iterator
        // Get a list of all of the slots in the instruction list
        for(auto &factory: current_factories)
        {
            Instruction *insn = (*factory)();

            insns.push_back(insn);

            auto s1 = insn->getSlots();
            slots.insert(slots.end(), s1.begin(), s1.end());
        }


        canonicalIteratorBasic c_iter(slots, 0);

        constantIterator cons_iter(slots);
        cons_iter.setLossy(true);

        // Bruteforce over the possible constants, and canonically iterate
        // over the register slots.
        do
        {
            do
            {
                // Perform an equivalence test, and if it succeeds, run
                // multiple tests to work out if the sequence is likely to be
                // correct.
                if(testEquivalence(insns, slots, mach_initial, mach_expected))
                {
                    bool correct = testEquivalenceMultiple<AvrMachine>(insns, slots, goal_insns, goal_slots);

                    if(correct)
                    {
                        auto mapping = canonicalMapping(slots);

                        if(mapping.second)
                        {
                            cout << "Found" << endl;
                            cout << print(insns, slots) << endl;
                        }
                    }
                }
            } while(c_iter.next());
        } while(cons_iter.next());

        // Free the instructions and slots
        for(auto slot: slots)
            delete slot;
        for(auto insn: insns)
            delete insn;

    } while(bruteforceIterate(insn_factories, current_factories));

    return 0;
}
