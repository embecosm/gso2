#include <algorithm>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

#include "algorithms/canonicalIterator.hpp"
#include "algorithms/bruteforce.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        cout << "Usage:\n\tcanonical_speed N_SLOTS [MAXREG] [MODIFIERS NLIVE]\n";
        return 1;
    }

    unsigned n_slots = stoi(argv[1]);

    unsigned max_n = n_slots;
    if(argc > 2)
        max_n = stoi(argv[2]);
    unsigned n_live = 2;
    string modifier;
    if(argc > 4)
    {
        modifier = argv[3];
        n_live = stoi(argv[4]);
        if(modifier.size() != n_slots)
        {
            cout << "The number of characters in the modifier should be equal to the number of slots\n";
            return 1;
        }
    }

    vector<Slot *> slots;
    clock_t start, end;
    vector<unsigned> vals;

    for(unsigned i = 0; i < max_n; ++i)
        vals.push_back(i);

    for(unsigned i = 0; i < n_slots; ++i)
    {
        RegisterSlot *rs = new RegisterSlot(modifier[i] == 'w', modifier[i] == 'r');

        rs->setValidArguments(vals);
        rs->setValue(0);
        rs->setRegisterClassID(0);
        slots.push_back(rs);
    }

    unsigned n_tests_live=0, n_tests=0, n_tests_basic=0, n_tests_bruteforce=0;

    cout << "Starting nextCanonicalLiveness speed test... " << flush;
    start = clock();
    canonicalIteratorLiveness c_iter_live(slots, n_live);

    while(c_iter_live.next())
        n_tests_live++;

    end = clock();

    double first_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << first_time << endl;

    for(unsigned i = 0; i < n_slots; ++i)
    {
        slots[i]->setValue(0);
    }

    cout << "Starting nextCanonical speed test...         " << flush;
    start = clock();
    canonicalIteratorGeneric c_iter(slots);
    while(c_iter.next())
        n_tests++;
    end = clock();

    double second_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << second_time << endl;

    for(unsigned i = 0; i < n_slots; ++i)
    {
        slots[i]->setValue(0);
    }

    cout << "Starting nextCanonicalBasic speed test...    " << flush;
    start = clock();
    canonicalIteratorBasic c_iter_basic(slots);
    while(c_iter_basic.next())
        n_tests_basic++;
    end = clock();

    double third_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << third_time << endl;

    cout << "Starting bruteforce tests...                 " << flush;
    vector<vector<unsigned>::iterator> indices;

    for(unsigned i = 0; i < n_slots; ++i)
    {
        indices.push_back(vals.begin());
    }

    start = clock();
    while(bruteforceIterate(vals, indices))
        n_tests_bruteforce++;
    end = clock();

    double fourth_time = (end - start) / (double) CLOCKS_PER_SEC;
    cout << fourth_time << endl;

    cout << "Speed ups:\n";
    cout << "   Live   vs bruteforce: " << fourth_time/first_time << endl;
    cout << "   Normal vs bruteforce: " << fourth_time/second_time << endl;
    cout << "   Basic  vs bruteforce: " << fourth_time/third_time << endl;

    cout << "Number of tests:\n";
    cout << "Live:   " << n_tests_live << endl;
    cout << "Normal: " << n_tests << endl;
    cout << "Basic:  " << n_tests_basic << endl;
    cout << "Brute:  " << n_tests_bruteforce << endl;

    return 0;
}
