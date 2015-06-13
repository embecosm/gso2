#define BOOST_TEST_MODULE test (algorithm) test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vector>
#include <iostream>

#include "algorithms/test.hpp"

using namespace std;

class TestMachine : public TargetMachine
{
public:
    TestMachine(vector<uint32_t> regs_={0,0,0,0})
    {
        for(int i = 0; i < 4; ++i)
            regs[i] = regs_[i];
    }

    void initialiseRandom()
    {
        for(int i = 0; i < 4; ++i)
            regs[i] = rand();
    }

    bool operator== (TestMachine &rhs)
    {
        for(int i = 0; i < 4; ++i)
        {
            if(regs[i] != rhs.regs[i])
                return false;
        }
        return true;
    }

    uint32_t regs[4];
};

class TestInstruction: public Instruction
{
public:
    TestInstruction(unsigned sum_slots_, unsigned add_)
    {
        sum_slots = sum_slots_;
        add_extra = add_;
        BOOST_REQUIRE(sum_slots > 0);
    }

    virtual unsigned execute(TargetMachine *_mach, Slot **slots)
    {
        TestMachine *mach = static_cast<TestMachine*>(_mach);
        uint32_t sum = 0;

        for(int i = 0; i < sum_slots; ++i)
        {
            uint32_t reg = mach->regs[slots[i]->getValue()];

            sum += reg;
        }

        sum += add_extra;

        mach->regs[slots[0]->getValue()] = sum;

        return sum_slots;
    }

    unsigned sum_slots;
    unsigned add_extra;
};

BOOST_AUTO_TEST_CASE( machine_state_tests )
{
    {
        vector<Instruction*> insns;
        vector<Slot*> slots;
        TestMachine mach_initial({0,0,0,0});
        TestMachine mach_expected({0,0,0,0});

        insns.push_back(new TestInstruction(1, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        BOOST_REQUIRE(testEquivalence(insns,slots,mach_initial, mach_expected) == true);
    }

    {
        vector<Instruction*> insns;
        vector<Slot*> slots;
        TestMachine mach_initial({1,2,0,0});
        TestMachine mach_expected({3,2,0,0});

        insns.push_back(new TestInstruction(2, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 1));

        BOOST_REQUIRE(testEquivalence(insns,slots,mach_initial, mach_expected) == true);
    }

    {
        vector<Instruction*> insns;
        vector<Slot*> slots;
        TestMachine mach_initial({1,2,0,0});
        TestMachine mach_expected({3,5,0,0});

        // sum r0, r1
        // sum r1, r0
        // Overall: r0 = r0 + r1,  r1 = r0 + r1 + r1
        insns.push_back(new TestInstruction(2, 0));
        insns.push_back(new TestInstruction(2, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 1));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 1));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        BOOST_REQUIRE(testEquivalence(insns,slots,mach_initial, mach_expected) == true);
    }
}

BOOST_AUTO_TEST_CASE( instruction_sequences_tests )
{
    srand(0);

    {
        vector<Instruction*> insns;
        vector<Slot*> slots;
        vector<Instruction*> ref_insns;
        vector<Slot*> ref_slots;

        // sum r0, r0
        insns.push_back(new TestInstruction(1, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        // sum r0, r0
        ref_insns.push_back(new TestInstruction(1, 0));
        ref_slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        BOOST_REQUIRE(testEquivalence<TestMachine>(insns,slots,ref_insns,ref_slots) == true);
        BOOST_REQUIRE(testEquivalenceMultiple<TestMachine>(insns,slots,ref_insns,ref_slots) == true);
    }

    {
        vector<Instruction*> insns;
        vector<Slot*> slots;
        vector<Instruction*> ref_insns;
        vector<Slot*> ref_slots;

        // sum r0, r0
        // sum r0, r0
        // Overall: r0 = r0 * 4
        insns.push_back(new TestInstruction(2, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        insns.push_back(new TestInstruction(2, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        // sum r0, r0, r0, r0
        // Overall: r0 = r0 * 4
        ref_insns.push_back(new TestInstruction(4, 0));
        ref_slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        ref_slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        ref_slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));
        ref_slots.push_back(new RegisterSlot(true, true, {0,1,2,3}, 0));

        BOOST_REQUIRE(testEquivalence<TestMachine>(insns,slots,ref_insns,ref_slots) == true);
        BOOST_REQUIRE(testEquivalenceMultiple<TestMachine>(insns,slots,ref_insns,ref_slots) == true);
    }
}
