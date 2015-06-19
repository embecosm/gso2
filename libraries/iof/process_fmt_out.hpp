#ifndef IOF_PROCESS_FMT_OUT_H
#define IOF_PROCESS_FMT_OUT_H

/** \file
    Provides functions for parsing and interpreting the format string
    and the format specification for output streams.  
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include <stdexcept>

#include "iof/fmt_processing_utils.hpp"


namespace iof 
{

namespace iof_private
{


/*  This aggregates all formatted output extension flags, ie not 
    supported by native std::ostreams (e.g. centering, truncation, etc). 
    */
class fmt_extensions
{
public:
    fmt_extensions(): 
        my_needCentering(false), my_oldNeedCenter(false), my_truncate(0), my_oldTruncate(0) {}
    bool operator==(const fmt_extensions& rhs) const 
    {
        return my_needCentering == rhs.my_needCentering && my_truncate == rhs.my_truncate;
    }
    
    void save()    {my_oldNeedCenter = my_needCentering; my_oldTruncate = my_truncate;}
    void restore() { my_needCentering = my_oldNeedCenter; my_truncate = my_oldTruncate;}
    
    void needCentering(bool state) {my_needCentering = state;}
    bool needCentering() const {return my_needCentering;}
    int truncate() const {return my_truncate;}
    int& truncate() {return my_truncate;}
    
    bool extensionsUsed() const {return my_needCentering || my_truncate > 0;}

private:
    bool my_needCentering;
    bool my_oldNeedCenter;

    int  my_truncate;
    int  my_oldTruncate;
};


inline const char* 
processToNextFmtMarker(std::ostream& strm, const char* str)
{
    return processToNextMarker(strm, str);
}


inline void 
setStreamFormat(std::ios& strm, std::ios::fmtflags flags, int width, int precision, char fillChar)
{
    // update formatting state of stream
    strm.flags(flags);
    // change w,p,f only if they have been changed:
    if (width>=0)     strm.width(width);
    if (precision>=0) strm.precision(precision);
    if (fillChar>0)   strm.fill(fillChar);
}


/// cancel any alignment flags, so stream will choose
inline void 
cancelAlignment(std::ostream& ostr) 
{
    const std::ios::fmtflags flags = ostr.flags() & ~std::ios_base::adjustfield;
    ostr.flags(flags);
}


/** \a found is chars to parse for int value; if no value, use *found 
    as fill character. Either way, \a found is changed to point to 
    last character used (so if no value, then only \a fillChar is 
    changed, otherwise, \a fillChar is unchanged, \a value is changed, 
    and \a found points to the last digit of the value. 
    */
inline void
getValueOrFill(const char*& found, char& fillChar, int& value)
{
    // if period is not followed by a number, interpret 
    // '.' as a fill char
    const char* digit = found + 1;
    assert(('9' - '0') == 9);
    if (*digit >= '0' && *digit <= '9')
    {
        found = getPosInt(digit, value);
        assert(found != digit);
        found --; // otherwise the found ++ below will cause missed char
    }
    else
        fillChar = *found;
}


/** Output everything in fmt to out till first marker (%),
    translate format codes to stream settings, and return
    pointer to next non-format character. Returns NULL
    if nothing left in fmt, ie fmt all sent to output.
    Throws exception if '%' not followed by 's' or ends
    string.
    */
