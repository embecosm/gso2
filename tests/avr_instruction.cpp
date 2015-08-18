#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "algorithms/canonicalIterator.hpp"
#include "algorithms/test.hpp"
#include "frontends/avr.hpp"

using namespace std;

BOOST_AUTO_TEST_SUITE(avr_instruction_test)

BOOST_AUTO_TEST_CASE( instruction_tests )
{
    AvrMachine mach, mach_expected;
    Instruction *insn;

    insn = new Avr_add();
    auto slots = insn->getSlots();
    slots[0]->setValue(0);
    slots[1]->setValue(0);

    mach.setRegisterValue(0, 0xA);
    insn->execute(&mach, &slots[0]);
    BOOST_REQUIRE(mach.getRegisterValue(0) == 0xA*2);

    mach.setRegisterValue(0, 0xF0);
    insn->execute(&mach, &slots[0]);
    BOOST_REQUIRE(mach.getRegisterValue(0) == ((0xF0*2)&0xFF));

    mach_expected.setRegisterValue(0, ((0xF0*2)&0xFF));
    BOOST_REQUIRE(mach_expected.equivalentState(mach) == true);

    ////////////////////////////////////////////////////////////
}

BOOST_AUTO_TEST_CASE( adc_test_1 )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_adc();
    auto slots = insn->getSlots();
    slots[0]->setValue(0);
    slots[1]->setValue(0);

    mach.setRegisterValue(0, 0x10);
    insn->execute(&mach, &slots[0]);
    BOOST_CHECK(mach.getRegisterValue(0) == 0x20);
}

BOOST_AUTO_TEST_CASE( adc_test_2 )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_adc();
    auto slots = insn->getSlots();
    slots[0]->setValue(0);
    slots[1]->setValue(0);

    mach.setRegisterValue(0, 0x10);
    mach.setFlagValue(AvrRegisterSlot::C, 1);
    insn->execute(&mach, &slots[0]);
    BOOST_CHECK(mach.getRegisterValue(0) == 0x21);
}

// Check the flags
BOOST_AUTO_TEST_CASE( add_test_1 )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_add();
    auto slots = insn->getSlots();
    slots[0]->setValue(0);
    slots[1]->setValue(0);

    mach.setRegisterValue(0, 0x90);
    mach.setFlagValue(AvrRegisterSlot::C, 0);
    insn->execute(&mach, &slots[0]);
    BOOST_CHECK(mach.getRegisterValue(0) == 0x20);
    BOOST_CHECK(mach.getFlagValue(AvrRegisterSlot::C) == 1);
    BOOST_CHECK(mach.getFlagValue(AvrRegisterSlot::V) == 1);
}

BOOST_AUTO_TEST_CASE( add_test_2 )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_add();
    auto slots = insn->getSlots();
    slots[0]->setValue(0);
    slots[1]->setValue(0);

    mach.setRegisterValue(0, 0xC0);
    mach.setFlagValue(AvrRegisterSlot::C, 0);
    insn->execute(&mach, &slots[0]);
    BOOST_CHECK(mach.getRegisterValue(0) == 0x80);
    BOOST_CHECK(mach.getFlagValue(AvrRegisterSlot::C) == 1);
    BOOST_CHECK(mach.getFlagValue(AvrRegisterSlot::V) == 0);
}

BOOST_AUTO_TEST_CASE( adiw_test )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_adiw();
    auto slots = insn->getSlots();
    slots[0]->setValue(24);
    slots[1]->setValue(0x18);

    mach.setRegisterValue(24, 0xF0);
    mach.setRegisterValue(25, 0x10);
    insn->execute(&mach, &slots[0]);
    BOOST_CHECK(mach.getRegisterValue(24) == 0x08);
    BOOST_CHECK(mach.getRegisterValue(25) == 0x11);
}

