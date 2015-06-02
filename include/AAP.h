#include "Type.h"
#include "TargetInfo.h"
#include "Instruction.h"
#include "OperandClass.h"


namespace SuperOpt {
namespace AAP {


// registers
RegisterID R0;
RegisterID R1;
RegisterID R2;
RegisterID R3;

// register classes
OperandClassID GPR_A;
OperandClassID GPR_B;

// instructions
Opcode ADD_r;
Opcode SUB_r;
//Opcode XOR_r;


void init(TargetInfo& ti)
{
  // registers
  R0 = ti.addReg(Register(16));
  R1 = ti.addReg(Register(16));
  R2 = ti.addReg(Register(16));
  R3 = ti.addReg(Register(16));

  // register classes
  RegisterClass GPR_A_def;
  GPR_A_def.addReg(R0)
           .addReg(R1)
           .addReg(R3);
  GPR_A = ti.addOperandClass(GPR_A_def);

  RegisterClass GPR_B_def;
  GPR_B_def.addReg(R0)
           .addReg(R2)
           .addReg(R3);
  GPR_B = ti.addOperandClass(GPR_B_def);

  // instruction
  InstrDesc ADD_r_desc;
  ADD_r_desc.addOperand(ti.getOperandClass(GPR_A), true)
            .addOperand(ti.getOperandClass(GPR_A), false)
            .addOperand(ti.getOperandClass(GPR_B), false);
  ADD_r = ti.addInstr(ADD_r_desc);

  InstrDesc SUB_r_desc;
  SUB_r_desc.addOperand(ti.getOperandClass(GPR_B), true)
            .addOperand(ti.getOperandClass(GPR_A), false)
            .addOperand(ti.getOperandClass(GPR_A), false);
  SUB_r = ti.addInstr(SUB_r_desc);
/*
  InstrDesc XOR_r_desc;
  XOR_r_desc.addOperand(ti.getOperandClass(GPR_B), true)
            .addOperand(ti.getOperandClass(GPR_A), false)
            .addOperand(ti.getOperandClass(GPR_B), false);
  XOR_r = ti.addInstr(XOR_r_desc);
*/
}


} // end of namespace AAP
} // end of namespace SuperOpt

