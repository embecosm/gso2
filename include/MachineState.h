#ifndef MACHINE_STATE_H
#define MACHINE_STATE_H

#include <array>

#include "Type.h"


namespace SuperOpt {


class IMachineState {
public:
  virtual int64_t getReg(RegisterID reg) const = 0;
  virtual void setReg(RegisterID reg, int64_t value) = 0;

  virtual uint64_t getPC() const = 0;
  virtual void setPC(uint64_t pc) = 0;
};


template<unsigned m_reg_count>
class MachineState : public IMachineState {
public:
  
  int64_t getReg(RegisterID reg) const override;
  void setReg(RegisterID reg, int64_t value) override;

  uint64_t getPC() const override;
  void setPC(uint64_t pc) override;

protected:
  MachineState() : IMachineState(), m_pc(0)
  {
    m_regs = {0};
  }

private:
  uint64_t m_pc;
  std::array<int64_t, m_reg_count> m_regs;
};


template<unsigned m_reg_count>
int64_t MachineState<m_reg_count>::getReg(RegisterID reg) const
{
  return m_regs[reg];
}

template<unsigned m_reg_count>
void MachineState<m_reg_count>::setReg(RegisterID reg, int64_t value)
{
  m_regs[reg] = value;
}

template<unsigned m_reg_count>
uint64_t MachineState<m_reg_count>::getPC() const
{
  return m_pc;
}

template<unsigned m_reg_count>
void MachineState<m_reg_count>::setPC(uint64_t pc)
{
  m_pc = pc;
}


} // end of namespace SuperOpt


#endif // TARGET_INFO_H

