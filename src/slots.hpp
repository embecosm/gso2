#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

#include <vector>
#include <algorithm>
#include <assert.h>

class Slot
{
public:
    virtual unsigned getValue() const
    {
        return value;
    }

    virtual void setValue(unsigned val)
    {
        value = val;
    }

    virtual void reset()
    {

    }

    friend std::ostream &operator<<(std::ostream &os, const Slot &d)
    {
        return os << d.toString();
    }

    friend std::ostream &operator<<(std::ostream &os, const Slot *d)
    {
        return os << d->toString();
    }

protected:
    unsigned value;

    virtual std::string toString() const
    {
        return std::to_string(getValue());
    }
};


class RegisterSlot : public Slot
{
public:
    RegisterSlot(bool _write=false, bool _read=true,
        std::vector<unsigned> _validArguments={}, unsigned value=0)
    {
        read = _read;
        write = _write;
        validArguments = _validArguments;
        setValue(value);
    }

    std::vector<unsigned> getValidArguments()
    {
        return validArguments;
    }

    void setValidArguments(std::vector<unsigned> validArguments_)
    {
        validArguments = validArguments_;
        std::sort(validArguments.begin(), validArguments.end());
    }

    virtual void reset()
    {
        if(validArguments.size() > 0)
            setValue(validArguments[0]);
    }

private:
    bool read, write;

    std::vector<unsigned> validArguments;

protected:
    virtual std::string toString() const
    {
        return "r" + std::to_string(getValue());
    }
};


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
