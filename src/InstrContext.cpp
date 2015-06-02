#include <vector>
#include <utility>
#include <climits>
#include <iostream>

#include "InstrContext.h"
#include "Type.h"
#include "TargetInfo.h"
#include "Operand.h"
#include "OperandClass.h"
#include "Instruction.h"


namespace SuperOpt {


// Retrieve the corresponding instruction for this context.
const Instruction& InstrContext::getInstr() const
{
  return m_instr;
}


// Get the registers which are live when entering this instruction
RegMask InstrContext::getLiveInRegMask() const
{
  return m_reg_mask;
}

// Calculate the registers which are live after this instruction has completed.
// This is based on the live input registers as well as those made live by
// this instruction.
RegMask InstrContext::getLiveOutRegMask() const
{
  RegMask reg_mask = m_reg_mask;

  const InstrDesc& desc = m_instr.getDesc();
  const unsigned n_outs = desc.outputCount();

  // iterate through the output operands, adding the output registers to
  // the register mask
  for (unsigned op_num = 0; op_num < n_outs; op_num++) {
    const RegisterID reg = m_instr.getOperand(op_num).getReg();
    reg_mask |= (1 << reg);
  }
  return reg_mask;
}


// Get the register mask for a given operand. An input operand can take any
// of the values of the live input register mask, whereas an output operand
// can make additional registers live
RegMask InstrContext::getOperandRegMask(const unsigned op_num) const
{
  const InstrDesc& desc = m_instr.getDesc();

  assert(op_num < desc.operandCount());

  if (op_num < desc.outputCount()) {
    return m_output_reg_masks[op_num];
  }
  else {
    return m_reg_mask;
  }
}


// Find the first unused register for a given register class.
// FIXME: This makes the assumption that the registers are in order in
// the register mask, but this is not yet enforced
RegisterID InstrContext::
nextUnusedRegister(RegMask reg_mask, RegMask class_mask)
{
  reg_mask = (reg_mask ^ class_mask) & class_mask;
  return __builtin_ctz(reg_mask);
}


// Start from the provided position in the register class and iterate until
// the next available register is found, or there are no more register to pick
// from. If there are no more registers to pick from, then wrap to the first
// available register and return true to indicate that a wrap occurred.
std::pair<bool, unsigned> InstrContext::
nextRegister(Operand& op,
             const RegisterClass& reg_class,
             unsigned reg_class_index,
             RegMask reg_mask)
{
  RegisterID reg;

  bool wrap = false;
  bool found_reg = false;

  while (!found_reg) {
    // get the next register in the register class and check whether it is
    // live in the provided register mask.
    reg_class_index++;
    if (reg_class_index >= reg_class.regCount()) {
      reg_class_index = 0;
      wrap = true;
    }
    reg = reg_class.getReg(reg_class_index);
    found_reg = reg_mask & (1 << reg);
  }

  op.setReg(reg);
  return std::make_pair(wrap, reg_class_index);
}


// Iterate to the next available immediate value for the given operand. If
// all available immediate values are exhausted then wrap to the first value
// and return true.
bool InstrContext::
nextImmediate(Operand& op, const ImmediateClass& imm_class)
{
  int64_t imm = op.getImm();

  bool wrap = false;
  imm += imm_class.getStep();
  if (imm > imm_class.getMax()) {
    imm = imm_class.getMin();
    wrap = true;
  }

  op.setImm(imm);
  return wrap;
}


// Iterate to the next available branch for the given operand.
// If there are no more available branch values then wrap to the first value
// and return true, otherwise return false.
bool InstrContext::
nextBranch(Operand& op,
           const BranchClass& branch_class,
           unsigned seq_index,
           unsigned seq_len)
{
  int64_t branch = op.getBranch();

  int64_t max_branch = branch_class.getMax();
  int64_t branch_rem = seq_len - seq_index;
  max_branch = std::min(branch_rem, max_branch);

  bool wrap = false;
  branch += branch_class.getStep();
  if (branch > max_branch) {
    branch = branch_class.getMin();
    assert(branch >= 0);
    wrap = true;
  }

  op.setBranch(branch);
  return wrap;
}


// Iterate to the next combination of operand values for the current 
// instruction. Start from the first operand and iterate, if there are no
// more available values for that operand then that operand will wrap around
// to its first value, and the next operand will iterate.
bool InstrContext::
nextOperands(unsigned seq_index, unsigned seq_len)
{
  const InstrDesc& instr_desc = m_instr.getDesc();

  bool wrap = true;
  unsigned op_index = 0;
  while (wrap && op_index < instr_desc.operandCount()) {
    Operand& op = m_instr.getOperand(op_index);
    const OperandClass& op_class = instr_desc.getOperandClass(op_index);

    switch (op_class.getType()) {
    case Operand::Type::REG: {
      const RegisterClass& reg_class = op_class.getRegClass();

      RegMask reg_mask = getOperandRegMask(op_index);
      unsigned reg_class_index = m_reg_class_indices[op_index];

      std::pair<bool, unsigned> res;
      res = nextRegister(op, reg_class, reg_class_index, reg_mask);

      wrap = res.first;
      m_reg_class_indices[op_index] = res.second;
      break;
    }
    case Operand::Type::IMM: {
      const ImmediateClass& imm_class = op_class.getImmClass();
      wrap = nextImmediate(op, imm_class);
      break;
    }
    case Operand::Type::BRANCH: {
      const BranchClass& branch_class = op_class.getBranchClass();
      wrap = nextBranch(op, branch_class, seq_index, seq_len);
      break;
    }
    }

    // If the current operand wrapped, carry into the next operand
    if (wrap) {
      op_index++;
    }
  }
  // If all of the operand wrapped, we have run out of combinations
  return wrap;
}


bool InstrContext::
setInstruction(const TargetInfo& target_info, Opcode opcode, RegMask reg_mask)
{
  // Perform sanity checks to ensure that we can pick this instruction
  const InstrDesc& instr_desc = target_info.getInstr(opcode);

  // Check that each input register class has at least one available register
  const unsigned n_outs = instr_desc.outputCount();
  const unsigned n_ops  = instr_desc.operandCount();

  for (unsigned op_index = n_outs; op_index < n_ops; op_index++) {
    assert(instr_desc.isInput(op_index));

    const OperandClass& op_class = instr_desc.getOperandClass(op_index);
    if (op_class.isReg()) {
      const RegisterClass& reg_class = op_class.getRegClass();
      const RegMask class_mask = reg_class.getMask();

      // if the mask is zero for an operand then it cannot be used as an input
      // so the instruction is not valid
      if (!(class_mask & reg_mask)) {
        return false;
      }
    }
  }

  m_instr.setDesc(target_info, opcode);
  m_reg_mask = reg_mask;

  // ensure that we have space for the output register masks and the
  // register class indices. These are subsequently initialized
  m_output_reg_masks.resize(n_outs);
  m_reg_class_indices.resize(n_ops);

  // Initialize the register masks for output operands
  // An output register can be any live input register
  // or it can be the same as another output register for the instruction
  // or it can be the next unused register in its register class
  RegMask out_mask = reg_mask;
  for (unsigned op_index = 0; op_index < n_outs; op_index++) {
    assert(instr_desc.isOutput(op_index));

    const RegisterClass& reg_class =
        instr_desc.getOperandClass(op_index).getRegClass();
    const RegMask class_mask = reg_class.getMask();

    // Add the first unused register in the register class to the mask for
    // this operand.
    RegisterID next_reg = nextUnusedRegister(out_mask, class_mask);
    if (next_reg < (sizeof(class_mask) * CHAR_BIT)) {
      out_mask |= (1 << next_reg);
    }
    m_output_reg_masks[op_index] = out_mask;
  }

  // Initialize each operand in turn
  // FIXME: This duplicate the work of nextRegister/nextImm/nextBranch, and
  // should probably be factored out
  for (unsigned op_index = 0; op_index < n_ops; op_index++) {
    Operand& op = m_instr.getOperand(op_index);

    const OperandClass& op_class = instr_desc.getOperandClass(op_index);
    switch (op_class.getType()) {
    case Operand::Type::REG: {
      const RegisterClass& reg_class = op_class.getRegClass();
      const RegMask reg_mask = getOperandRegMask(op_index);

      // find the first free register. We checked earlier that there should
      // always be at least one free.
      RegisterID reg;
      bool found_reg = false;
      unsigned reg_class_index = 0;
      while (!found_reg) {
        assert(reg_class_index < reg_class.regCount());

        reg = reg_class.getReg(reg_class_index);
        found_reg = reg_mask & (1 << reg);
        if (!found_reg) {
          reg_class_index++;
        }
      }

      // init operand, record the index of this reg in the class
      op.setReg(reg);
      m_reg_class_indices[op_index] = reg_class_index;
      break;
    }
    case Operand::Type::IMM: {
      const ImmediateClass& imm_class = op_class.getImmClass();

      int64_t imm = imm_class.getMin();
      op.setImm(imm);
      break;
    }
    case Operand::Type::BRANCH: {
      const BranchClass& branch_class = op_class.getBranchClass();

      // FIXME: validate branch distance is okay?
      int64_t branch = branch_class.getMin();
      op.setBranch(branch);
      break;
    }
    }
  }
  return true;
}


bool InstrContext::nextInstruction(const TargetInfo& target_info)
{
  bool success = false;
  while(!success) {
    Opcode opcode = m_instr.getOpcode();
    opcode++;

    if (opcode >= target_info.instrCount()) {
      // no more instructions to pick from
      return true;
    }
    success = setInstruction(target_info, opcode, m_reg_mask);
  }
  return false;
}


} // end of namespace SuperOpt

