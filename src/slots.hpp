#ifndef __SLOTS_HPP__
#define __SLOTS_HPP__

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
    RegisterSlot(int _type, bool _write=false, bool _read=true)
    {
        type = _type;
        read = _read;
        write = _write;
    }

private:
    int type;
    bool read, write;
};


class ConstantSlot : public Slot
{

};

#endif
