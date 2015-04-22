#ifndef __GENERIC_BACKEND_HPP__
#define __GENERIC_BACKEND_HPP__

#include "../backend.hpp"

class GenericBasicBlock : public BasicBlock
{
public:
    // GenericUops();
    // ~GenericUops();

    virtual void add_r8(RegisterSlot *output, RegisterSlot *input_1, RegisterSlot *input_2);

    void end();
};

#endif /* __GENERIC_BACKEND_HPP__ */
