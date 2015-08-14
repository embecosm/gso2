#define BOOST_TEST_MODULE canonical test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <map>

#include "algorithms/canonical.hpp"

using namespace std;

template class std::vector<Slot*>;

vector<unsigned> getSlotValues(const vector<Slot*> slots)
{
    vector<unsigned> values;

    for(auto s: slots)
    {
        values.push_back(s->getValue());
    }

    return values;
}

void setupSlots(vector<Slot*> slots, vector<vector<unsigned>> valids, vector<unsigned> class_ids={})
{
    BOOST_REQUIRE_EQUAL(slots.size(), valids.size());

    if(class_ids.size() != slots.size())
    {
        std::map<vector<unsigned>, unsigned> id_map;
        unsigned n_id = 0;

        for(unsigned i = 0; i < slots.size(); ++i)
        {
            auto it = id_map.find(valids[i]);

            if(it != id_map.end())
            {
                class_ids.push_back(it->second);
            }
            else
            {
                class_ids.push_back(n_id);
                id_map[valids[i]] = n_id++;
            }
        }
    }

    for(unsigned i = 0; i < slots.size(); i++)
    {
        RegisterSlot *rs = (RegisterSlot*) slots[i];

        rs->setValidArguments(valids[i]);
        rs->setValue(*valids[i].begin());
        rs->setRegisterClassID(class_ids[i]);
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

    canonicalIteratorGeneric c_iter(slots);

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

void checkTestDataUnordered(vector<Slot *> slots, string filename)
{
    ifstream infile(filename);
    string start;
    unsigned n_tests;

    BOOST_REQUIRE_MESSAGE(getline(infile, start),
        "Test data file \"" + filename + "\" is corrupt at the start");
    n_tests = stoi(start);

    canonicalIteratorGeneric c_iter(slots);

    vector<vector<unsigned>> correct_sequences;
    vector<vector<unsigned>> computed_sequences;

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

        correct_sequences.push_back(seq);
    }

    // TODO first is dup
    do {
        computed_sequences.push_back(getSlotValues(slots));
    } while(c_iter.next());

    sort(correct_sequences.begin(), correct_sequences.end());
    sort(computed_sequences.begin(), computed_sequences.end());

    if(correct_sequences != computed_sequences)
    {
        cout << "Correct sequences has " << correct_sequences.size() << " elements\n";
        cout << "Computed sequences has " << computed_sequences.size() << " elements\n";

        vector<vector<unsigned>> diff;
        set_symmetric_difference(correct_sequences.begin(), correct_sequences.end(),
            computed_sequences.begin(), computed_sequences.end(), back_inserter(diff));

        cout << "Differences:\n";
        for(auto d: diff)
        {
            cout << "   ";
            for(auto v: d)
                cout << v<< " ";
            cout << endl;
        }
    }

    BOOST_REQUIRE_MESSAGE(correct_sequences == computed_sequences, filename);

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

        if(getSlotValues(slots) != seq)
        {
            cout << "Expected: ";
            for(auto s: seq)
                cout << s << " ";
            cout << endl;

            cout << "Got     : ";
            for(auto s: getSlotValues(slots))
                cout << s << " ";
            cout << endl;
        }

        BOOST_REQUIRE_EQUAL(seq.size(), slots.size());
        BOOST_REQUIRE(getSlotValues(slots) == seq);

        c_iter.next();
    }
}

BOOST_AUTO_TEST_CASE( standard_tests_1 )
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

BOOST_AUTO_TEST_CASE( standard_tests_2 )
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

BOOST_AUTO_TEST_CASE( single_tests_1 )
{ // Four slots, single values
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot()};

    setupSlots(slots, {
            {0},
            {0},
            {0},
            {0}}
        );

    canonicalIteratorGeneric c_iter(slots);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
}

BOOST_AUTO_TEST_CASE( single_tests_2 )
{ // Four slots, single values
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot()};

    setupSlots(slots, {
            {1},
            {3},
            {2},
            {5}}
        );

    canonicalIteratorGeneric c_iter(slots);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,3,2,5}));
}

