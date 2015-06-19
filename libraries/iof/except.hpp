#ifndef IOF_EXCEPTIONS_H
#define IOF_EXCEPTIONS_H

/** \file
    Provides exceptions used in the input and output functions and classes. 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include <sstream>
#include <stdexcept>

namespace iof 
{


namespace iof_private
{

/*  Formatted I/O can really benefit from knowing for which arg did the I/O fail. 
    So all std::ios::failure and all error bits (eof, fail, bad) are trapped
    and converted to a iof_private::failure exception that can get caught at the top 
    level and information saved there. Note that iof_private::failure should never 
    escape out of the iof library. 
    */
class failure
{
public:
    enum err_type 
    {
        IO_FAILURE, 
        MARKER_NOT_CLOSED
    };
    
    failure(const char* problem, err_type type = IO_FAILURE)
        : problem(problem), errType(type) {}
    
    const char* const problem;
    const err_type errType;
    
};

}


/** Base of hierarchy of format-string related exceptions (so this 
    does not include e.g. iof::in_failure, which derive from ios::failure since
    it is ios-flags related).
    */
class except: public std::runtime_error
{
    public:
        except(const std::string& msg): std::runtime_error(msg) {}
};


/** Exception that gets thrown when no closing char found for marker.
    This means the end of format string was reached before finding 's' 
    or 'S' (or ')', for input skipping). The \a markerStart indicates 
    the start of the marker, since this is the last 
    known "valid" point in format string. 
    */
class marker_not_closed: public iof::except
{
    public:
        marker_not_closed(const iof_private::failure& fail, const std::string& fmt, 
            const std::string& msg): except(msg), 
            start(fail.problem ? fail.problem - fmt.c_str() : -1), fmt(fmt) {}
        
        const size_t start;
        const std::string& fmt;
};


/// Exception thrown when a format marker is not closed before end of a format string
class marker_not_closed_fmt: public marker_not_closed
{
    public:
        marker_not_closed_fmt(const iof_private::failure& fail, const std::string& fmt)
            : marker_not_closed(fail, fmt, "Unclosed format marker") {}
};

/// Exception thrown when a skip marker is not closed before end of a format string
class marker_not_closed_skip: public marker_not_closed
{
    public:
        marker_not_closed_skip(const iof_private::failure& fail, const std::string& fmt)
            : marker_not_closed(fail, fmt, "Unclosed skip marker") {}
};


/// Exception to indicate that input read has failed
class in_failure: public std::ios::failure
{
    public: 
        in_failure(size_t pos, const std::ios::failure& fail)
            : std::ios::failure(msg(pos, fail)), pos(pos) {}
        
        static std::string msg(size_t pos, const std::ios::failure&) {return msg(pos);}
        
        static std::string msg(size_t pos) 
        {
            std::ostringstream msg;
            msg << "Pos " << (unsigned int)(pos) << " of fmt string caused input exception";
            return msg.str();
        }
        
        const size_t pos;
        
};


/** Exception to indicate that there are more format markers 
    than there are objects to input or output.
    */
class too_many_markers: public iof::except
{
    public: 
        too_many_markers(size_t pos, const char* fmt)
            : except(msg(pos, fmt)), pos(pos) {}
        
        static std::string msg(size_t pos, const char* fmt) 
        {
            std::ostringstream msg;
            msg << "Too many format markers, starting at pos " 
                << (unsigned int)(pos) << " of '" << fmt << "'";
            return msg.str();
        }
        
        const size_t pos;
};


/** Exception to indicate that a fmt_spec object has been 
    sent to stream without using 
    persistence, which means the formatting is lost.
    */
class fmt_no_effect: public iof::except
{
    public: 
        static inline int getPos(const char* next, const char* fmt) {return next ? int(next - fmt): -1;}
        
        fmt_no_effect(const char* next, const char* fmt)
            : except(msg(getPos(next, fmt), fmt)), pos(getPos(next, fmt)) {}
        
        static std::string msg(int pos, const char* fmt) 
        {
            std::ostringstream msg;
            std::ostringstream posStr; 
            if (pos >= 0) posStr << "pos " << pos;
            else posStr << "end"; 
            msg << "fmt_spec obj used at " << posStr.str() << " of '" << fmt 
            	<< "' needs to be persistent to have an effect";
            return msg.str();
        }
        
        const size_t pos;
};


} // namespace 

#endif // IOF_EXCEPTIONS_H
