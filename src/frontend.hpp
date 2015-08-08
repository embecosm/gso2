#ifndef __FRONTEND_HPP__
#define __FRONTEND_HPP__

#include <vector>
#include "slots.hpp"
#include "utility.hpp"

class Slot;
class TargetMachineBase;
class Backend;

// A base class for each instruction we can emulate
class Instruction
{
public:
    // Instruction();
    virtual ~Instruction() {};

    virtual unsigned execute(TargetMachineBase *_mach, Slot **slots) = 0;
    virtual std::vector<Slot*> getSlots() {return {};};
    virtual unsigned getNumberOfSlots() = 0;


    virtual std::string toString() { return ""; };
    virtual std::string toString(Slot **slots) { return "";};
    virtual std::string getName() { return "";};

    virtual bool parse(std::string input, std::vector<Slot*> &slotlist) { return false;};
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

    void initialiseRandom()
    {
        for(int i = 0; i < NumberOfRegisters; ++i)
        {
            registers[i] = (RegisterType) rand();
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

    // Test whether the other state is the same. This is not commutative, since
    // it only checks the registers which have been written in this machine state
    bool equivalentState(TargetMachine &other)
    {
        bool equiv = true;

        for(unsigned i = 0; i < NumberOfRegisters; ++i)
            if(register_written[i] && registers[i] != other.registers[i])
                equiv = false;

        return equiv;
    }

    // Checks whether the other state is contained within the current state,
    // by getting working out whether there is a register mapping from this
    // state to the other. This works, even if additional register have been
    // written to in this state, and even if the register labels are not the
    // same.

    // List of which reference register, maps to the other register
    bool containsState(TargetMachine &other,
        std::vector<std::pair<unsigned,unsigned>> &mapping)
    {
        RegisterType reg_map[NumberOfRegisters];
        RegisterType reg_map_other[NumberOfRegisters];
        unsigned n_reg=0, n_reg_other=0;
        bool equiv;

        // If a mapping is specified, don't try to find one, just compare
        if(mapping.size() != 0)
        {
            for(auto &map: mapping)
            {
                if(registers[map.first] != other.registers[map.second] ||
                    !register_written[map.first] || !other.register_written[map.second])
                {
                    return false;
                }
            }
            return true;
        }

        // Fill in the array with the registers that are written
        for(unsigned i = 0; i < NumberOfRegisters; ++i)
        {
            if(register_written[i])
            {
                reg_map[n_reg++] = i;
            }
            if(other.register_written[i])
            {
                reg_map_other[n_reg_other++] = i;
            }
        }

        // If the other state requires more registers than we have, then the
        // states cannot be equivalent.
        if(n_reg_other > n_reg || n_reg_other == 0)
            return false;

        std::vector<unsigned> rm(reg_map, reg_map+n_reg);
        Combinations<unsigned> comb_iter(rm, n_reg_other);
        std::vector<unsigned> reg_list;

        do
        {
            comb_iter.getSelection(reg_list);

            do
            {
                equiv = true;
                for(unsigned i = 0; i < n_reg_other; ++i)
                {
                    if(registers[reg_map[reg_list[i]]] != other.registers[reg_map_other[i]])
                        equiv = false;
                }
                if(equiv)
                {
                    for(unsigned i = 0; i < n_reg_other; ++i)
                        mapping.push_back(std::make_pair(reg_map[reg_list[i]], reg_map_other[i]));
                    break;
                }
            } while(std::next_permutation(reg_list.begin(), reg_list.end()));
            if(equiv)
                break;
        } while(comb_iter.next());

        return equiv;
    }

    bool containsState(TargetMachine &other)
    {
        std::vector<std::pair<unsigned,unsigned>> temporary;
        return containsState(other, temporary);
    }


protected:
    RegisterType registers[NumberOfRegisters];
    bool register_written[NumberOfRegisters];
    bool register_read[NumberOfRegisters];
};

template <typename RegisterType, unsigned NumberOfRegisters, unsigned NumberOfFlags>
class TargetMachineWithFlags : public TargetMachine<RegisterType, NumberOfRegisters>
{
public:
    TargetMachineWithFlags() : TargetMachine<RegisterType, NumberOfRegisters>()
    {
        for(unsigned i = 0; i < NumberOfFlags; ++i)
            flags[i] = false;

    }

    void initialiseRandom()
    {
        TargetMachine<RegisterType, NumberOfRegisters>::initialiseRandom();

        for(int i = 0; i < NumberOfFlags; ++i)
        {
            flags[i] = rand() & 1;
        }
    }

    bool getFlagValue(unsigned reg)
    {
        return flags[reg];
    }

    void setFlagValue(unsigned reg, bool value)
    {
        flags[reg] = value;
    }

protected:
    bool flags[NumberOfFlags];
    // TODO possible extension: read and write status for flags
};

#endif
