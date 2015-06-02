#ifndef __BACKEND_HPP__
#define __BACKEND_HPP__

#include <vector>
#include "frontend.hpp"

class BasicBlock
{
public:
    // Start a new basic block
    virtual void start();

    // UOPS
    virtual void add_r8(RegisterSlot *output, RegisterSlot *input_1, RegisterSlot *input_2);

    // End the basic block
    virtual void end();
};

class InstructionTemplate
{
public:
    virtual ~InstructionTemplate();

    // Return a new basic block
    virtual BasicBlock *newBasicBlock();

    // Return a new slot for registers
    virtual RegisterSlot *newRegisterSlot(int type);
    // Other slot types

    // Set the entry basic block to the instruction
    virtual void setEntryPoint(BasicBlock *entry);

    // Compile the template to code in memory
    virtual void compile() {}

    virtual void execute(TargetMachine *);

protected:
    std::vector<BasicBlock*> basicblocks;
    std::vector<Slot*> slots;

    BasicBlock *entrypoint;
};

class Backend
{
public:
    // Backend();
    // virtual ~Backend();

    // InstructionTemplate startInstructionTemplate() = 0;


};

#endif
