#ifndef __AVR_HPP__
#define __AVR_HPP__

#include "../frontend.hpp"
// #include "../uops.hpp"
#include "../slots.hpp"
#include <vector>
#include <string>
#include <string.h>

class AvrMachine : public TargetMachine
{
public:
    AvrMachine()
    {
        for(int i = 0; i < 32; ++i)
            registers[i] = 0;
    }

    uint8_t getRegister(Slot &reg)
    {
        return registers[reg.getValue()];
    }

    void setRegister(Slot &reg, uint8_t value)
    {
        registers[reg.getValue()] = value;
    }


    uint8_t getRegisterValue(unsigned reg)
    {
        return registers[reg];
    }

    void setRegisterValue(unsigned reg, uint8_t value)
    {
        registers[reg] = value;
    }

    std::string toString()
    {
        std::string s;

        for(int i = 0; i < 32; ++i)
        {
            s += "\t";
            s += "r" + std::to_string(i) + " = " + std::to_string(registers[i]) + "  ";
            if(i % 4 == 3)
                s += "\n";
        }

        return s;
    }

private:
    uint8_t registers[32];
};

enum AvrRegisterClasses
{
    ALL_REGISTERS,
    REGISTERS_16PLUS,
};

class AvrAdd : public Instruction
{
public:
    // Uops generateUops()
    // {
    //     // TODO need to define our backend somewhere
    //     auto bb = backend->newBasicBlock();

    //     auto reg_slot1 = backend->newRegisterSlot(AvrRegisterClasses::ALL_REGISTERS);
    //     auto reg_slot2 = backend->newRegisterSlot(AvrRegisterClasses::ALL_REGISTERS);

    //     bb->start();
    //     bb->add_r8(reg_slot1, reg_slot1, reg_slot2);
    //     bb->end();
    // }

    void execute(TargetMachine *_mach, std::vector<Slot> &slots)
    {
        // assert slots.size() == 2
        // assert slot types are RegisterSlots

        AvrMachine *mach = static_cast<AvrMachine*>(_mach);
        unsigned char rA = mach->getRegister(slots[0]);
        unsigned char rB = mach->getRegister(slots[1]);

        rA = rA + rB;

        mach->setRegister(slots[0], rA);
    }

    // Return the number and types of register slots required
    //    which ones are read and written
    std::vector<Slot> getSlots()
    {
        return {RegisterSlot(AvrRegisterClasses::ALL_REGISTERS, true),
            RegisterSlot(AvrRegisterClasses::ALL_REGISTERS)};
    }

    std::string toString()
    {
        return "add S1, S2";
    }

    std::string toString(std::vector<Slot> &slots)
    {
        return std::string("add r") + std::to_string(slots[0].getValue()) + ", r" + std::to_string(slots[1].getValue());
    }
};


#endif