BOOST_AUTO_TEST_CASE( sparse_tests_1 )
{ // Three slots, simple
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    setupSlots(slots, {
            {0,1},
            {0,},
            {0,1}}
        );

    canonicalIteratorGeneric c_iter(slots);

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

BOOST_AUTO_TEST_CASE( sparse_tests_2)
{ // Three slots, simple
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    setupSlots(slots, {
            {0,1},
            {0,2},
            {0,1}}
        );

    canonicalIteratorGeneric c_iter(slots);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1}));
    c_iter.next();
    BOOST_CHECK(getSlotValues(slots) == vector<unsigned>({0,2,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0}));
    c_iter.next();

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
}

BOOST_AUTO_TEST_CASE( sparse_tests_3 )
{ // Four slots, simple
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot()};

    setupSlots(slots, {
            {0,1},
            {0,2},
            {0,2,3},
            {0,1}}
        );

    canonicalIteratorGeneric c_iter(slots);

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
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,2,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,3,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,2,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,2,3,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
}

BOOST_AUTO_TEST_CASE( sparse_tests_4 )
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

    checkTestDataUnordered(slots, "data/sparse_4.csv");
}

BOOST_AUTO_TEST_CASE( sparse_tests_5 )
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

    checkTestDataUnordered(slots, "data/sparse_5.csv");
}


BOOST_AUTO_TEST_CASE( long_tests_1 )
{ // 12 slots, limited values
    vector<Slot *> slots;

    for(int i = 0; i < 12; ++i)
    {
        RegisterSlot *rs = new RegisterSlot();
        rs->setValidArguments({0,1,2,3});
        rs->setValue(0);
        rs->setRegisterClassID(0);
        slots.push_back(rs);
    }

    checkTestData(slots, "data/long_1.csv");
}

BOOST_AUTO_TEST_CASE( long_tests_2 )
{ // 20 slots, very limited values
    vector<Slot *> slots;

    for(int i = 0; i < 20; ++i)
    {
        RegisterSlot *rs = new RegisterSlot();
        rs->setValidArguments({0,1});
        rs->setValue(0);
        rs->setRegisterClassID(0);
        slots.push_back(rs);
    }

    checkTestData(slots, "data/long_2.csv");
}

BOOST_AUTO_TEST_CASE( basic_tests_1 )
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

BOOST_AUTO_TEST_CASE( basic_tests_2 )
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


BOOST_AUTO_TEST_CASE( basic_prematch_tests )
{   // Three slots, too manyvalues
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    setupSlots(slots, {
            {0,1,2,3},
            {0,1,2,3},
            {0,1,2,3}}
        );

    canonicalIteratorBasic c_iter(slots, 0);

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
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,0,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,1,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,1,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({1,2,3}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
}

BOOST_AUTO_TEST_CASE( classid_tests_1 )

{   // Four slots, full range of values
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot()};

    ((RegisterSlot*)slots[0])->setRegisterClassID(0);
    ((RegisterSlot*)slots[1])->setRegisterClassID(0);
    ((RegisterSlot*)slots[2])->setRegisterClassID(0);
    ((RegisterSlot*)slots[3])->setRegisterClassID(0);

    setupSlots(slots, {
            {0,1,2,3},
            {0,1,2,3},
            {0,1,2,3},
            {0,1,2,3}}
        );

    checkTestData(slots, "data/standard_1.csv");
}

BOOST_AUTO_TEST_CASE( classid_tests_2 )
{ // Eight slots, full range of values
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

    ((RegisterSlot*)slots[0])->setRegisterClassID(0);
    ((RegisterSlot*)slots[1])->setRegisterClassID(0);
    ((RegisterSlot*)slots[2])->setRegisterClassID(0);
    ((RegisterSlot*)slots[3])->setRegisterClassID(0);
    ((RegisterSlot*)slots[4])->setRegisterClassID(0);
    ((RegisterSlot*)slots[5])->setRegisterClassID(0);
    ((RegisterSlot*)slots[6])->setRegisterClassID(0);
    ((RegisterSlot*)slots[7])->setRegisterClassID(0);

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

    checkTestData(slots, "data/standard_2.csv");
}

