#include <iostream>

#include "TargetInfo.h"
#include "InstrSequence.h"
#include "AAP.h"


using namespace SuperOpt;


int main()
{
  TargetInfo& target_info = *(new TargetInfo());

  AAP::init(target_info);

  InstrSequence instr_seq(target_info, 5, 1);

  uint64_t a = 0;

  bool wrap = false;
  while (!wrap) {
    wrap = instr_seq.nextInstrSequence(target_info);
    a++;
  }
  std::cout << a << std::endl;
}

