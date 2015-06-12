#ifndef __FRONTEND_HPP__
#define __FRONTEND_HPP__

#include <vector>

class Slot;
class TargetMachine;
class Backend;

// A base class for each instruction we can emulate
class Instruction
{
public:
    // Instruction();
    // virtual ~Instruction();

    virtual unsigned execute(TargetMachine *_mach, Slot **slots) = 0;
    virtual std::vector<Slot*> getSlots() {return {};};


    virtual std::string toString() { return ""; };
    virtual std::string toString(Slot **slots) { return "";};
    virtual std::string getName() { return "";};

    // virtual Uops generateUops() = 0;
};

class TargetMachine
{
public:
    // TargetMachine();
    // virtual ~TargetMachine();


    // virtual void setBackend(Backend *backend);

// protected:
    Backend *backend;
};

#endif
