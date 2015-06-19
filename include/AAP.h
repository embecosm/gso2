#include "Type.h"
#include "TargetInfo.h"
#include "Instruction.h"
#include "OperandClass.h"
#include "MachineState.h"


namespace SuperOpt {
namespace AAP {


// registers
RegisterID R0, R1, R2, R3, R4, R5, R6, R7;

// register classes
OperandClassID GPR;

// Immediate classes
OperandClassID Imm3, Imm6, Imm9, Imm10, Imm12, Imm16;

// Branch classes
OperandClassID Off3, Off6, Off9, Off10, Off16, Off18;

// instructions
Opcode ADD_r,   SUB_r,   AND_r,  OR_r,  XOR_r,  ASR_r,  LSL_r,  LSR_r,  MOV_r,
       ADD_i10, SUB_i10, AND_i9, OR_i9, XOR_i9, ASR_i6, LSL_i6, LSR_i6, MOV_i16,
       ADDC_r,  SUBC_r,
       BRA, BEQ, BNE, BLTS, BGTS, BLTU, BGTU;


// AAP Machine State has 8 registers, and also adds a carry flag which can
// be queried
class AAPMachineState : public MachineState<8> {
public:
  AAPMachineState() : MachineState(), m_carry(false)
  { }

  void setCarry(bool carry) { m_carry = carry; }
  bool getCarry() const { return m_carry; }

private:
  bool m_carry;
};


// ALU register operations
void execute_ALU_r(const Instruction& instr,
                   IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  RegisterID reg_out = instr.getOperand(0).getReg();
  RegisterID reg_a = instr.getOperand(1).getReg();
  RegisterID reg_b = instr.getOperand(2).getReg();

  uint16_t a = state->getReg(reg_a);
  uint16_t b = state->getReg(reg_b);
  uint16_t out;

  const Opcode op = instr.getOpcode();
  if (op == ADD_r) {
    state->setCarry(((uint32_t)a + (uint32_t)b) > 0xffff);
    out = a + b;
  }
  else if (op == SUB_r) {
    state->setCarry(b > a);
    out = a - b;
  }
  else if (op == AND_r) { out = a & b; }
  else if (op == OR_r)  { out = a | b; }
  else if (op == XOR_r) { out = a ^ b; }
  else if (op == ASR_r) { out = (int16_t)a >> b; }
  else if (op == LSL_r) { out = a << b; }
  else if (op == LSR_r) { out = a >> b; }
  else if (op == ADDC_r) {
    uint16_t c = state->getCarry();
    state->setCarry(((uint32_t)a + (uint32_t)b + (uint32_t)c) > 0xffff);
    out = a + b + c;
  }
  else if (op == SUBC_r) {
    uint16_t c = state->getCarry();
    state->setCarry((b + c) > a);
    out = a - b - c;
  }
  else {
    assert(0 && "Not a 3-register ALU operation");
  }

  state->setReg(reg_out, out);
}


// ALU immediate operations
void execute_ALU_i(const Instruction& instr,
                   IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  RegisterID reg_out = instr.getOperand(0).getReg();
  RegisterID reg_a = instr.getOperand(1).getReg();

  uint16_t a = state->getReg(reg_a);
  uint16_t b = instr.getOperand(2).getImm();
  uint16_t out;

  const Opcode op = instr.getOpcode();
  if (op == ADD_i10) {
    state->setCarry(((uint32_t)a + (uint32_t)b) > 0xffff);
    out = a + b;
  }
  else if (op == SUB_i10) {
    state->setCarry(b > a);
    out = a - b;
  }
  else if (op == ASR_i6) { out = (int16_t)a >> b; }
  else if (op == LSL_i6) { out = a << b; }
  else if (op == LSR_i6) { out = a >> b; }
  else if (op == AND_i9) { out = a & b; }
  else if (op == OR_i9)  { out = a | b; }
  else if (op == XOR_i9) { out = a ^ b; }
  else {
    assert(0 && "Not a register-immediate ALU operation");
  }

  state->setReg(reg_out, out);
}


// Move operations
void execute_MOV_r(const Instruction& instr,
                   IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  RegisterID reg_out = instr.getOperand(0).getReg();
  RegisterID reg_in  = instr.getOperand(1).getReg();

  state->setReg(reg_out, state->getReg(reg_in));
}

void execute_MOV_i(const Instruction& instr,
                   IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  RegisterID reg_out = instr.getOperand(0).getReg();
  int64_t value = instr.getOperand(1).getImm();

  state->setReg(reg_out, value);
}


// Branches
void execute_BRA(const Instruction& instr,
                 IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  uint16_t pc = state->getPC();
  int16_t offset = instr.getOperand(0).getBranch();
  state->setPC(pc + offset);
}

void execute_BR_CC(const Instruction& instr,
                   IMachineState * const machine_state)
{
  AAPMachineState * const state = static_cast<AAPMachineState*>(machine_state);

  uint16_t pc = state->getPC();
  int16_t offset = instr.getOperand(0).getImm();

  uint16_t target_pc = pc + offset;

  RegisterID reg_a = instr.getOperand(1).getReg();
  RegisterID reg_b = instr.getOperand(2).getReg();

  uint16_t a = state->getReg(reg_a);
  uint16_t b = state->getReg(reg_b);

  const Opcode op = instr.getOpcode();

  if (op == BEQ) {
    if (a == b) { state->setPC(target_pc); }
  }
  else if (op == BNE) {
    if (a != b) { state->setPC(target_pc); }
  }
  else if (op == BLTS) {
    if ((int16_t)a < (int16_t)b) { state->setPC(target_pc); }
  }
  else if (op == BGTS) {
    if ((int16_t)a > (int16_t)b) { state->setPC(target_pc); }
  }
  else if (op == BLTU) {
    if (a < b) { state->setPC(target_pc); }
  }
  else if (op == BGTU) {
    if (a > b) { state->setPC(target_pc); }
  }
  else {
    assert(0 && "Not a conditional branch operation!");
  }
}


void init(TargetInfo& ti)
{
  // registers
  R0 = ti.addReg(Register(16));
  R1 = ti.addReg(Register(16));
  R2 = ti.addReg(Register(16));
  R3 = ti.addReg(Register(16));
  R4 = ti.addReg(Register(16));
  R5 = ti.addReg(Register(16));
  R6 = ti.addReg(Register(16));
  R7 = ti.addReg(Register(16));

  // register classes
  RegisterClass GPR_def;
  GPR_def.addReg(R0).addReg(R1).addReg(R2).addReg(R3)
         .addReg(R4).addReg(R5).addReg(R6).addReg(R7);
  GPR = ti.addOperandClass(GPR_def);

  // immediate classes
  ImmediateClass  Imm3_def(0,     0);
  ImmediateClass  Imm6_def(0,    63);
  ImmediateClass  Imm9_def(0,   511);
  ImmediateClass Imm10_def(0,  1023);
  ImmediateClass Imm12_def(0,  4095);
  ImmediateClass Imm16_def(0, 65535);
  Imm3  = ti.addOperandClass(Imm3_def);
  Imm6  = ti.addOperandClass(Imm6_def);
  Imm9  = ti.addOperandClass(Imm9_def);
  Imm10 = ti.addOperandClass(Imm10_def);
  Imm12 = ti.addOperandClass(Imm12_def);
  Imm16 = ti.addOperandClass(Imm16_def);

  // Only use immediate 3 for now
  Imm6  = Imm3;
  Imm9  = Imm3;
  Imm10 = Imm3;
  Imm12 = Imm3;
  Imm16 = Imm3;

  // branch offsets
  BranchClass  Off3_def(     -4,      3);
  BranchClass  Off6_def(    -32,     31);
  BranchClass  Off9_def(   -256,    255);
  BranchClass Off10_def(   -512,    511);
  BranchClass Off16_def( -32768,  32767);
  BranchClass Off18_def(-131072, 131071);
  Off3  = ti.addOperandClass(Off3_def);
  Off6  = ti.addOperandClass(Off6_def);
  Off9  = ti.addOperandClass(Off9_def);
  Off10 = ti.addOperandClass(Off10_def);
  Off16 = ti.addOperandClass(Off16_def);
  Off18 = ti.addOperandClass(Off18_def);


  // ALU instructions
  InstrDesc ALU_r_desc;
  ALU_r_desc.addOperand(ti.getOperandClass(GPR), true)
            .addOperand(ti.getOperandClass(GPR), false)
            .addOperand(ti.getOperandClass(GPR), false);
  ALU_r_desc.setExecuteFn(&execute_ALU_r);

  ADD_r = ti.addInstr(ALU_r_desc);
  SUB_r = ti.addInstr(ALU_r_desc);
  AND_r = ti.addInstr(ALU_r_desc);
  OR_r  = ti.addInstr(ALU_r_desc);
  XOR_r = ti.addInstr(ALU_r_desc);
  ASR_r = ti.addInstr(ALU_r_desc);
  LSL_r = ti.addInstr(ALU_r_desc);
  LSR_r = ti.addInstr(ALU_r_desc);
  ADDC_r = ti.addInstr(ALU_r_desc);
  SUBC_r = ti.addInstr(ALU_r_desc);

/*
  // Imm10 Add/Sub
  InstrDesc ALU_i10_desc;
  ALU_i10_desc.addOperand(ti.getOperandClass(GPR), true)
              .addOperand(ti.getOperandClass(GPR), false)
              .addOperand(ti.getOperandClass(Imm10), false);
  ALU_i10_desc.setExecuteFn(&execute_ALU_i);

  ADD_i10 = ti.addInstr(ALU_i10_desc);
  SUB_i10 = ti.addInstr(ALU_i10_desc);

  // Imm6 Shifts
  InstrDesc ALU_i6_desc;
  ALU_i6_desc.addOperand(ti.getOperandClass(GPR), true)
             .addOperand(ti.getOperandClass(GPR), false)
             .addOperand(ti.getOperandClass(Imm6), false);
  ALU_i6_desc.setExecuteFn(&execute_ALU_i);

  ASR_i6 = ti.addInstr(ALU_i6_desc);
  LSL_i6 = ti.addInstr(ALU_i6_desc);
  LSR_i6 = ti.addInstr(ALU_i6_desc);

  // Imm9 Logical ops
  InstrDesc ALU_i9_desc;
  ALU_i9_desc.addOperand(ti.getOperandClass(GPR), true)
             .addOperand(ti.getOperandClass(GPR), false)
             .addOperand(ti.getOperandClass(Imm9), false);
  ALU_i9_desc.setExecuteFn(&execute_ALU_i);

  AND_i9 = ti.addInstr(ALU_i9_desc);
  OR_i9  = ti.addInstr(ALU_i9_desc);
  XOR_i9 = ti.addInstr(ALU_i9_desc);
*/

  // Move instructions
  InstrDesc MOV_r_desc;
  MOV_r_desc.addOperand(ti.getOperandClass(GPR), true)
            .addOperand(ti.getOperandClass(GPR), false);
  MOV_r_desc.setExecuteFn(&execute_MOV_r);
  MOV_r = ti.addInstr(MOV_r_desc);

/*
  InstrDesc MOV_i16_desc;
  MOV_i16_desc.addOperand(ti.getOperandClass(GPR), true)
              .addOperand(ti.getOperandClass(Imm16), false);
  MOV_i16_desc.setExecuteFn(&execute_MOV_i);
  MOV_i16 = ti.addInstr(MOV_i16_desc);
*/

  // Branch instructions
  InstrDesc BRA_desc;
  BRA_desc.addOperand(ti.getOperandClass(Off18), false);
  BRA_desc.setExecuteFn(&execute_BRA);
  BRA = ti.addInstr(BRA_desc);

  InstrDesc BR_CC_desc;
  BR_CC_desc.addOperand(ti.getOperandClass(Off16), false)
            .addOperand(ti.getOperandClass(GPR), false)
            .addOperand(ti.getOperandClass(GPR), false);
  BR_CC_desc.setExecuteFn(&execute_BR_CC);

  BEQ  = ti.addInstr(BR_CC_desc);
  BNE  = ti.addInstr(BR_CC_desc);
  BLTS = ti.addInstr(BR_CC_desc);
  BGTS = ti.addInstr(BR_CC_desc);
  BLTU = ti.addInstr(BR_CC_desc);
  BGTU = ti.addInstr(BR_CC_desc);
}


} // end of namespace AAP
} // end of namespace SuperOpt