BOOST_AUTO_TEST_CASE( overspecified_tests )
{   // Three slots, too manyvalues
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    setupSlots(slots, {
            {0,1,2,3,5},
            {0,1,2,3,4},
            {0,1,2,3,7}}
        );

    canonicalIteratorGeneric c_iter(slots);

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

// Test the skipping of slots which are not register slots
BOOST_AUTO_TEST_CASE( slot_tests_1 )
{   // Three slots, too manyvalues
    vector<Slot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new Slot(), new RegisterSlot(), new Slot()};

    ((RegisterSlot*)slots[0])->setValidArguments({0,1,2});
    ((RegisterSlot*)slots[1])->setValidArguments({0,1,2});
    ((RegisterSlot*)slots[3])->setValidArguments({0,1,2});

    slots[0]->setValue(0);
    slots[1]->setValue(0);
    slots[3]->setValue(0);

    canonicalIteratorGeneric c_iter(slots);

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

BOOST_AUTO_TEST_CASE( slot_tests_2 )
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

vector<unsigned> possibleRegisters(vector<RegisterSlot*> &slotlist, vector<unsigned> &values, unsigned val);

BOOST_AUTO_TEST_CASE( possible_tests )
{
    {   // Test the possibleRegisters function, for remappings
        vector<RegisterSlot *> slots = {new RegisterSlot(), new RegisterSlot(),
            new RegisterSlot()};

        slots[0]->setValidArguments({0,1,2,3,4});
        slots[1]->setValidArguments({0,1,5});
        slots[2]->setValidArguments({0,1,4});

        slots[0]->setValue(0);
        slots[1]->setValue(0);
        slots[2]->setValue(1);

        auto value_list = vector<unsigned>({0,0,1});

        auto p = possibleRegisters(slots, value_list, 0);

        BOOST_REQUIRE(p == vector<unsigned>({0,1}));
    }
}

BOOST_AUTO_TEST_CASE( mapping_tests_1 )
{   // 5 slots, simple mapping
    vector<RegisterSlot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot(), new RegisterSlot(), new RegisterSlot()};

    slots[0]->setValidArguments({0,1,2,3});
    slots[1]->setValidArguments({0,1,2,3});
    slots[2]->setValidArguments({0,1,2,3});
    slots[3]->setValidArguments({0,1,2,3});
    slots[4]->setValidArguments({0,1,2,3});

    slots[0]->setValue(0);
    slots[1]->setValue(0);
    slots[2]->setValue(0);
    slots[3]->setValue(0);
    slots[4]->setValue(0);

    auto mapped = canonicalMapping(slots, {0,0,0,0,1});
    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({0,0,0,0,1}));

    auto mapped2 = canonicalMapping(slots);
    BOOST_REQUIRE(mapped2.second == true);
    BOOST_REQUIRE(mapped2.first == vector<unsigned>({0,0,0,0,0}));
}

BOOST_AUTO_TEST_CASE( mapping_tests_2 )
{   // Three slots, more complex mappings
    vector<RegisterSlot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    slots[0]->setValidArguments({0,1,2,3});
    slots[1]->setValidArguments({0,1,2});
    slots[2]->setValidArguments({0,2,3});

    slots[0]->setValue(1);
    slots[1]->setValue(1);
    slots[2]->setValue(0);

    auto mapped = canonicalMapping(slots);
    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({0,0,2}));
}

BOOST_AUTO_TEST_CASE( mapping_tests_3 )
{   // Three slots, more complex mappings
    vector<RegisterSlot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    slots[0]->setValidArguments({0,1,2,3});
    slots[1]->setValidArguments({1,2});
    slots[2]->setValidArguments({0,2,3});

    slots[0]->setValue(0);
    slots[1]->setValue(0);
    slots[2]->setValue(0);

    auto mapped = canonicalMapping(slots);
    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({2,2,2}));
}

BOOST_AUTO_TEST_CASE( mapping_tests_4 )
{   // Three slots, more complex mappings
    vector<RegisterSlot *> slots = {new RegisterSlot(), new RegisterSlot(),
        new RegisterSlot()};

    slots[0]->setValidArguments({1,2,3});
    slots[1]->setValidArguments({0,1});
    slots[2]->setValidArguments({0,2,3});

    slots[0]->setValue(0);
    slots[1]->setValue(0);
    slots[2]->setValue(0);

    auto mapped = canonicalMapping(slots);
    BOOST_REQUIRE(mapped.second == false);
}

BOOST_AUTO_TEST_CASE( liveness_mapping_tests_1 )
{
    vector<RegisterSlot *> slots = {
        new RegisterSlot(false, true),
        new RegisterSlot(false, true),
        new RegisterSlot(true,  false)
    };

    slots[0]->setValidArguments({0,1,2});
    slots[1]->setValidArguments({0,1,2});
    slots[2]->setValidArguments({0,1,2});

    auto value_list = vector<unsigned>({0,0,0});

    auto mapped = canonicalMappingLive(slots, value_list, 1);

    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({0,0,0}));
}

