#include <iostream>
#include <algorithm>
#include <memory>

#include <mpi.h>
#include "parallel/mpi.hpp"

#include "frontends/avr.hpp"
// #include "frontends/x86_64.hpp"
#include "algorithms/canonical.hpp"
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

    // An example goal function. Eventually we should read in a sequence of
    // instructions and use this as a goal.
    function<void(MachineType&)> goal(
        [](MachineType &mach){
            // mach.setRegisterValue(1, (mach.getRegisterValue(1)+37)&0xFF);
            mach.setRegisterValue(0, 0x10);
            mach.setRegisterValue(1, 0x11);
            // mach.setRegisterValue(2, 0x12);
            // mach.setRegisterValue(3, 0x13);
        }
    );

    MachineType mach, mach_expected, mach_initial;

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

    vector<int> insn_ids;

    while(getWork(insn_ids)) {
        vector<Instruction*> insns;

        // Create the instructions, as per the current iterator
        for(unsigned i = 0; i < insn_ids.size(); ++i)
            insns.push_back(insn_factories[insn_ids[i]]());

        vector<Slot*> slots;
        vector<MachineType::ConstantSlot*> constant_slots;

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
            else if(dynamic_cast<MachineType::ConstantSlot*>(slot) != 0)
            {
                constant_slots.push_back((MachineType::ConstantSlot*)slot);

                ((MachineType::ConstantSlot*)slot)->iteratorSkip(true);
                slot->setValue(0);
            }
            else
                slot->setValue(0);
        }

        canonicalIteratorGeneric c_iter(slots);
        // cout << print(insns,slots) << endl;

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
                    MachineType mach_test;

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

    }

    MPI_Finalize();

    return 0;
}
