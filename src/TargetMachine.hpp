#ifndef __TARGETMACHINE_H__
#define __TARGETMACHINE_H__

class TargetMachineBase
{

};

/*! \class TargetMachine

    This class holds the state of the processor - the register values. Each
    register also has a written and read bit associated with it, recording
    whether it has been read or changed. Methods are provided for accessing
    each, as well as comparing the state to other states. The class is
    templated, accepting a type for the registers and a number of registers
    for this state.

    This class is designed to be subclassed and customised for a specific
    processor.
*/
template <typename RegisterType, unsigned NumberOfRegisters>
class TargetMachine : public TargetMachineBase
{
public:
    /*! Construct the target machine

        Set all registers to zero, and mark all registers as not read and not
        written.
    */
    TargetMachine()
    {
        for(unsigned i = 0; i < NumberOfRegisters; ++i)
        {
            registers[i] = 0;
            register_written[i] = false;
            register_read[i] = false;
        }
    }

    /*! Initialise the state of the TargetMachine to random.

        All registers are marked as unread and unwritten, and set to a random
        value.
    */
    void initialiseRandom()
    {
        for(int i = 0; i < NumberOfRegisters; ++i)
        {
            registers[i] = (RegisterType) rand();
            register_written[i] = false;
            register_read[i] = false;
        }
    }

    /*! Get the value of a register, specified by a slot.

        @param reg   A slot whose value is the register index
        @return      The value of the register.
    */
    RegisterType getRegister(Slot *reg)
    {
        register_read[reg->getValue()] = true;
        return registers[reg->getValue()];
    }

    /*! Set the value of a register, specified by a slot.

        @param reg      The register to set.
        @param value    The value to set the register to.
    */
    void setRegister(Slot *reg, RegisterType value)
    {
        register_written[reg->getValue()] = true;
        registers[reg->getValue()] = value;
    }

    /*! Get the value of a register, by register number.

        @param reg      The number of the register.
        @return         The value of the register.
    */
    RegisterType getRegisterValue(unsigned reg)
    {
        register_read[reg] = true;
        return registers[reg];
    }

    /*! Set the value of a register, by register number.

        @param reg      The number of the register.
        @param value    The value to set the register to.
    */
    void setRegisterValue(unsigned reg, RegisterType value)
    {
        register_written[reg] = true;
        registers[reg] = value;
    }

    /*! Perform a simple check to determine whether another TargetMachine is the
        same as this one. This is not commutative, since it only checks the
        registers marked as written in this state.

        For example:
        \code
            Current machine state:
                r0 = 10, r1 = 20
            Other machine state:
                r0 = 10, r1 = 20, r2 = 30
        \endcode
            The other state is equivalent, since only r0 and r1 are checked.

        \code
            Current machine state:
                r0 = 10, r1 = 20, r2 = 20
            Other machine state:
                r0 = 10, r1 = 20
        \endcode
            The other state is not equivalent, since r0, r1 and r2 are checked.

        @param other    The state to check equivalence to.
        @return         Whether or not the states are equivalent.
    */
    bool equivalentState(TargetMachine &other)
    {
        bool equiv = true;

        for(unsigned i = 0; i < NumberOfRegisters; ++i)
            if(register_written[i] && registers[i] != other.registers[i])
                equiv = false;

        return equiv;
    }

    /*! Check whether the other state is contained within the current state.

        This attempts to find out whether there is a permutation of written
        registers in the current state which corresponds to the other state.
        This means that as long as the other register values are amongst the
        written values in our current state, the result is true. The function
        computes a mapping between the current registers and the other
        registers.

        If a mapping is already specified, then it just checks the state is
        contained, as per that mapping. This allows the mapping to be computed
        at first, and then used in subsequent calls, preserving the register
        mapping.

        For example:
        \code
            Current machine state:
                r0 = 10, r1 = 20
            Other machine state:
                r4 = 20
        \endcode
            r4 can be mapped onto r1, therefore the current state contains
            state other.

        The function checks that the current state has atleast as many
        register written as the other state. If the current state has N
        registers written and the other state has M register written, the
        number of combinations is N choose M. Each permutation of these M
        registers is then compared to see if a mapping can be found.

        TODO: next_permutation takes ~50% of this function's runtime, perhaps
        the combinations should be cached and reused.

        @param other    The state to test whether it is a subset of the
                        current state.
        @param mapping  A mapping of this state's registers to the other
                        state's registers.
        @return         Whether the written registers in other can be mapped
                        onto the current state.
    */
    bool containsState(TargetMachine &other,
        std::vector<std::pair<unsigned,unsigned>> *mapping=nullptr)
    {
        RegisterType reg_map[NumberOfRegisters];
        RegisterType reg_map_other[NumberOfRegisters];
        unsigned n_reg=0, n_reg_other=0;
        bool equiv;

        // If a mapping is specified, don't try to find one, just compare
        if(mapping && mapping->size() != 0)
        {
            for(auto &map: *mapping)
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
        Combinations comb_iter(rm.size(), n_reg_other);
        std::vector<unsigned> reg_list;

        // Iterate over each combination of n_reg_other registers from our
        // register list.
        do
        {
            comb_iter.getSelection(reg_list);

            // Iterate over each permutation of the chosen combination.
            do
            {
                // Check if all the registers match. If they do, then we have
                // found a mapping. If mapping is not null, record this.
                equiv = true;
                for(unsigned i = 0; i < n_reg_other; ++i)
                {
                    if(registers[reg_map[reg_list[i]]] != other.registers[reg_map_other[i]])
                        equiv = false;
                }
                if(equiv)
                {
                    if(mapping)
                        for(unsigned i = 0; i < n_reg_other; ++i)
                            mapping->push_back(std::make_pair(reg_map[reg_list[i]], reg_map_other[i]));
                    break;
                }
            } while(std::next_permutation(reg_list.begin(), reg_list.end()));
            if(equiv)
                break;
        } while(comb_iter.next());

        return equiv;
    }

protected:
    RegisterType registers[NumberOfRegisters];
    bool register_written[NumberOfRegisters];
    bool register_read[NumberOfRegisters];
};



#endif