BOOST_AUTO_TEST_CASE( adiw_state_test )
{
    AvrMachine mach, mach_expected;
    auto insn = new Avr_adiw();
    auto slots = insn->getSlots();

    vector<Instruction*> insns;
    insns.push_back(insn);

    slots[0]->setValue(24);
    slots[1]->setValue(0x18);

    mach.setRegisterValue(24, 0xF0);
    mach.setRegisterValue(25, 0x10);

    mach_expected.setRegisterValue(24, 0x08);
    mach_expected.setRegisterValue(25, 0x11);
    BOOST_CHECK(testEquivalence(insns, slots, mach, mach_expected) == true);
}

BOOST_AUTO_TEST_CASE( adiw_lambda_test )
{
    AvrMachine mach, mach_expected;

    vector<Instruction*> insns;
    insns.push_back(new Avr_movw());
    insns.push_back(new Avr_adiw());

    auto slots = insns[0]->getSlots();
    auto s2 = insns[1]->getSlots();
    slots.insert(slots.end(), s2.begin(), s2.end());

    function<void(AvrMachine&)> goal(
        [](AvrMachine &mach){
            uint16_t tmp = (mach.getRegisterValue(1) << 8) | mach.getRegisterValue(0);
            tmp += 0x10;

            mach.setRegisterValue(0, tmp & 0xFF);
            mach.setRegisterValue(1, (tmp >> 8) & 0xFF);
        }
    );

    // movw r24, r0
    slots[0]->setValue(24);
    slots[1]->setValue(0);
    // adiw r24, 0x10
    slots[2]->setValue(24);
    slots[3]->setValue(0x10);

    mach.setRegisterValue(0, 0xAB);
    mach.setRegisterValue(1, 0xCD);

    mach_expected = mach;
    goal(mach_expected);

    BOOST_CHECK(testEquivalence(insns, slots, mach, mach_expected) == true);

    // BOOST_CHECK(testEquivalence(insns, slots, mach, goal) == true);
}

BOOST_AUTO_TEST_CASE( machine_tests )
{
    AvrMachine mach, mach2;

    mach.setRegisterValue(0, 0xFF);
    BOOST_REQUIRE(mach.getRegisterValue(0) == 0xFF);

    mach2.setRegisterValue(0, 0xFF);
    BOOST_REQUIRE(mach == mach2);
    BOOST_REQUIRE(mach.equivalentState(mach2) == true);

    mach2.setRegisterValue(1, 0xAA);
    BOOST_REQUIRE(!(mach == mach2));
    BOOST_REQUIRE(mach.equivalentState(mach2) == true);

    mach.setRegisterValue(1, 0xAB);
    BOOST_REQUIRE(mach.equivalentState(mach2) == false);

}


BOOST_AUTO_TEST_CASE( machine_permutation_tests )
{
    {
        AvrMachine mach, mach2;

        mach.setRegisterValue(0, 0xFF);
        mach2.setRegisterValue(1,0xFF);
        BOOST_REQUIRE(mach.containsState(mach2) == true);
    }

    {
        AvrMachine mach, mach2;
        mach.setRegisterValue(1, 0xAB);
        mach2.setRegisterValue(0, 0xAB);
        BOOST_REQUIRE(mach.containsState(mach2) == true);
    }

    {
        AvrMachine mach, mach2;
        mach.setRegisterValue(4, 0x12);
        mach2.setRegisterValue(3, 0x12);
        BOOST_REQUIRE(mach.containsState(mach2) == true);
    }

    {
        AvrMachine mach, mach2;
        mach.setRegisterValue(4, 0x12);
        mach2.setRegisterValue(3, 0x11);
        BOOST_REQUIRE(mach.containsState(mach2) == false);
    }

    {
        AvrMachine mach, mach2;
        mach.setRegisterValue(0, 0xAB);
        mach.setRegisterValue(1, 0xCD);
        mach2.setRegisterValue(24, 0xAB);
        mach2.setRegisterValue(25, 0xCD);
        BOOST_REQUIRE(mach.containsState(mach2) == true);
    }
}

BOOST_AUTO_TEST_SUITE_END()