BOOST_AUTO_TEST_CASE( liveness_mapping_tests_2 )
{
    vector<RegisterSlot *> slots = {
        new RegisterSlot(false, true),
        new RegisterSlot(false, true),
        new RegisterSlot(true,  false)
    };

    slots[0]->setValidArguments({0,1,2});  // read
    slots[1]->setValidArguments({0,1,2});  // read
    slots[2]->setValidArguments({0,1,2});  // write

    auto value_list = vector<unsigned>({0,1,2});

    auto mapped = canonicalMappingLive(slots, value_list, 1);

    BOOST_REQUIRE(mapped.second == false);
}

BOOST_AUTO_TEST_CASE( liveness_mapping_tests_3 )
{
    vector<RegisterSlot *> slots = {
        new RegisterSlot(false, true),
        new RegisterSlot(false, true),
        new RegisterSlot(true,  false)
    };

    slots[0]->setValidArguments({0,1,2});  // read
    slots[1]->setValidArguments({0,1,2});  // read
    slots[2]->setValidArguments({0,1,2});  // write

    auto value_list = vector<unsigned>({1,1,0});

    auto mapped = canonicalMappingLive(slots, value_list, 1);

    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({0,0,1}));
}

BOOST_AUTO_TEST_CASE( liveness_mapping_tests_4 )
{
    vector<RegisterSlot *> slots = {
        new RegisterSlot(false, true),
        new RegisterSlot(false, true),
        new RegisterSlot(true,  true)
    };

    slots[0]->setValidArguments({0,1,2});  // read
    slots[1]->setValidArguments({0,1,2});  // read
    slots[2]->setValidArguments({0,2});  // write

    auto value_list = vector<unsigned>({1,1,0});

    pair<vector<unsigned>,bool> mapped;

    mapped = canonicalMappingLive(slots, value_list, 1);

    BOOST_REQUIRE(mapped.second == false);

    mapped = canonicalMappingLive(slots, value_list, 2);

    BOOST_REQUIRE(mapped.second == true);
    BOOST_REQUIRE(mapped.first == vector<unsigned>({0,0,2}));
}

BOOST_AUTO_TEST_CASE( liveness_mapping_tests_5 )
{
    vector<RegisterSlot *> slots = {
        new RegisterSlot(true,  false),
        new RegisterSlot(false, true),
        new RegisterSlot(false, true)
    };

    slots[0]->setValidArguments({0,1,2});  // write
    slots[1]->setValidArguments({0,1,2});  // read
    slots[2]->setValidArguments({0,1,2});  // read

    auto value_list = vector<unsigned>({0,1,0});

    auto mapped = canonicalMappingLive(slots, value_list, 0);

    BOOST_REQUIRE(mapped.second == false);
}

BOOST_AUTO_TEST_CASE( liveness_standard_tests_1 )
{
    vector<Slot *> slots = {
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2})}; // r

    canonicalIteratorLiveness c_iter(slots, 1);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0}));
    c_iter.next();
}

BOOST_AUTO_TEST_CASE( liveness_standard_tests_2 )
{
    vector<Slot *> slots = {
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2})}; // r

    canonicalIteratorLiveness c_iter(slots, 2);

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
    c_iter.next();
}

BOOST_AUTO_TEST_CASE( liveness_standard_tests_3 )
{
    vector<Slot *> slots = {
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(false, true, {0,1,2})}; // r

    canonicalIteratorLiveness c_iter(slots, 1);

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
    c_iter.next();
}

BOOST_AUTO_TEST_CASE( liveness_standard_tests_4 )
{
    vector<Slot *> slots = {
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2})}; // r

    canonicalIteratorLiveness c_iter(slots, 1);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1,1}));
    c_iter.next();
}

BOOST_AUTO_TEST_CASE( liveness_standard_tests_5 )
{
    vector<Slot *> slots = {
        new RegisterSlot(true, false, {0,1,2}),  // w
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2}),  // r
        new RegisterSlot(false, true, {0,1,2})}; // r

    canonicalIteratorLiveness c_iter(slots, 2);

    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,0,1,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,0,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,1,2}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,2,0}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,2,1}));
    c_iter.next();
    BOOST_REQUIRE(getSlotValues(slots) == vector<unsigned>({0,1,2,2}));
    c_iter.next();
}
