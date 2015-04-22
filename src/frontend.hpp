#ifndef __FRONTEND_HPP__
#define __FRONTEND_HPP__

class Backend;

// A base class for each instruction we can emulate
class Instruction
{
public:
    // Instruction();
    // virtual ~Instruction();

    virtual Uops generateUops() = 0;
};

class TargetMachine
{
public:
    TargetMachine();
    virtual ~TargetMachine();


    virtual setBackend(Backend *backend);

// protected:
    Backend *backend;
};

#endif
