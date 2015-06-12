#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#include "algorithms/canonical.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 3)
    {
        cout << "Usage:\n\tcanonical_speed N_SLOTS N_TESTS\n";
        return 1;
    }

    unsigned n_tests = stoi(argv[2]);
    unsigned n_slots = stoi(argv[1]);

    vector<Slot *> slots;
    clock_t start, end;
    vector<unsigned> vals;

    for(unsigned i = 0; i < n_slots; ++i)
        vals.push_back(i);

    for(unsigned i = 0; i < n_slots; ++i)
    {
        RegisterSlot *rs = new RegisterSlot();

        rs->setValidArguments(vals);
        rs->setValue(0);
        slots.push_back(rs);
    }

    cout << "Starting nextCanonical speed test...        " << flush;
    start = clock();
    auto canonical_skips = initialiseCanonical(slots);
    for(unsigned i = 0; i < n_tests; ++i)
    {
        nextCanonical(slots, canonical_skips);
    }
    end = clock();

    double first_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << first_time << endl;


    for(unsigned i = 0; i < n_slots; ++i)
    {
        slots[i]->setValue(0);
    }

    cout << "Starting nextCanonicalBasic speed test...   " << flush;
    start = clock();
    for(unsigned i = 0; i < n_tests; ++i)
    {
        nextCanonicalBasic(slots);
    }
    end = clock();

    double second_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << second_time << endl;

    cout << "Basic is " << first_time/second_time << " times faster." << endl;

    return 0;
}
