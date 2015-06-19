#ifndef OPERAND_CLASS_H
#define OPERAND_CLASS_H

#include <vector>
#include <cassert>
#include <cstdint>
#include <iostream>

#include "Type.h"
#include "Operand.h"


namespace SuperOpt {


class RegisterClass {
public:
  RegisterID getReg(unsigned index) const {
    return m_regs[index];
  }

  RegisterClass& addReg(RegisterID reg_id) {
    m_reg_mask |= 1ull << (uint64_t)reg_id;

    // FIXME: This list needs to be sorted
    m_regs.push_back(reg_id);
    return *this;
  }

  RegMask getMask() const { return m_reg_mask; }
  unsigned regCount() const { return m_regs.size(); }

private:
  RegMask m_reg_mask;
  std::vector<RegisterID> m_regs;
};


class ImmediateClass {
public:
  ImmediateClass() = delete;
  ImmediateClass(int64_t min, int64_t max, int64_t step = 1)
    : m_min(min), m_max(max), m_step(step)
  {
    assert(min <= max);
  }

  int64_t getMin() const { return m_min; }
  int64_t getMax() const { return m_max; }
  unsigned getStep() const { return m_step; }

private:
  int64_t m_min;
  int64_t m_max;
  unsigned m_step;
};


// use uint64_t or subclass ImmediateClass?
class BranchClass : public ImmediateClass {
public:
  BranchClass(int64_t min, int64_t max, int64_t step = 1)
    : ImmediateClass(min, max, step)
  {}
};


class OperandClass {
public:
  OperandClass() = delete;

  ~OperandClass()
  {
    switch (m_type) {
    case Operand::Type::REG:
      m_reg_class.~RegisterClass();
      break;
    case Operand::Type::IMM:
      m_imm_class.~ImmediateClass();
      break;
    case Operand::Type::BRANCH:
      m_branch_class.~BranchClass();
      break;
    default:
      assert(0 && "Unknown operand type!");
    }
  }

  OperandClass(const OperandClass& other)
    : m_type(other.m_type)
  {
    switch (other.m_type) {
    case Operand::Type::REG:
      m_reg_class = other.m_reg_class;
      break;
    case Operand::Type::IMM:
      m_imm_class = other.m_imm_class;
      break;
    case Operand::Type::BRANCH:
      m_branch_class = other.m_branch_class;
      break;
    default:
      assert(0 && "Unknown operand type!");
    }
  }

  OperandClass(RegisterClass& reg_class)
    : m_type(Operand::Type::REG),
      m_reg_class(reg_class)
  {}

  OperandClass(ImmediateClass& imm_class)
    : m_type(Operand::Type::IMM),
      m_imm_class(imm_class)
  {}

  OperandClass(BranchClass& branch_class)
    : m_type(Operand::Type::BRANCH),
      m_branch_class(branch_class)
  {}

  bool isReg() const { return m_type == Operand::Type::REG; }
  bool isImm() const { return m_type == Operand::Type::IMM; }
  bool isBranch() const { return m_type == Operand::Type::BRANCH; }

  Operand::Type getType() const { return m_type; }

  const RegisterClass& getRegClass() const
  {
    assert(isReg() && "Operand class not a register class!");
    return m_reg_class;
  }

  const ImmediateClass& getImmClass() const
  {
    assert(isImm() && "Operand class not an immediate class!");
    return m_imm_class;
  }

  const BranchClass& getBranchClass() const {
    assert(isBranch() && "Operand class not a branch class!");
    return m_branch_class;
  }

  // Dump the operand definition to an output stream
  void dump(std::ostream& out) const {
    switch (m_type) {
    case Operand::Type::REG:
      out << "REG";
      break;
    case Operand::Type::IMM:
      out << "IMM";
      break;
    case Operand::Type::BRANCH:
      out << "BRANCH";
      break;
    default:
      assert(0 && "Not a valid operand type!");
    }
  }

private:
  Operand::Type m_type;
  union {
    RegisterClass  m_reg_class;
    ImmediateClass m_imm_class;
    BranchClass    m_branch_class;
  };
};


} // end of namespace SuperOpt


#endif // OPERAND_CLASS_H

