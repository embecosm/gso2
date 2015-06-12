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
    AvrMachine mach;
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

    ////////////////////////////////////////////////////////////
}
