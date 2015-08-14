#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

#include <iostream>
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
    virtual ~Slot() {}

    /*! Get the value stored by the slot.

        @return The value stored by the slot.
    */
    virtual uint64_t getValue() const
    {
        return value;
    }

    /*! Set the value stored in the slot.

        @param val  Set the value stored to val.
    */
    virtual void setValue(uint64_t val)
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
        @return    The stream.
    */
    friend std::ostream &operator<<(std::ostream &os, const Slot &d)
    {
        return os << d.toString();
    }

    /*! Stream operator to display the slot's value on the specified stream.
        This is mostly a convenience function, since Slots are often refered
        to via pointers.

        @param os  The stream to output to.
        @param d   The slot whose value to output.
        @return    The stream.
    */
    friend std::ostream &operator<<(std::ostream &os, const Slot *d)
    {
        return os << d->toString();
    }

    /*! Stream operator to input a value into the slot.

        This is a forwarding function, which calls the virtual function input.
        This ensures that the proper parsing of the slot can be performed
        (e.g. recognising registers or constants).

        @param in   The input stream.
        @param d    The slot to input into.
        @return     The stream.
    */
    friend std::istream &operator>>(std::istream &in, Slot &d)
    {
        return d.input(in);
    }

protected:
    uint64_t value;

    /*! A conversion method, since the friend stream operators cannot be
        virtual. The method converts the slot's value to a string which
        can be displayed.

        @return The printable string of the slot's value.
    */
    virtual std::string toString() const
    {
        return std::to_string(getValue());
    }

    /*! Input method.

        This inputs a value from the input stream into the slot's value. This
        function will be overridden if a different input format is required by
        subclasses.

        @param in   The input stream.
        @return     The input stream.
    */
    virtual std::istream &input(std::istream &in)
    {
        unsigned v;

        in >> v;

        if(!in.fail())
            setValue(v);

        return in;
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
    virtual void reset() override
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

    /*! Check whether the slot writes to its register

        @return  True if the register refered to by the slot gets written.
    */
    bool isWriteSlot()
    {
        return write;
    }

    /*! Check whether the slot reads its register

        @return  True if the register refered to by the slot is read.
    */
    bool isReadSlot()
    {
        return read;
    }

private:
    bool read, write;
    int classid;
    std::vector<unsigned> validArguments;

protected:
    /*! Format the register for output.

        By default the format just prepends 'r' to the number stored in the
        slot.

        @return  A string representing the register.
    */
    virtual std::string toString() const override
    {
        return "r" + std::to_string(getValue());
    }

    /*! Parse a register and store its value.

        The format expected is rX, where X is the register number. If this
        could not be matched, the fail bit of the stream is set.

        @param in   The input stream.
        @return     The input streamm.
    */
    virtual std::istream &input(std::istream &in) override
    {
        unsigned v;
        char r;

        in >> r >> v;

        if(!in.fail() && r != 'r')
            in.setstate(std::ios::failbit);

        if(!in.fail())
            setValue(v);

        return in;
    }
};

/*! \class ConstantSlot

    The constant slot holds a constant value that may be given to an
    instruction. The constant slot accepts a number of ranges, each which
    describe which constants may be set for the instruction. While most
    instructions just accept a single range of constants, this allows the
    number of constants considered to be restricted arbitrarily, since
    iterating through every possible constant vastly increases the search
    space.

    The constant slot has a 'lossy' mode, where only certain constants will be
    iterated. This is enabled by the iteratorSkip function. The constant
    enabled by setting the lossy setting include all in the range 0 to 16, 2^n
    for all n, and 2^n - 1 for all n.

    TODO: Should the logic for lossy mode be moved into constantIterator
    instead?
*/

class ConstantSlot : public Slot
{
public:

    /*! Initialise the constant slot.

        The ranges of the constant slot are initialise here. Each range is
        inclusive, with the first number being the lowest and the second
        number being the highest (and last) number to assign to the constant
        slot.

        @param ranges_  A list of ranges for the constant slot.
    */
    ConstantSlot(std::vector<std::pair<uint64_t, uint64_t>> ranges_)
    {
        // TODO: validate that the ranges are strictly ordered, i.e., they are
        // in order and non-overlapping.
        ranges = ranges_;
        current_range = 0;
        lossy = false;
    }

    /*! Reset the value of the slot.

        The value is the lower (i.e. first value in the first range), or the
        lowest value in this list of lossy values.
    */
    virtual void reset() override
    {
        if(lossy)
        {
            current_range = 0;
            value = lossy_values[0];
        }
        else
        {
            if(ranges.size() > 0)
            {
                value = ranges[0].first;
                current_range = 0;
            }
        }
    }

    /*! Set the slot value to the next constant value.

        If the last value has been reached, and the value wraps around, false
        is returned, otherwise true.

        If lossy mode is enabled, this function just increments current_range
        and uses this to index into the list of lossy values. Otherwise, it is
        determined whether the current value is with in a particular range. If
        the end of that range has been reached the next range is selected.

        @return   True if there are still more value to iterate over.
    */
    bool next()
    {
        assert(ranges.size() > 0);

        if(lossy)
        {
            // Iterate over the lossy values instead.
            current_range++;
            if(current_range >= lossy_values.size())
            {
                current_range = 0;
                value = lossy_values[0];
                return false;
            }
            else
            {
                value = lossy_values[current_range];
                return true;
            }
        }
        else
        {
            // Find out whether we are at the end of our current range or not.
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
    }

    /*! Enable skipping of infrequently used constants.

        Some constants are used more frequently than others in programs. The
        constant enabled by setting the lossy setting include all in the range
        0 to 16, 2^n for all n, and 2^n - 1 for all n. This function resets
        the current range.

        The lossy values are implemented by storing all the possible values in
        lossy_values, which is then iterated by next.

        @param lossy_  Whether to enable to lossy iteration of constants or
                       not.
    */
    void iteratorSkip(bool lossy_)
    {
        lossy = lossy_;

        // Reset the counter to 0, since we may change mode of operation.
        current_range = 0;

        // Precompute the lossy values and store them.
        lossy_values.clear();

        if(!lossy)
            return;

        for(auto &p: ranges)
        {
            // Add all values under 16 to lossy_values, if they are in the
            // range.
            if(p.first < 16)
            {
                for(uint64_t i = p.first; i < 16 && i < p.second; ++i)
                    lossy_values.push_back(i);
            }

            // Starting at 16, add all powers of two, and all powers of two
            // minus one to the list, as long as they appear in the current
            // range.
            uint64_t v = 16;
            for(unsigned i = 4; i < sizeof(uint64_t)*8; ++i, v <<= 1)
            {
                // 2^n
                if(v >= p.first && v <= p.second)
                {
                    lossy_values.push_back(v);
                }
                // 2^n - 1
                if(v-1 >= p.first && v-1 <= p.second)
                {
                    lossy_values.push_back(v-1);
                }
            }
        }
    }

private:
    // Ranges are inclusive
    std::vector<std::pair<uint64_t, uint64_t>> ranges;

    bool lossy;
    std::vector<uint64_t> lossy_values;

    // Current range is used to describe which range is begin picked from if
    // we are not iterating lossily, otherwise it is the index of the current
    // value in the lossy_values list.
    unsigned current_range;

};

#endif