inline const char* 
process1FmtSpec(std::ostream& strm, const char* fmt, fmt_extensions& extraFmt, 
    bool& restoreFmt, bool requireClosure = true)
{
	// if no format or format is empty just return
    if (fmt == NULL) return NULL;
    if (*fmt == '\0')
    {
        if (requireClosure)
            throw iof_private::failure(fmt, failure::MARKER_NOT_CLOSED);
        return NULL;
    }

    std::ios::fmtflags nowFlags = strm.flags();
    setFlags(nowFlags, std::ios::internal, std::ios::adjustfield);
    int width=-1, precision=-1;
    char fillChar=0;
    
    bool extraNumInfo=false; // used for '#'
    const char* found = fmt;
    bool moreFmtChars = true;
    while (moreFmtChars)
    {
        // end of marker? 
        switch (*found)
        {
            case 'S': restoreFmt = false;
            case 's': 
            case  0 : moreFmtChars = false; continue;
        }
        // take action if extraNumInfo was recently set
        switch (*found)
        {
            case 'X': case 'x': 
            case 'o': 
                if (extraNumInfo) nowFlags |= std::ios::showbase;
                break;
            case 'f': 
            case 'e': case 'E': 
            case 'g': case 'G': 
                if (extraNumInfo) nowFlags |= std::ios::showpoint;
                break;
        }
        // upper/lowercase for hex/scentific
        switch (*found)
        {
            case 'X': case 'E': case 'G': 
                nowFlags |= std::ios::uppercase; break;
            case 'x': case 'e': case 'f': 
                nowFlags &= ~std::ios::uppercase; break;
        }
        // everything else
        switch (*found)
        {
            // alignment:
            case '-':
            case '<':
                extraFmt.needCentering(false);
                setFlags(nowFlags, std::ios::left, std::ios::adjustfield);
                break;
            case '>':
                extraFmt.needCentering(false);
                setFlags(nowFlags, std::ios::right, std::ios::adjustfield);
                break;
            case '_': 
                extraFmt.needCentering(false);
                setFlags(nowFlags, std::ios::internal, std::ios::adjustfield);
                break;
            case '=':
                extraFmt.needCentering(true);
                cancelAlignment(strm);
                break;
            case '^': // let stream choose
                extraFmt.needCentering(false);
                cancelAlignment(strm);
                break;

            // base
            case 'X': case 'x':
                setFlags(nowFlags, std::ios::hex, std::ios::basefield);
                break;
            case 'o':
                setFlags(nowFlags, std::ios::oct, std::ios::basefield);
                break;
            case 'a': // turn on boolalpha
                nowFlags |= std::ios::boolalpha; 
                break;
            case 'b': // turn off boolalpha, ie show as bit 0 or 1
                nowFlags &= ~std::ios::boolalpha;
                break;
            case 'd': // decimal means ios::dec + fixed + prec=0
                setFlags(nowFlags, std::ios::dec,   std::ios::basefield);
                setFlags(nowFlags, std::ios::fixed, std::ios::floatfield);
                precision = 0;
                break;
                
            // fixed/scientific
            case 'f':
                setFlags(nowFlags, std::ios::fixed, std::ios::floatfield);
                break;
            case 'E': case 'e':
                setFlags(nowFlags, std::ios::scientific, std::ios::floatfield);
                break;
            case 'G': case 'g':
                // CLEAR all floatfield flags, so stream will CHOOSE
                nowFlags &= ~std::ios_base::floatfield;
                break;

            // width/precision
            case '@': 
                getValueOrFill(found, fillChar, extraFmt.truncate());
                break;
            case '.': 
                getValueOrFill(found, fillChar, precision);
                break;
            case '0': 
                // uncommon to set width to 0 so assume mean fill char
                fillChar = '0';
                break;
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': 
            case '9': {
            	const char* const afterWidth = getPosInt(found, width);
                assert(afterWidth != found);
            	// substract -1 otherwise the found ++ below will cause missed char:
            	found = afterWidth - 1; 
            	}
                break;

            // extra info
            case '+':
                nowFlags |= std::ios::showpos;
                break;
            case '#': 
                extraNumInfo = true; 
                break;
            case IOF_PARSER_ESC_CHAR: 
                {
                    found ++;
                	if (*found == '0') // if esc = 0, assume width
                		width = 0; 
                	else if (*found != '\0') // it's a fill char
                		fillChar = *found; 
                    else found --; // because the found++ below
                }
                break;
            default:                              // try a fill character
                fillChar = *found;
        }
        found ++;
    }
    setStreamFormat(strm, nowFlags, width, precision, fillChar);

    if (*found == '\0')
    {
        if (requireClosure)
            // the format string is the last part of the string, and is
            // not terminated by 's'; not valid
            throw iof_private::failure(fmt, failure::MARKER_NOT_CLOSED);
            
        found = NULL;
    }
    else 
    {
        found ++;
        if (*found == '\0')
            found = NULL;
    }

    assert(!found || *found);        
    return found;
}


/// Output atring \a obj to stream \a out, centered according to out.width()
inline void 
outputCentered(const std::string& obj, std::ostream& out)
{
    if (int(obj.size()) >= out.width())
    {
        out << obj;
        return;
    }
    
    assert(long(obj.size()) < long(out.width()));
    const size_t numSpaces = out.width() - obj.size();
    out.width(0);
    const char fill = (out.fill() ? out.fill() : ' ');
    if (numSpaces % 2 == 0)
    {
        const std::string spaces(numSpaces/2, fill);
        out << spaces << obj << spaces;
    }
    else
    {
        const std::string spacesL(numSpaces/2, fill);
        const std::string spacesR((numSpaces+1)/2, fill);
        assert(spacesL.size() + spacesR.size() == numSpaces);
        out << spacesL << obj << spacesR;
    }
}


