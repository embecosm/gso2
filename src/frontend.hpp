#ifndef __FRONTEND_HPP__
#define __FRONTEND_HPP__

#include <vector>
#include "slots.hpp"

class Slot;
class TargetMachineBase;
class Backend;

// A base class for each instruction we can emulate
class Instruction
{
public:
    // Instruction();
    // virtual ~Instruction();

    virtual unsigned execute(TargetMachineBase *_mach, Slot **slots) = 0;
    virtual std::vector<Slot*> getSlots() {return {};};


    virtual std::string toString() { return ""; };
    virtual std::string toString(Slot **slots) { return "";};
    virtual std::string getName() { return "";};

    // virtual Uops generateUops() = 0;
};

class TargetMachineBase
{

};

template <typename RegisterType, unsigned NumberOfRegisters>
class TargetMachine : public TargetMachineBase
{
public:
    TargetMachine()
    {
        for(unsigned i = 0; i < NumberOfRegisters; ++i)
        {
            registers[i] = 0;
            register_written[i] = false;
            register_read[i] = false;
        }
    }

    RegisterType getRegister(Slot *reg)
    {
        register_read[reg->getValue()] = true;
        return registers[reg->getValue()];
    }

    void setRegister(Slot *reg, RegisterType value)
    {
        register_written[reg->getValue()] = true;
        registers[reg->getValue()] = value;
    }


    RegisterType getRegisterValue(unsigned reg)
    {
        register_read[reg] = true;
        return registers[reg];
    }

    void setRegisterValue(unsigned reg, RegisterType value)
    {
        register_written[reg] = true;
        registers[reg] = value;
    }

    // virtual ~TargetMachine();


    // virtual void setBackend(Backend *backend);

// protected:
    Backend *backend;

protected:
    RegisterType registers[NumberOfRegisters];
    bool register_written[NumberOfRegisters];
    bool register_read[NumberOfRegisters];
};

#endif
