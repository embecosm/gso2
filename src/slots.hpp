#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

#include <unordered_set>

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

protected:
    unsigned value;
};


class RegisterSlot : public Slot
{
public:
    RegisterSlot(bool _write=false, bool _read=true)
    {
        read = _read;
        write = _write;
    }

    std::unordered_set<unsigned> getValidArguments()
    {
        return validArguments;
    }

    void setValidArguments(std::unordered_set<unsigned> validArguments_)
    {
        validArguments = validArguments_;
    }

private:
    bool read, write;

    std::unordered_set<unsigned> validArguments;
};


class ConstantSlot : public Slot
{

};

#endif
