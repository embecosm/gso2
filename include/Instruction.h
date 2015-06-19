#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <vector>
#include <cassert>
#include <iostream>

#include "Type.h"
#include "Operand.h"


namespace SuperOpt {


class OperandClass;
class TargetInfo;
class Instruction;
class IMachineState;


typedef void (*ExecuteFn)(const Instruction&, IMachineState*);


class InstrDesc {
public:
  InstrDesc();

  InstrDesc& addOperand(const OperandClass& op_class, const bool is_output);

  bool isOutput(const unsigned op_num) const;
  bool isInput(const unsigned op_num) const;

  unsigned outputCount() const { return m_n_outputs; }
  unsigned operandCount() const { return m_operands.size(); }

  void setExecuteFn(ExecuteFn execute_fn) {
    assert(execute_fn != nullptr);
    m_execute_fn = execute_fn;
  }
  ExecuteFn getExecuteFn() const { return m_execute_fn; }

  const OperandClass& getOperandClass(const unsigned op_num) const;

  void dump(std::ostream& out) const;

private:
  unsigned m_n_outputs;
  std::vector<const OperandClass*> m_operands;

  ExecuteFn m_execute_fn;
};


class Instruction {
public:
  Instruction();

  // Get a reference to a mutable operand
  Operand& getOperand(const unsigned op_num);
  const Operand& getOperand(const unsigned op_num) const;

  void setDesc(const TargetInfo& target_info, const Opcode opcode);

  Opcode getOpcode() const { return m_opcode; }
  const InstrDesc& getDesc() const { return *m_desc; }

  void dump(std::ostream& out) const;

private:
  // Hold a pointer to the instruction definition here for sanity checks when
  // setting operands, and to avoid continually looking up the definition
  // in the target info
  Opcode m_opcode;
  const InstrDesc *m_desc;
  std::vector<Operand> m_operands;
};


} // end of namespace SuperOpt


#endif // INSTRUCTION_H

