#include <iostream>

#include "TargetInfo.h"
#include "InstrSequence.h"
#include "AAP.h"


using namespace SuperOpt;


int main()
{
  TargetInfo& target_info = *(new TargetInfo());

  AAP::init(target_info);

  InstrSequence instr_seq(target_info, 2, 1);
  instr_seq.dump(std::cout);

  bool wrap = false;
  while (!wrap) {
    wrap = instr_seq.nextInstrSequence(target_info);
    instr_seq.dump(std::cout);
  }
}

