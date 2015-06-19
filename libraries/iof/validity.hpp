#ifndef IOF_VALIDITY_HPP_
#define IOF_VALIDITY_HPP_

/** \file
    Provides validity class to test for validity of formatted input. 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include <string>


namespace iof 
{


/** Represents the validity state of a stream. Currently used only for input 
    problems: an instance of this class can be populated by iof::fmtr to get 
    details on input errors, as in 
    
    \code
    iof::validity ok;
    while  (cin >> iof::fmtr("%s:%s") >> hr >> min >> ok) 
        // do something
        // ...
    if (!ok) // access problemPos and fmt data members
        // ...
    \endcode    
    */
struct validity
{
    int problemPos;  ///< index of where problem was, in format string
    std::string fmt; ///< the format string
    
    validity(): problemPos(-1) {}
    void set(const char* problemPos_, const std::string& fmtStr)
    {
        problemPos = (problemPos_ ? int(problemPos_ - fmtStr.c_str()) : -1);
        fmt = fmtStr;
    }
    
    /// return true if ok, false if not
    operator bool()  const {return problemPos < 0;}
    bool operator!() const {return problemPos >= 0;}
};


} // namespace


#endif // IOF_VALIDITY_HPP_
