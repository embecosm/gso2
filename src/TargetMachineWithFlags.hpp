#ifndef __TARGETMACHINEWITHFLAGS_HPP__
#define __TARGETMACHINEWITHFLAGS_HPP__

#include <vector>
#include "slots.hpp"
#include "utility.hpp"

#include "TargetMachine.hpp"

/*! \class TargetMachineWithFlags

    This class extends the TargetMachine class to handle an arbitrary number
    of processor flags.
*/
template <typename RegisterType, unsigned NumberOfRegisters, unsigned NumberOfFlags>
class TargetMachineWithFlags : public TargetMachine<RegisterType, NumberOfRegisters>
{
public:
    /*! Initialise the the class, setting flags to 0.
    */
    TargetMachineWithFlags() : TargetMachine<RegisterType, NumberOfRegisters>()
    {
        for(unsigned i = 0; i < NumberOfFlags; ++i)
            flags[i] = false;

    }

    /*! Set the value of the flags randomly.

        This overrides the initialiseRandom function of TargetMachine, also
        providing randomised flags.

    */
    void initialiseRandom()
    {
        TargetMachine<RegisterType, NumberOfRegisters>::initialiseRandom();

        for(int i = 0; i < NumberOfFlags; ++i)
        {
            flags[i] = rand() & 1;
        }
    }

    /*! Get the value of a flag

        @param flg  The flag to return the value of.
        @return     The value of the flag.
    */
    bool getFlagValue(unsigned flg)
    {
        return flags[flg];
    }

    /*! Set the value of a flag

        @param flg      The flag to set the value of
        @param value    The value to set to.
    */
    void setFlagValue(unsigned flg, bool value)
    {
        flags[flg] = value;
    }

    bool equivalentState(TargetMachine &other)
    {
        assert(!"TODO implement me.");
    }

protected:
    bool flags[NumberOfFlags];
    // TODO possible extension: read and write status for flags
};


#endif
