#ifndef INSTR_SEQUENCE_H
#define INSTR_SEQUENCE_H

#include <vector>
#include <iostream>

#include "InstrContext.h"
#include "Type.h"


namespace SuperOpt {


class TargetInfo;


class InstrSequence {
public:
  InstrSequence() = delete;

  // Create a new instruction sequence of a provided length with a starting
  // Live-In register mask
  InstrSequence(const TargetInfo& target_info,
                unsigned len,
                RegMask reg_mask);

  // Initialize the sequence of instruction, starting with the instruction
  // after the given position in the sequence.
  void initInstrSequence(const TargetInfo& target_info,
                         unsigned start_pos,
                         RegMask reg_mask);

  bool nextInstrSequence(const TargetInfo& target_info);

  void dump(std::ostream& out) const;

private:
  // Holds the context for each of the instructions in the superoptimizer
  std::vector<InstrContext> m_instrs;
};


} // end of namespace SuperOpt


#endif // INSTR_SEQUENCE_H

