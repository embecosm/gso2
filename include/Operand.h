#ifndef OPERAND_H
#define OPERAND_H

#include <vector>
#include <cassert>
#include <cstdint>

#include "Type.h"


namespace SuperOpt {


class Operand {
public:
  enum class Type { REG, IMM, BRANCH };

  void setReg(RegisterID reg)
  {
    m_type = Type::REG;
    m_reg = reg;
  }

  void setImm(const int64_t imm)
  {
    m_type = Type::IMM;
    m_imm = imm;
  }

  void setBranch(const int64_t branch)
  {
    m_type = Type::BRANCH;
    m_branch = branch;
  }

  RegisterID getReg() const {
    assert(m_type == Type::REG && "Operand not a register!");
    return m_reg;
  }

  int64_t getImm() const {
    assert(m_type == Type::IMM && "Operand not an immediate!");
    return m_imm;
  }

  int64_t getBranch() const {
    assert(m_type == Type::BRANCH && "Operand not a branch!");
    return m_branch;
  }


  void dump(std::ostream& out) const
  {
    switch (m_type) {
    case Type::REG:
      out << "REG: " << m_reg;
      break;
    case Type::IMM:
      out << "IMM: " << m_imm;
      break;
    case Type::BRANCH:
      out << "BRANCH: " << m_branch;
      break;
    }
  }

private:
  Type m_type;
  union {
    RegisterID m_reg;
    int64_t m_imm;
    int64_t m_branch;
  };
};


} // end of namespace SuperOpt


#endif // OPERAND_H