/// Copy some format stuff from \a out into \a buffer
inline void 
setupExtensionBuffer(std::ostream& buffer, const std::ostream& out)
{    
    buffer.precision(out.precision());
    buffer.flags(out.flags());
    buffer.imbue(out.getloc());
    cancelAlignment(buffer);
}


// generic
template <typename TT> struct is_fund {typedef int No;};
// specializations for pointers and all fundamental types that can be output
template <typename TT> struct is_fund<TT*> {typedef int Yes;};
template <> struct is_fund<bool>  {typedef int Yes;};
template <> struct is_fund<char>  {typedef int Yes;};
template <> struct is_fund<short> {typedef int Yes;};
template <> struct is_fund<int>   {typedef int Yes;};
template <> struct is_fund<long>  {typedef int Yes;};
template <> struct is_fund<long long> {typedef int Yes;};
template <> struct is_fund<float> {typedef int Yes;};
template <> struct is_fund<double> {typedef int Yes;};
template <> struct is_fund<long double> {typedef int Yes;};
template <> struct is_fund<unsigned char>  {typedef int Yes;};
template <> struct is_fund<unsigned short> {typedef int Yes;};
template <> struct is_fund<unsigned int>   {typedef int Yes;};
template <> struct is_fund<unsigned long>  {typedef int Yes;};
template <> struct is_fund<unsigned long long> {typedef int Yes;};

    
/// Output fundamental types, centering if necessary; truncation ignored
template <typename TT>
inline typename is_fund<TT>::Yes
outputAdvanced(const TT& obj, std::ostream& out, const fmt_extensions& extraFmt)
{
    if (extraFmt.needCentering() && out.width() > 0)
    {
        std::ostringstream ostr;
        setupExtensionBuffer(ostr, out);
        ostr << obj;
        outputCentered( ostr.str(), out );
    }
    else
        out << obj;
        
    return 0;
}


// overload for std::string
inline void 
outputAdvanced(const std::string& obj, std::ostream& out, const fmt_extensions& extraFmt)
{
    const bool truncate = (extraFmt.truncate() > 0 && size_t(extraFmt.truncate()) < obj.size());
    if (extraFmt.needCentering() && out.width() > 0)
        outputCentered( (truncate ? obj.substr(0, extraFmt.truncate()) : obj), out );
    else
        out << (truncate ? obj.substr(0, extraFmt.truncate()) : obj);
}


inline void 
outputAdvancedNoTruncation(const char* obj, std::ostream& out, const fmt_extensions& extraFmt)
{
    if (extraFmt.needCentering() && out.width() > 0)
        outputCentered(obj, out);
    else
        out << obj;
}


/// this overload takes care of both char* and char[]
inline void 
outputAdvanced(const char* obj, std::ostream& out, const fmt_extensions& extraFmt)
{
    if (extraFmt.truncate() > 0 && size_t(extraFmt.truncate()) < strlen(obj)) 
    {
        char* buffer = new char[extraFmt.truncate()+1];
        strncpy(buffer, obj, extraFmt.truncate());
        buffer[extraFmt.truncate()] = 0;
        try {
            outputAdvancedNoTruncation(buffer, out, extraFmt);
        }
        catch (...) 
        {
            delete[] buffer; 
            throw;
        }
        delete[] buffer; 
    }
    else
        outputAdvancedNoTruncation(obj, out, extraFmt);
}


/** Specialization to output UDT's. UDT's use the width for the
    whole object instead of the default STL stream behavior, which 
    would apply the width to only the first fundamental object. 
    This specialization also handles centering if necessary.
    */
template <typename TT>
inline typename is_fund<TT>::No 
outputAdvanced(const TT& obj, std::ostream& out, const fmt_extensions& extraFmt)
{
    if (out.width() > 0 || extraFmt.truncate() > 0)
    {
        std::ostringstream ostr;
        setupExtensionBuffer(ostr, out);
        ostr << obj;
        const bool truncate = (extraFmt.truncate() > 0);
        if (extraFmt.needCentering())
            outputCentered( (truncate ? ostr.str().substr(0,extraFmt.truncate()) : ostr.str()), out );
        else
            out << (truncate ? ostr.str().substr(0,extraFmt.truncate()) : ostr.str());
    }
    else
    {
        cancelAlignment(out);
        out << obj;
    }
    
    return 0;
}

    
} // namespace iof_private

} // namespace iof

#endif // IOF_PROCESS_FMT_OUT_H

