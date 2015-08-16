#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdint.h>
#include <vector>
#include <string>
#include <functional>

class Combinations
{
public:
    Combinations(unsigned size_, unsigned n_);

    void getSelection(std::vector<unsigned> &sel);

    bool next();

private:
    uint64_t current, max;
    unsigned n, size;
};

class Instruction;
class Slot;

std::string print(const std::vector<Instruction *> &insns,
    std::vector<Slot*> &slots);


bool parseInstructionList(std::string input, std::vector<std::function<Instruction *()>> factories,
    std::vector<Instruction*> &insns, std::vector<Slot*> &slots);

#endif
