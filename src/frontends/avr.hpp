#ifndef __AVR_HPP__
#define __AVR_HPP__

#include "../frontend.hpp"
#include "../uops.hpp"

class AvrMachine : public TargetMachine
{
public:
//     AvrMachine();
//     ~AvrMachine();

};

enum AvrRegisterClasses
{
    ALL_REGISTERS,
    REGISTERS_16PLUS,
};

class AvrAdd : public Instruction
{
public:
    Uops generateUops()
    {
        // TODO need to define our backend somewhere
        auto bb = backend->newBasicBlock();

        auto reg_slot1 = backend->newRegisterSlot(AvrRegisterClasses::ALL_REGISTERS);
        auto reg_slot2 = backend->newRegisterSlot(AvrRegisterClasses::ALL_REGISTERS);

        bb->start();
        bb->add_r8(reg_slot1, reg_slot1, reg_slot2);
        bb->end();
    }
};


#endif
