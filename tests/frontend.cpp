#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "frontend.hpp"

BOOST_AUTO_TEST_SUITE(frontend_test)

BOOST_AUTO_TEST_CASE( target_machine_register_test )
{
    TargetMachine<uint32_t, 8> test_mach, expected_mach;

    for(int i = 0; i < 8; ++i)
        BOOST_CHECK(test_mach.getRegisterValue(i) == 0);

    BOOST_CHECK(test_mach.equivalentState(expected_mach) == true);

    test_mach.setRegisterValue(0, 0xFFFFFFFF);
    BOOST_CHECK(test_mach.getRegisterValue(0) == 0xFFFFFFFF);
    for(int i = 1; i < 8; ++i)
        BOOST_CHECK(test_mach.getRegisterValue(i) == 0);

    BOOST_CHECK(test_mach.equivalentState(expected_mach) == false);
}

BOOST_AUTO_TEST_CASE( target_machine_containsState_test )
{
    TargetMachine<uint32_t, 8> test_mach, expected_mach;

    expected_mach.setRegisterValue(0, 0xFFFFFFFF);
    test_mach.setRegisterValue(0, 0xFFFFFFFF);

    BOOST_CHECK(test_mach.containsState(expected_mach) == true);

    test_mach.setRegisterValue(1, 0xFFFFFFFF);
    BOOST_CHECK(test_mach.containsState(expected_mach) == true);

    test_mach.setRegisterValue(0, 0x0);
    BOOST_CHECK(test_mach.containsState(expected_mach) == true);

    test_mach.setRegisterValue(1, 0x0);
    BOOST_CHECK(test_mach.containsState(expected_mach) == false);

    expected_mach.setRegisterValue(0, 0xF00DCAFE);
    expected_mach.setRegisterValue(2, 0xBEEFCAFE);
    test_mach.setRegisterValue(0, 0x1);
    test_mach.setRegisterValue(1, 0x2);
    test_mach.setRegisterValue(2, 0xF00DCAFE);
    test_mach.setRegisterValue(3, 0x4);
    test_mach.setRegisterValue(4, 0xBEEFCAFE);
    BOOST_CHECK(test_mach.containsState(expected_mach) == true);
}

BOOST_AUTO_TEST_SUITE_END()
