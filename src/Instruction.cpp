#include <vector>
#include <iostream>

#include "Type.h"
#include "TargetInfo.h"
#include "Instruction.h"


namespace SuperOpt {


class OperandClass;


InstrDesc::InstrDesc()
  : m_n_outputs(0), m_operands()
{}


// Add a new operand to the instruction description. All output operands must
// be added before any inputs.
InstrDesc& InstrDesc::
addOperand(const OperandClass& op_class, const bool is_output)
{
  m_operands.push_back(&op_class);
  if (is_output) {
    assert(op_class.isReg() && "Output operand must be a register!");
    m_n_outputs++;
  }
  return *this;
}

// Check whether an operand is an output
bool InstrDesc::isOutput(const unsigned op_num) const {
  assert(op_num < m_operands.size() && "index out of bounds!");
  return op_num < m_n_outputs;
}

// Check whether an operand is an input
bool InstrDesc::isInput(const unsigned op_num) const {
  assert(op_num < m_operands.size() && "index out of bounds!");
  return !isOutput(op_num);
}

// Get the operand class associated with the given operand
const OperandClass& InstrDesc::
getOperandClass(const unsigned op_num) const
{
  assert(op_num < m_operands.size() && "index out of bounds!");
  return *m_operands[op_num];
}

// Dump the instruction description to an output stream
void InstrDesc::dump(std::ostream& out) const
{
  out << "<(outs ";

  int i = 0;
  while (i < outputCount()) {
    out << "A\n";
    if (i != 0) {
      out << ", ";
    }
    getOperandClass(i).dump(out);
    i++;
  }

  out << "), (ins ";

  while (i < operandCount()) {
    out << "B\n";
    if (i != outputCount()) {
      out << ", ";
    }
    getOperandClass(i).dump(out);
    i++;
  }
  out << ")>\n";
}


// Construct a new uninitialized instruction.
Instruction::Instruction()
  : m_opcode(0), m_desc(nullptr), m_operands(0)
{}

// Get mutable and immutable references to a specified operand
Operand& Instruction::getOperand(const unsigned op_num)
{
  assert(op_num < m_desc->operandCount() && "index out of bounds!");
  return m_operands[op_num];
}
const Operand& Instruction::getOperand(const unsigned op_num) const
{
  assert(op_num < m_desc->operandCount() && "index out of bounds!");
  return m_operands[op_num];
}

// Set the instruction description, this holds details about the type of
// operands and whether they are inputs or outputs.
void Instruction::setDesc(const TargetInfo& target_info, const Opcode opcode)
{
  m_opcode = opcode;
  m_desc = &target_info.getInstr(opcode);

  // ensure there is enough space for the operands
  m_operands.resize(m_desc->operandCount());
}

// Dump the instruction to an output stream
void Instruction::dump(std::ostream& out) const
{
  out << '<' << getOpcode() << ' ';
  for (unsigned i = 0; i < getDesc().operandCount(); i++) {
    if (i != 0) {
      out << ", ";
    }
    getOperand(i).dump(out);
  }
  out << '>';
}


} // end of namespace SuperOpt

