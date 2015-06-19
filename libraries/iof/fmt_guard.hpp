#ifndef IOF_FMT_GUARD_HPP_
#define IOF_FMT_GUARD_HPP_

/** \file
    Provides the stream_fmt class to aggregate the data that 
    constitutes the "formatting state" of a std::iostream.
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/

#include "iof/fmt_spec.hpp"


namespace iof 
{
    

/** Restore formatting, at guard destruction, that stream had when guard 
    constructed. Can be used with any stream derived from std::istream or 
    std::ostream.
    */
class fmt_guard
{
    public:
        fmt_guard(std::ios& strm): my_fmt(strm), my_strm(strm) {}
        
        template <typename Stream>
        fmt_guard(Stream& strm, const fmt_spec& newFmt)
            : my_fmt(strm), my_strm(strm) {strm << newFmt;}
            
        template <typename Stream>
        fmt_guard(Stream& strm, const char* newFmt)
            : my_fmt(strm), my_strm(strm) {strm << fmt_spec(newFmt);}
            
       ~fmt_guard() { my_fmt.applyTo(my_strm); }
       
    private:
        const stream_fmt my_fmt;
        std::ios&        my_strm;
};

} // namespace iof

#endif // IOF_FMT_GUARD_HPP_
