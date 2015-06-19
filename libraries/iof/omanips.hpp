#ifndef IOF_OUT_MANIPS_HPP_
#define IOF_OUT_MANIPS_HPP_


/** \file
    Provides two output "manipulators" that can be used with the output 
    functions (prints, coutf, etc). 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include <ostream>


namespace iof 
{


/*  Allow flushing with prints(stream, fmt, arg1..argN, flush) and
    endlining with prints(stream, fmt, arg1..argN, eol). The proper
    way would be to define e.g. 
    ostream& eol(ostream& output) {return output << endl;}
    but VC++ doesn't handle passing function pointers as template 
    parameters in the prints(), tostr(), etc overloads.  So enum 
    is used. 
    */
    
namespace iof_private_manips
{
    enum HideEol {eol};
    enum HideFlush {flush};
};
/// this defines the eol "manipulator"
static const iof_private_manips::HideEol   eol   = iof_private_manips::eol;
/// this defines the flush "manipulator"
static const iof_private_manips::HideFlush flush = iof_private_manips::flush;

// allow sending eol to stream, causing std::endl to be sent
inline std::ostream& 
operator<<(std::ostream& out, iof_private_manips::HideEol)
{
    return out << std::endl;
}
// allow sending flush to stream, causing stream to be flushed
inline std::ostream& 
operator<<(std::ostream& out, iof_private_manips::HideFlush)
{
    return out.flush();
}


} // namespace 


#endif // IOF_OUT_MANIPS_HPP_
