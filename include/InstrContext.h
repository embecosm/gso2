#ifndef INSTR_CONTEXT_H
#define INSTR_CONTEXT_H

#include <vector>
#include <utility>

#include "Type.h"
#include "Instruction.h"


namespace SuperOpt {


class TargetInfo;
class Operand;
class RegisterClass;
class ImmediateClass;
class BranchClass;


class InstrContext {
public:
  bool setInstruction(const TargetInfo& target_info,
                      Opcode opcode,
                      RegMask reg_mask);

  bool nextInstruction(const TargetInfo& target_info);

  bool nextOperands(unsigned seq_pos, unsigned seq_len);

  const Instruction& getInstr() const;

  RegMask getLiveInRegMask() const;
  RegMask getLiveOutRegMask() const;

private:
  RegMask getOperandRegMask(unsigned op_num) const;

  static RegisterID
  nextUnusedRegister(RegMask reg_mask,
                     RegMask class_mask);

  static std::pair<bool, unsigned>
  nextRegister(Operand& op,
               const RegisterClass& reg_class,
               unsigned reg_class_index,
               RegMask reg_mask);

  static bool nextImmediate(Operand& op, const ImmediateClass& imm_class);

  static bool
  nextBranch(Operand& op,
             const BranchClass& branch_class,
             unsigned seq_index,
             unsigned seq_len);

  // Current instruction
  Instruction m_instr;

  // Index of each register operand in their respective register classes
  std::vector<unsigned> m_reg_class_indices;

  // Mask of the live registers for this instruction
  RegMask m_reg_mask;

  // Mask of registers which may be chosen for each output operand
  std::vector<RegMask> m_output_reg_masks;
};


} // end of namespace SuperOpt


#endif // INSTR_CONTEXT_H

