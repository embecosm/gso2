#include <iostream>
#include <algorithm>
#include <memory>

#include <mpi.h>
#include "parallel/mpi.hpp"

#include "frontends/avr.hpp"
#include "algorithms/canonicalIterator.hpp"
#include "algorithms/constantIterator.hpp"
#include "algorithms/bruteforce.hpp"
#include "algorithms/test.hpp"
#include "utility.hpp"

using namespace std;

typedef AvrMachine MachineType;
// typedef x86_64_Machine MachineType;

int main(int argc, char *argv[])
{
    // TODO: options

    MPI_Init(NULL, NULL);

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

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(world_rank == 0)
    {
        vector<int> factory_ids;

        for(unsigned i = 0; i < insn_factories.size(); ++i)
            factory_ids.push_back(i);

        vector<vector<int>> factory_lists;

        cout << "MASTER "<<endl;

        for(auto &f: current_factories)
            factory_lists.push_back(factory_ids);

        distributeTasks(factory_lists);

        MPI_Finalize();
        return 0;
    }

    cout << "SLAVE " << world_rank << endl;

    // Get a list of instructions and slots from the string
    vector<Instruction*> goal_insns;
    vector<Slot*> goal_slots;

    parseInstructionList(instruction_str, insn_factories, goal_insns, goal_slots);

    // Create a quick test state
    executeSequence(goal_insns, goal_slots, mach_expected);

    cout << "Goal sequence:\n";
    cout << print(goal_insns, goal_slots) << endl;


    vector<int> insn_ids;

    while(getWork(insn_ids)) {
        vector<Instruction*> insns;

        // Create the instructions, as per the current iterator
        for(unsigned i = 0; i < insn_ids.size(); ++i)
            insns.push_back(insn_factories[insn_ids[i]]());

        vector<Slot*> slots;

        // Get a list of all of the slots in the instruction list
        for(auto insn: insns)
        {
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

    }

    MPI_Finalize();

    return 0;
}
