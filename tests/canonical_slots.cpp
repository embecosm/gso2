#define BOOST_TEST_MODULE canonical test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>

#include "algorithms/canonical.hpp"

using namespace std;

template class std::vector<Slot*>;

vector<unsigned> getSlotValues(vector<Slot*> slots)
{
    vector<unsigned> values;

    for(auto s: slots)
    {
        values.push_back(s->getValue());
    }

    return values;
}

void setupSlots(vector<Slot*> slots, vector<vector<unsigned>> valids)
{
    BOOST_REQUIRE_EQUAL(slots.size(), valids.size());

    for(unsigned i = 0; i < slots.size(); i++)
    {
        RegisterSlot *rs = (RegisterSlot*) slots[i];

        rs->setValidArguments(valids[i]);
        rs->setValue(*valids[i].begin());
    }
}

void checkTestData(vector<Slot *> slots, string filename)
{
    ifstream infile(filename);
    string start;
    unsigned n_tests;

    BOOST_REQUIRE_MESSAGE(getline(infile, start),
        "Test data file \"" + filename + "\" is corrupt at the start");
    n_tests = stoi(start);

    canonicalIterator c_iter(slots);

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

        BOOST_REQUIRE_EQUAL(seq.size(), slots.size());

        if(getSlotValues(slots) != seq)
        {
            cout << "Error:\n\tGot   : ";
            for(auto s: getSlotValues(slots))
                cout << s << " ";
            cout << "\n\tWanted: ";
            for(auto s: seq)
                cout << s << " ";
            cout << endl;
            BOOST_REQUIRE_MESSAGE(getSlotValues(slots) == seq, filename);
        }

        c_iter.next();
    }
}

void checkTestDataBasic(vector<Slot *> slots, string filename)
{
    ifstream infile(filename);
    string start;
    unsigned n_tests;

    BOOST_REQUIRE_MESSAGE(getline(infile, start),
        "Test data file \"" + filename + "\" is corrupt at the start");
    n_tests = stoi(start);

    canonicalIteratorBasic c_iter(slots);

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

        // for(auto s: getSlotValues(slots))
        //     cout << s << " ";
        // cout << endl;

        BOOST_REQUIRE_EQUAL(seq.size(), slots.size());
        BOOST_REQUIRE(getSlotValues(slots) == seq);

        c_iter.next();
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

        checkTestData(slots, "data/standard_1.csv");
    }

    { // Eight slots, full range of values
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7},
                {0,1,2,3,4,5,6,7}}
            );

        checkTestData(slots, "data/standard_2.csv");
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

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
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

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
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

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    }

    { // Three slots, simple
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        setupSlots(slots, {
                {0,1},
                {0,2},
                {0,1}}
            );

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
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

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,2,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,2,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,3,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,3,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
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

        checkTestData(slots, "data/sparse_4.csv");
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

        checkTestData(slots, "data/sparse_5.csv");
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

        checkTestData(slots, "data/long_1.csv");
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

        checkTestData(slots, "data/long_2.csv");
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

        checkTestDataBasic(slots, "data/standard_1.csv");
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

        checkTestDataBasic(slots, "data/standard_2.csv");
    }
}

BOOST_AUTO_TEST_CASE( overspecified_tests )
{
    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        setupSlots(slots, {
                {0,1,2,3,5},
                {0,1,2,3,4},
                {0,1,2,3,7}}
            );

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,2}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    }
}

// Test the skipping of slots which are not register slots
BOOST_AUTO_TEST_CASE( slot_tests )
{
    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new Slot(), new RegisterSlot(), new Slot()};

        ((RegisterSlot*)slots[0])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[1])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[3])->setValidArguments({0,1,2});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[3]->setValue(0);

        canonicalIterator c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,1,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,1,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0,0}));
    }

    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new Slot(), new RegisterSlot(), new Slot()};

        ((RegisterSlot*)slots[0])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[1])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[3])->setValidArguments({0,1,2});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[2]->setValue(0);
        slots[3]->setValue(0);
        slots[4]->setValue(0);

        canonicalIteratorBasic c_iter(slots);

        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,1,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,0,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,1,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,2,0}));
        c_iter.next();
        BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0,0}));
    }
}

vector<unsigned> possibleRegisters(vector<Slot*> &slotlist, unsigned val);

BOOST_AUTO_TEST_CASE( possible_tests )
{
    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        ((RegisterSlot*)slots[0])->setValidArguments({0,1,2,3,4});
        ((RegisterSlot*)slots[1])->setValidArguments({0,1,4,2});
        ((RegisterSlot*)slots[2])->setValidArguments({0,1,4});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[2]->setValue(1);

        // auto p = possibleRegisters(slots, {0,1,2}, 0);

        // for(auto k: p)
        //     cout << k << " ";
        // cout << endl;
        //TODO
    }
}

BOOST_AUTO_TEST_CASE( mapping_tests )
{
    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        ((RegisterSlot*)slots[0])->setValidArguments({0,1,2,3});
        ((RegisterSlot*)slots[1])->setValidArguments({0,1,2,3});
        ((RegisterSlot*)slots[2])->setValidArguments({0,1,2,3});
        ((RegisterSlot*)slots[3])->setValidArguments({0,1,2,3});
        ((RegisterSlot*)slots[4])->setValidArguments({0,1,2,3});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[2]->setValue(0);
        slots[3]->setValue(0);
        slots[4]->setValue(0);

        auto mapped = canonicalMapping(slots, {0,0,0,0,1});

        cout << mapped.second << endl;
        for(auto reg: mapped.first)
            cout << reg << " ";
        cout << endl;
    }

    {   // Three slots, too manyvalues
        vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

        ((RegisterSlot*)slots[0])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[1])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[2])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[3])->setValidArguments({0,1,2});
        ((RegisterSlot*)slots[4])->setValidArguments({0,1,2});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[2]->setValue(0);
        slots[3]->setValue(0);
        slots[4]->setValue(0);

        canonicalIterator c_iter(slots);

        for(int i = 0; i < 10; ++i)
        {
            // for(auto r : getSlotValues(slots))
            //     cout << r << " ";
            // cout << endl;
            c_iter.next();
        }

    }
}
