#include <iostream>

#include "frontends/avr.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // Parse options

    // Initialise for particular target

    // Initialise backend

    AvrMachine mach;
    AvrAdd add;

    auto slots1 = add.getSlots();
    auto slots2 = add.getSlots();

    slots1[0].setValue(0);
    slots1[1].setValue(2);

    slots2[0].setValue(2);
    slots2[1].setValue(1);

    mach.setRegisterValue(0, 0x1);
    mach.setRegisterValue(1, 0x2);
    mach.setRegisterValue(2, 0x4);
    mach.setRegisterValue(3, 0x8);

    add.execute(&mach, slots1);
    add.execute(&mach, slots2);

    cout << add.toString(slots1) << endl << add.toString(slots2) << endl;

    cout << hex << "0x" << (unsigned)mach.getRegisterValue(0) << endl;

    cout << mach.toString();

    return 0;
}
