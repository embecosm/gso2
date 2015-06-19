#ifndef IOF_FMT_SPEC_HPP_
#define IOF_FMT_SPEC_HPP_

/** \file
    Provide classes for aggregating and managing the formatting 
    information of a std::iostream.
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/

#include "iof/process_fmt_in.hpp"
#include "iof/process_fmt_out.hpp"
#include "iof/stream_fmt.hpp"


namespace iof 
{
    

namespace iof_private
{

	
template <typename TT>
class formatted
{
public:
    const std::string fmt;
    const TT& obj; 

	formatted(const std::string& fmt, const TT& obj): fmt(fmt), obj(obj) {}
    inline void applyTo(std::ostream&, fmt_extensions& fmtExtensions) const;
    
private:
};

} // namespace iof_private


/** Represent a pure "format specification", ie the format
    specification characters that can appear between a 
    '\%' and 's' in a format string. 
    
    Warning: the string is assumed to start the format
    specification. I.e. if first char of format spec is '%' 
    it will be set the fill char to '%'. 
    */
class fmt_spec
{
    public:
        fmt_spec() {}
        fmt_spec(const std::string& fmt): my_fmt(fmt) {}
        void operator=(const std::string& fmt) {my_fmt = fmt;}
        
        bool operator==(const std::string& fmt) const {return my_fmt == fmt;}
        bool operator==(const fmt_spec& rhs) const { return rhs.my_fmt == my_fmt; }
        bool operator!=(const fmt_spec& rhs) const { return ! (*this == rhs); }
        
        const std::string& str() const {return my_fmt;}
        
        /// Create a temporary "formatted" object
        template <typename TT> 
        iof_private::formatted<TT> operator()(const TT& obj) const 
        {
            return iof_private::formatted<TT>(my_fmt, obj);
        }

    private:
        std::string my_fmt;
};


/// Send \a fmt directly to \a out std::ostream, without iof::fmtr
inline std::ostream& 
operator<<(std::ostream& out, const fmt_spec& fmt)
{
    bool restore; // not used
    iof_private::fmt_extensions fmtExtensions;
    iof_private::process1FmtSpec(out, fmt.str().c_str(), fmtExtensions, restore, false);
    if (fmtExtensions.extensionsUsed()) 
        throw std::runtime_error(
        "Formatting extensions (centering, ...) not supported without iof::fmtr");
    return out;
}


/// Send \a fmt to \a in std::istream; same as fmt.applyTo(out).
inline std::istream& 
operator>>(std::istream& in, const fmt_spec& fmt)
{
    bool restore; // not used
    char delim = 0; // not used
    iof_private::process1FmtSpec(in, fmt.str().c_str(), restore, delim, false);
    return in;
}



template <typename TT> 
inline void 
iof_private::formatted<TT>::applyTo(std::ostream& out, fmt_extensions& fmtExtensions)
const
{
    bool restore; // not used
    iof_private::process1FmtSpec(out, fmt.c_str(), fmtExtensions, restore, false);
    outputAdvanced(obj, out, fmtExtensions);
}


/// Direct output to a stream
template <typename TT>
inline std::ostream& 
operator<<(std::ostream& out, const iof_private::formatted<TT>& fmtdObj)
{
    iof_private::fmt_extensions fmtExtensions;
    fmtdObj.applyTo(out, fmtExtensions);
    if (fmtExtensions.extensionsUsed()) 
        throw std::runtime_error(
        "Formatting extensions (centering, ...) not supported without iof::fmtr");
    return out;
}


} // namespace iof

#endif // IOF_FMT_SPEC_HPP_

