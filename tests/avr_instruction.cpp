#define BOOST_TEST_MODULE avr instruction test
#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

#include "algorithms/canonical.hpp"
#include "frontends/avr.hpp"

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
        mach2.setRegisterValue(3, 0x11);
        BOOST_REQUIRE(mach.containsState(mach2) == false);
    }

}
