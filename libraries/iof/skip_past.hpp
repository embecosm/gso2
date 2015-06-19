#ifndef IOF_IGNORE_HPP_
#define IOF_IGNORE_HPP_


/** \file
    Provides the skip_past class, which allows for run-time specification of 
    input skipping, using one or more calls to istream::ignore().  
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include "iof/process_fmt_in.hpp"
#include "iof/except.hpp"


namespace iof
{


/** Calls istream::ignore() based on a "skipper specification"
    or on explicitly specified width/fill char. In latter case, use width=0 
    to mean infinity (gobble until EOF). 
    */
class skip_past
{
public:
    skip_past(const std::string& ign): my_ignStr(ign), my_width(-1), my_fill(0) {}
    skip_past(int width, char fill = 0): my_width(width), my_fill(fill) 
    {
        if (my_width < 0) 
            throw std::invalid_argument("Width in skip_past must be >= 0");
    }

    void applyTo(std::istream& in) const
    {
        try 
        {
            if (!my_ignStr.empty()) 
                iof_private::doSkip(in, my_ignStr.c_str(), false);
            else 
                iof_private::execSkip(in, my_width, my_fill, my_ignStr.c_str());
        }
        catch (const iof_private::failure& fail)
        {
            // silence exception since fmtr will see that stream is fail();
            // if not using fmtr, it's up to the user to check stream
            // state; if in.exceptions(), we never get here
            assert(fail.errType != iof_private::failure::MARKER_NOT_CLOSED);
        }
    }
    
private:
    const std::string my_ignStr;
    const int my_width;
    const char my_fill;
};


inline std::istream& 
operator>>(std::istream& in, skip_past ign)
{
    ign.applyTo(in);
    return in;
}


} // namespace 


#endif // IOF_IGNORE_HPP_
