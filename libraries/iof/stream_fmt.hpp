#ifndef IOF_STREAM_FMT_HPP_
#define IOF_STREAM_FMT_HPP_

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

#include <ios>

namespace iof 
{
    
/** Represent the format of an (input or output) stream. This aggregates
    the std::ios::fmtflags with width, precision and fill character. 
    */
struct stream_fmt
{
    const int width, precision;
    const char fillChar;
    const std::ios::fmtflags flags;
    
    
    stream_fmt(const std::ios& strm)
        : width(strm.width()), precision(strm.precision()), 
          fillChar(strm.fill()), flags(strm.flags()) {}

    stream_fmt(const stream_fmt& fmt)
        : width(fmt.width), precision(fmt.precision), 
          fillChar(fmt.fillChar), flags(fmt.flags) {}

    /// Apply this format to \a stream
    void applyTo(std::ios& strm) const 
    {
        strm.fill(fillChar);
        strm.precision(precision);
        strm.width(width);
        strm.flags(flags);
    }

    bool operator==(const stream_fmt& rhs) const 
    {
        return rhs.width     == width 
            && rhs.precision == precision 
            && rhs.fillChar  == fillChar
            && rhs.flags     == flags;
    }
    bool operator!=(const stream_fmt& rhs) const {return !(*this == rhs);}
    
};


} // namespace iof


#endif // IOF_STREAM_FMT_HPP_
