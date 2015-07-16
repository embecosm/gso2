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
    if(argc < 3)
    {
        cout << "Usage:\n\tcanonical_speed N_SLOTS N_TESTS [MAXREG] [NLIVE]\n";
        return 1;
    }

    unsigned n_tests = stoi(argv[2]);
    unsigned n_slots = stoi(argv[1]);

    unsigned max_n = n_slots;
    if(argc > 3)
        max_n = stoi(argv[3]);
    unsigned n_live = 2;
    if(argc > 4)
        n_live = stoi(argv[4]);

    vector<Slot *> slots;
    clock_t start, end;
    vector<unsigned> vals;

    for(unsigned i = 0; i < max_n; ++i)
        vals.push_back(i);

    for(unsigned i = 0; i < n_slots; ++i)
    {
        RegisterSlot *rs = new RegisterSlot();

        rs->setValidArguments(vals);
        rs->setValue(0);
        rs->setRegisterClassID(0);
        slots.push_back(rs);
    }

    cout << "Starting nextCanonicalLiveness speed test...        " << flush;
    start = clock();
    canonicalIteratorLiveness c_iter_live(slots, n_live);
    for(unsigned i = 0; i < n_tests; ++i)
    {
        c_iter_live.next();
    }
    end = clock();

    double first_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << first_time << endl;

    for(auto p: slots)
        cout << p->getValue() << " ";
    cout << "\n\t";

    for(unsigned i = 0; i < n_slots; ++i)
    {
        slots[i]->setValue(0);
    }

    cout << "Starting nextCanonical speed test...        " << flush;
    start = clock();
    canonicalIterator c_iter(slots);
    for(unsigned i = 0; i < n_tests; ++i)
    {
        c_iter.next();
    }
    end = clock();

    double second_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << second_time << endl;

    for(auto p: slots)
        cout << p->getValue() << " ";
    cout << "\n\t";

    for(unsigned i = 0; i < n_slots; ++i)
    {
        slots[i]->setValue(0);
    }

    cout << "Starting nextCanonicalBasic speed test...   " << flush;
    start = clock();
    canonicalIteratorBasic c_iter_basic(slots);
    for(unsigned i = 0; i < n_tests; ++i)
    {
        c_iter_basic.next();
    }
    end = clock();

    double third_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << third_time << endl;

    for(auto p: slots)
        cout << p->getValue() << " ";
    cout << "\n\t";


    cout << "Basic is " << second_time/third_time << " times faster." << endl;
    cout << "Basic is " << first_time/third_time << " times faster than liveness." << endl;

    return 0;
}
