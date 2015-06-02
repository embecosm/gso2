#ifndef REGISTER_H
#define REGISTER_H


namespace SuperOpt {


class Register {
public:
  Register() = delete;
  Register(const unsigned bit_width)
    : m_bit_width(bit_width)
  {}

  unsigned getBitWidth() const { return m_bit_width; }

private:
  const unsigned m_bit_width;
};


} // end of namespace SuperOpt


#endif // REGISTER_H

