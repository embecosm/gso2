#ifndef TARGET_INFO_H
#define TARGET_INFO_H

#include <vector>
#include <iostream>

#include "Type.h"
#include "Register.h"
#include "Instruction.h"
#include "OperandClass.h"


namespace SuperOpt {


class TargetInfo {
public:
  RegisterID addReg(const Register& reg) {
    RegisterID reg_id = m_regs.size();
    m_regs.push_back(reg);
    return reg_id;
  }

  Opcode addInstr(const InstrDesc& instr) {
    Opcode opcode = m_instrs.size();
    m_instrs.push_back(instr);
    return opcode;
  }

  OperandClassID addOperandClass(const OperandClass& op_class)
  {
    OperandClassID class_id = m_op_classes.size();
    m_op_classes.push_back(op_class);
    return class_id;
  }


  const Register&
  getReg(RegisterID reg) const { return m_regs[reg]; }

  const InstrDesc&
  getInstr(Opcode opcode) const { return m_instrs[opcode]; }

  const OperandClass&
  getOperandClass(OperandClassID index) const { return m_op_classes[index]; }

  unsigned instrCount() const { return m_instrs.size(); }

private:
  std::vector<Register> m_regs;
  std::vector<InstrDesc> m_instrs;
  std::vector<OperandClass> m_op_classes;
};


} // end of namespace SuperOpt


#endif // TARGET_INFO_H

