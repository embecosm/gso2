#include <vector>
#include <iostream>

#include "InstrSequence.h"
#include "InstrContext.h"
#include "TargetInfo.h"
#include "Instruction.h"
#include "Type.h"


namespace SuperOpt {


InstrSequence::InstrSequence(const TargetInfo& target_info,
                             unsigned len,
                             RegMask reg_mask)
  : m_instrs(len)
{
  initInstrSequence(target_info, 0, reg_mask);
}


// Initialize all of the instructions in turn starting with the instruction
// at the provided position.
void InstrSequence::
initInstrSequence(const TargetInfo& target_info,
                  unsigned seq_pos,
                  RegMask reg_mask)
{
  const unsigned seq_len = m_instrs.size();

  while (seq_pos < seq_len) {
    // initialize the current instruction based on the live register mask
    InstrContext& instr_context = m_instrs[seq_pos];
    instr_context.setInstruction(target_info, 0, reg_mask);

    // get live output registers from the instruction and use to initialize
    // the next
    reg_mask = instr_context.getLiveOutRegMask();
    seq_pos++;

    // TODO: perform sanity checks as new instructions are added
    // Peephole?
  }
}


// Iterate to the next valid instruction sequence.
bool InstrSequence::
nextInstrSequence(const TargetInfo& target_info)
{
  const unsigned seq_len = m_instrs.size();

  int pos = seq_len - 1;
  while (pos >= 0) {
    InstrContext& instr_context = m_instrs[pos];

    // iterate the operands for the instruction at the current position
    bool wrap = instr_context.nextOperands(pos, seq_len);
    if (!wrap) {
      break;
    }

    // If the operands wrapped, then iterate the instruction
    wrap = instr_context.nextInstruction(target_info);
    if (!wrap) {
      break;
    }

    // No more instructions at this position, so carry on to the previous
    // instruction.
    pos--;
  }

  // If the first instruction wrapped we're completely out of combinations
  if (pos < 0) {
    return true;
  }

  // TODO: Perform sanity checks on the new set of operands up to the current
  // position. Peephole?

  // If we only updated the last instruction and didn't wrap, then no
  // reinitialization needs to be done.
  if (pos == seq_len - 1) {
    return false;
  }

  // The current instruction has updated, so we need to reinitialize the
  // following instructions based on its live output mask.
  assert(pos < seq_len - 1);
  initInstrSequence(target_info, pos + 1, m_instrs[pos].getLiveOutRegMask());
  return false; // success
}


void InstrSequence::dump(std::ostream& out) const
{
  //out << "{\n";
  for (unsigned i = 0; i < m_instrs.size(); i++) {
    const InstrContext& instr_context = m_instrs[i];
    instr_context.getInstr().dump(out);
    out << '\n';
  }
  out << '\n';
  //out << "}\n";
}


} // end of namespace SuperOpt

