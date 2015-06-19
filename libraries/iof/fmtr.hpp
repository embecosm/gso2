#ifndef IOF_FORMATTER_H
#define IOF_FORMATTER_H

/** \file
    Provides the fmtr class, which allows a more STL-like syntax for the 
    formatting of input and output. Formatter can be considered a manipulator, 
    or an overtaker, depending on the operator syntax you favor.  
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include "iof/fmtr_out.hpp"
#include "iof/fmtr_in.hpp"


namespace iof
{


/** Stream input/output formater (the most important class of iof). 
    This similar in behavior to a manipulator,
    that outputs to the stream the remainder of the arguments 
    according to its format string: 
    
    std::cout << iof::fmtr("hi %s, it is %s\n") << name << time;
    */
class fmtr
{
public:
    fmtr(): my_fmt("") {}
    explicit fmtr(const std::string& fmt): my_fmt(fmt) {}
    const std::string& fmt() const {return my_fmt;}
    
private:
    const std::string my_fmt;
};


inline iof_private::fmtr_out 
operator<<(std::ostream& out, const fmtr& fmtr)
{
    return iof_private::fmtr_out(out, fmtr.fmt(), true);
}


inline iof_private::fmtr_in
operator>>(std::istream& in, const fmtr& fmtr)
{
    return iof_private::fmtr_in(in, fmtr.fmt());
}


} // namespace iof


#endif // IOF_FORMATTER_H

