#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

#include <vector>
#include <algorithm>

class Slot
{
public:
    virtual unsigned getValue()
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

protected:
    unsigned value;
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
};


class ConstantSlot : public Slot
{

};

#endif
