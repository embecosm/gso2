#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

#include <vector>
#include <algorithm>
#include <assert.h>

/*! \class Slot

    The slot class is the basis of all the parameters in the instruction
    which can be iterated over by a superoptimizer. For example, the class
    is subclassed to represent registers and constants.
*/

class Slot
{
public:
    /*! Get the value stored by the slot.

        @return The value stored by the slot.
    */
    virtual unsigned getValue()
    {
        return value;
    }

    /*! Set the value stored in the slot.

        @param val  Set the value stored to val.
    */
    virtual void setValue(unsigned val)
    {
        value = val;
    }

    /*! Reset the value in the slot. This is subclassed, to reset the value,
        since it doesn't necessarily make sense to reset the value sometimes.
    */
    virtual void reset()
    {

    }

    /*! Stream operator to display the slot's value on the specified stream.

        @param os  The stream to output to.
        @param d   The slot whose value to output.
    */
    friend std::ostream &operator<<(std::ostream &os, Slot &d)
    {
        return os << d.toString();
    }

    /*! Stream operator to display the slot's value on the specified stream.
        This is mostly a convenience function, since Slots are often refered
        to via pointers.

        @param os  The stream to output to.
        @param d   The slot whose value to output.
    */
    friend std::ostream &operator<<(std::ostream &os, Slot *d)
    {
        return os << d->toString();
    }

protected:
    unsigned value;

    /*! A conversion method, since the friend stream operators cannot be
        virtual. The method converts the slot's value to a string which
        can be displayed.

        @return The printable string of the slot's value.
    */
    virtual std::string toString()
    {
        return std::to_string(getValue());
    }
};

/*! \class RegisterSlot

    The RegisterSlot class contains information about a slot which can accept
    different registers. Information, such as whether the register is read,
    written, and its valid arguments are stored.
*/

class RegisterSlot : public Slot
{
public:
    /*! Initialise the RegisterSlot, with information about the class of
        values that can be stored, and whether the slot is a writable
        register, readable register or both. The _validArguments parameter
        should be sorted for the canonical form iterators to work properly.

        @param _write   Specify whether the register slot represents a
                        register which is written to by an instruction.
        @param _read    Specify whether the register slot represents
                        a register which is read by an instruction.
        @param _validArguments
                        A list of the possible values the register slot
                        will accept.
        @param value    An value to initialise the slot to.
    */
    RegisterSlot(bool _write=false, bool _read=true,
        std::vector<unsigned> _validArguments={}, unsigned value=0, int _classid=-1)
    {
        read = _read;
        write = _write;
        validArguments = _validArguments;
        classid = _classid;
        std::sort(validArguments.begin(), validArguments.end());
        setValue(value);
    }

    /*! Get the list of registers that the slot will accept.

        @return The list of accepted register names.
    */
    std::vector<unsigned> getValidArguments()
    {
        return validArguments;
    }

    /*! Set the list of acceptable registers for this slot.

        @param validArguments_  The values which this RegisterSlot can take.
    */
    void setValidArguments(std::vector<unsigned> validArguments_)
    {
        validArguments = validArguments_;
        std::sort(validArguments.begin(), validArguments.end());
    }

    /*! Reset the value of the Slot to the first in the set of valid
        arguments, if one has been set. Otherwise, do not reset the value.
    */
    virtual void reset()
    {
        if(validArguments.size() > 0)
            setValue(validArguments[0]);
    }

    /*! Get the ID of the register class associated with this register slot

        @return The class ID of the register slot.
    */
    int getRegisterClassID()
    {
        return classid;
    }

    /*! Set the ID of the register class associated with this register slot

        @param classid  The class ID of the register slot.
    */
    void setRegisterClassID(int classid)
    {
        this->classid = classid;
    }

private:
    bool read, write;
    int classid;
    std::vector<unsigned> validArguments;

protected:
    virtual std::string toString()
    {
        return "r" + std::to_string(getValue());
    }
};

/*! \class ConstantSlot

    The constant slot holds a constant value that may be given to an instruction. The
    constant slot accepts a number of ranges, each which describe which constants may
    be set for the instruction. While most instructions just accept a single range of
    constants, this allows the number of constants considered to be restricted, since
    iterating through every possible constant vastly increases the search space.
*/

class ConstantSlot : public Slot
{
public:
    // TODO: validate that the ranges are strictly ordered, i.e., they are in
    //  order and non-overlapping
    ConstantSlot(std::vector<std::pair<unsigned, unsigned>> ranges_)
    {
        ranges = ranges_;
        current_range = 0;
    }

    virtual void reset()
    {
        if(ranges.size() > 0)
        {
            value = ranges[0].first;
            current_range = 0;
        }
    }

    bool next()
    {
        assert(ranges.size() > 0);
        if(value >= ranges[current_range].second)
        {
            current_range++;
            if(current_range >= ranges.size())
            {
                current_range = 0;
                value = ranges[current_range].first;
                return false;
            }
            else
            {
                value = ranges[current_range].first;
                return true;
            }
        }
        else
        {
            value++;
            return true;
        }
    }

private:
    // Ranges are inclusive
    std::vector<std::pair<unsigned, unsigned>> ranges;
    unsigned current_range;
};

#endif
