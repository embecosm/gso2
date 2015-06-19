#ifndef IOF_PROCESS_FMT_IN_H
#define IOF_PROCESS_FMT_IN_H

/** \file
    Provides functions for parsing and interpreting the format string
    and the format specification for input streams.  
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/

#include <climits>
#include <stdexcept>

#include "iof/fmt_processing_utils.hpp"


namespace iof 
{


namespace iof_private
{


/** For input, uses current width and fillChar to call 
    strm.ignore(width, fillChar); if no fill char and width < 0, skips one 
    char; if no fill char and width==0, width=INT_MAX otherwise width.
    \exception iof_private::failure if skipping hit the EOF mark, or if some 
    failure to read chars occurred
    */
inline void 
execSkip(std::istream& strm, int width, char fillChar, const char* skipCmd)
{
    if (fillChar > 0) 
    {
        if (width > 0) 
            strm.ignore(width, fillChar);
        else 
            strm.ignore(INT_MAX, fillChar);
            // WARNING: using std::numeric_limits<int>::max() instead of INT_MAX
            // fails compilation in VC++ in *certain* cases (unknown cause)
    }
    else if (width < 0)
        strm.ignore(1);
    else
        strm.ignore(width == 0 ? INT_MAX : width);
    
    if (!strm)
        throw iof_private::failure(skipCmd);
}


static const char skipOpen = '[';
static const char skipClose = ']';


/** Note that \a fmt must point to the first char 
    after the char that opens the skip command (see skipOpen static const). 
    \exception iof_private::failure if stream error (attempt to skip 
    past eof or marker not closed)
    */
inline const char* 
doSkip(std::istream& in, const char* fmt, bool requireClose = true)
{
    assert(fmt != NULL);
    const char* const start = fmt;
    bool doneSkip = false;
    char fillChar = 0;
    int width = -1;
    while (*fmt != 0 && *fmt != skipClose)
    {
        doneSkip = false;
        switch (*fmt)
        {
            case '0': 
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': 
            case '9': {
                const char* const afterWidth = getPosInt(fmt, width);
                assert(afterWidth != fmt);
                // substract -1 otherwise loop will cause missed char:
                fmt = afterWidth - 1;
                }
                break;
                
            case '>':
                execSkip(in, width, fillChar, fmt);
                doneSkip = true;
                break;
                
            case IOF_PARSER_ESC_CHAR: 
                fmt ++; 
                if (*fmt != 0) fillChar = *fmt;
                else fmt--; // because eos and fmt++ below
                break;

            default: 
                fillChar = *fmt;
        }
        
        fmt ++;
    }

    if (*fmt == 0 && requireClose)
        // the skip command is the last part of the string, and is
        // not terminated by ')'; not valid
        throw iof_private::failure(start, failure::MARKER_NOT_CLOSED);
    
    if (!doneSkip) 
        execSkip(in, width, fillChar, fmt);
        
    // if at end, return end; otherwise, skip closing char
    return (*fmt == 0 ? fmt : fmt+1);
}


/** Processes chars in \a str until reaches first format marker. 
    It checks input characters, and processes skipping markers.
    Returns NULL if no format marker found before end of string. 
    Otherwise, returns pointer to char after marker (which could 
    point to a format spec char or '\0').
    
    \exception iof_private::failure if could not read a char from stream or 
        attempted skip failed due to stream failure (eof, fail, bad) or 
        wrong char read or no closing marker character on skip command
    */
inline const char* 
processToNextFmtMarker(std::istream& strm, const char* str)
{
    const char* found = str;
    bool needMore = (found != NULL);
    while (needMore) 
    {
        found = processToNextMarker(strm, found);
        if (found && *found == skipOpen) 
            found = doSkip(strm, found+1);
        else
            needMore = false;
    }
    
    return found;
}


/// Set format flags of input stream, and width (only if >= 0)
inline void 
setStreamFormat(std::istream& strm, std::ios::fmtflags flags, int width, 
    int, char fillChar)
{
    // update formatting state of stream
    strm.flags(flags);
    if (fillChar!=0) strm.fill(fillChar);
    if (width>=0)    strm.width(width);
}


/** Process a format string \a fmt for one object, from \a strm input stream, until 
    closing 's' or 'S' is found (or end of string). The \a restoreFmt will be false 
    if 'S' was found instead of 's'.
    \exception iof_private::failure if 's' or 'S' not found before end 
    of \a fmt string
    */
inline
const char* process1FmtSpec(std::istream& strm, const char* const fmt, 
    bool& restoreFmt, char& delim, bool requireClosure = true)
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
    int width=-1;
    int fillChar=0;
    
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
        // everything else
        switch (*found)
        {
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
            case 'd': // decimal means ios::dec + fixed=0
                setFlags(nowFlags, std::ios::dec,   std::ios::basefield);
                setFlags(nowFlags, std::ios::fixed, std::ios::floatfield);
                break;
                
            case '0': 
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': 
            case '9': {
                const char* const afterWidth = getPosInt(found, width);
                assert(afterWidth != found);
                // substract -1 otherwise the found ++ below will cause missed char:
                found = afterWidth - 1; 
                }
                break;

            case IOF_PARSER_ESC_CHAR: 
                {
                    if (*(found+1) != 0)
                        fillChar = *(++found);
                }
                break;
            case '.': // possibly delimiter x in '..x', or fill
                if (*(found+1) == '.') // next is also dot? 
                {
                    found ++;
                    if (*(found+1) != 0)
                        delim = *(++found); 
                }
                else //no, must be fill
                    fillChar = *found;
                break;
            // extra info
            case '~': // skip whitespace
                nowFlags &= ~std::ios::skipws;
                break;
            // fallback
            default: // assume a fill character
                fillChar = *found;
        }
        found ++;
    }
    setStreamFormat(strm, nowFlags, width, 0, fillChar);

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
        // go past the closing marker char
        found ++;
        if (*found == '\0')
            found = NULL;
    }
    
    assert(!found || *found);
    return found;
}

} // namespace iof_private


} // namespace iof

#endif // IOF_PROCESS_FMT_IN_H

