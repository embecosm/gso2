#ifndef __GENERICTARGETMACHINE_HPP__
#define __GENERICTARGETMACHINE_HPP__

#include <vector>
#include "slots.hpp"
#include "utility.hpp"

/*! \class GenericTargetMachine

    A generic machine which supports an arbitrary number of registers, flags
    and memory.
*/
template <typename RegisterType, unsigned NumberOfRegisters, unsigned NumberOfFlags,
    typename MemoryDataType=uint8_t, typename MemoryAddressType=uint8_t,
    unsigned MemorySize=1024, bool unified=true>
class GenericTargetMachine :
public TargetMachineWithFlags<RegisterType, NumberOfRegisters, NumberOfFlags>
{
public:
    /*! Initialise the the class, setting flags to 0.
    */
    GenericTargetMachine() :
    TargetMachineWithFlags<RegisterType, NumberOfRegisters, NumberOfFlags>()
    {
        // TODO Implement (initialisation of memory)
        assert(!"TODO implement me.");
    }

    void initialiseRandom()
    {
        // TODO Implement (randomise memory)

        // Implement this by having a flag which says random is enabled, any
        // location which has not been written returns a random result.
        assert(!"TODO implement me.");
    }

    MemoryDataType getMemoryValue(MemoryAddressType address)
    {
        // TODO Implement (retreive memory)
        assert(!"TODO implement me.");
        return MemoryDataType();
    }

    void setMemoryValue(MemoryAddressType address, MemoryDataType value)
    {
        // TODO Implement (set memory)
        assert(!"TODO implement me.");
    }

    bool equivalentState(TargetMachine &other)
    {
        // TODO Implement (check memory is equal)
        assert(!"TODO implement me.");
    }

    bool containsState(TargetMachine &other,
        std::vector<std::pair<unsigned,unsigned>> *mapping=nullptr)
    {
        // TODO Implement
        // First call TargetMachine::containsState
        // Then perform a similar analysis with memory
        assert(!"TODO implement me.");
    }

protected:

    // TODO implement memory as a list of reads and writes, rather than an
    // actual array of items.
};


#endif
