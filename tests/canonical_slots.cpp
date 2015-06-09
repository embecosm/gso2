#define BOOST_TEST_MODULE canonical test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

#include "algorithms/canonical.hpp"

using namespace std;

vector<unsigned> getSlotValues(vector<Slot*> slots)
{
    vector<unsigned> values;

    for(auto s: slots)
    {
        values.push_back(s->getValue());
    }

    return values;
}

void setupSlots(vector<Slot*> slots, vector<unordered_set<unsigned>> valids)
{
    BOOST_CHECK_EQUAL(slots.size(), valids.size());

    for(unsigned i = 0; i < slots.size(); i++)
    {
        RegisterSlot *rs = (RegisterSlot*) slots[i];

        rs->setValidArguments(valids[i]);
        rs->setValue(*min_element(valids[i].begin(), valids[i].end()));
    }
}

void checkTestData(vector<Slot *> slots, string filename, bool (*testFn)(vector<Slot*>))
{
    ifstream infile(filename);
    string start;
    unsigned n_tests;

    BOOST_REQUIRE_MESSAGE(getline(infile, start),
        "Test data file \"" + filename + "\" is corrupt at the start");
    n_tests = stoi(start);

    for(unsigned i = 0; i < n_tests; ++i)
    {
        string line;

        BOOST_REQUIRE_MESSAGE(getline(infile, line),
            "Test data file \"" + filename + "\" does not have enough entries");

        vector<unsigned> seq;
        istringstream ss(line);

        for(unsigned j = 0; j < slots.size(); ++j)
        {
            string token;

            BOOST_REQUIRE_MESSAGE(getline(ss, token, ','),
                "Test data file \"" + filename +
                "\" has incorrect number of entries for test "+to_string(i));
            seq.push_back(stoi(token));
        }

        BOOST_CHECK_EQUAL(seq.size(), slots.size());
        BOOST_CHECK(getSlotValues(slots) == seq);

        // nextCanonical(slots);
        (*testFn)(slots);
    }
}

BOOST_AUTO_TEST_CASE( standard_tests )
{
    { // Four slots, full range of values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3},
                {0,1,2,3},
                {0,1,2,3},
                {0,1,2,3}}
            );

        checkTestData(slots, "data/standard_1.csv", nextCanonical);
    }

    { // Eight slots, full range of values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8}}
            );

        checkTestData(slots, "data/standard_2.csv", nextCanonical);
    }
}

BOOST_AUTO_TEST_CASE( single_tests )
{
    { // Four slots, single values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0},
                {0},
                {0},
                {0}}
            );

        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    }

    { // Four slots, single values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {1},
                {3},
                {2},
                {5}}
            );

        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
    }
}

BOOST_AUTO_TEST_CASE( sparse_tests )
{
    { // Three slots, simple
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,},
                {0,1}}
            );

        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    }

    { // Three slots, simple
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,2},
                {0,1}}
            );

        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    }

    { // Four slots, simple
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,2},
                {0,2,3},
                {0,1}}
            );

        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,2,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,2,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,0,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,0,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,2,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,2,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,3,0}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,3,1}));
        nextCanonical(slots);
        BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    }

    { // 8 slots, limited values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,2},
                {0,2,3},
                {0,2,3},
                {0,1},
                {0,2},
                {0,1},
                {0,2,3}}
            );

        checkTestData(slots, "data/sparse_4.csv", nextCanonical);
    }

    { // 11 slots, limited values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,2},
                {1,2},
                {0,1,2},
                {1,2},
                {0,2},
                {0,1},
                {0,2},
                {0,1,2},
                {1,2},
                {0,1}}
            );

        checkTestData(slots, "data/sparse_5.csv", nextCanonical);
    }
}

BOOST_AUTO_TEST_CASE( long_tests )
{
    { // 12 slots, limited values
        vector<Slot *> slots;

        for(int i = 0; i < 12; ++i)
        {
            RegisterSlot *rs = new RegisterSlot();
            rs->setValidArguments({0,1,2,3});
            rs->setValue(0);
            slots.push_back(rs);
        }

        checkTestData(slots, "data/long_1.csv", nextCanonical);
    }

    { // 20 slots, very limited values
        vector<Slot *> slots;

        for(int i = 0; i < 20; ++i)
        {
            RegisterSlot *rs = new RegisterSlot();
            rs->setValidArguments({0,1});
            rs->setValue(0);
            slots.push_back(rs);
        }

        checkTestData(slots, "data/long_2.csv", nextCanonical);
    }
}

BOOST_AUTO_TEST_CASE( basic_tests )
{
    {   // Four slots, full range of values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3},
                {0,1,2,3},
                {0,1,2,3},
                {0,1,2,3}}
            );

        checkTestData(slots, "data/standard_1.csv", nextCanonicalBasic);
    }

    { // Eight slots, full range of values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8},
                {0,1,2,3,4,5,6,7,8}}
            );

        checkTestData(slots, "data/standard_2.csv", nextCanonicalBasic);
    }
}
