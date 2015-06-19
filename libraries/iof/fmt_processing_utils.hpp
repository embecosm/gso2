#ifndef IOF_PROCESS_ALL_HPP_
#define IOF_PROCESS_ALL_HPP_


/** \file
    Provides functions for parsing and interpreting the format string
    and the format specification, used by both input and output streams.

    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying
    **  LICENSE_IOF_1_0.txt in root folder of library for complete
    **  terms and conditions).
    \endverbatim
*/

#include <iostream>
#include <cassert>

#include "iof/except.hpp"


namespace iof
{


namespace iof_private
{


/** Convert \a found to int and put in \a val. Assumes that found is
    positive (i.e. can be =0). If found < 0, it is assumed
    an error. Returns \a found if error, otherwise returns next
    char not part of the int (ie first char after int).
    */
inline
const char* getPosInt(const char* found, int& val)
{
    char* newFound = NULL;
    val = strtol(found, &newFound, 10);
    if (val < 0) val = 0;
    assert(val >= 0);
    assert(newFound != NULL);
    return newFound;
}


/// Character to use to escape chars from parser
#define IOF_PARSER_ESC_CHAR '!'


/// Set \a addFlag in \a flags, after clearing the \a clearFlag of \a flags
inline
void setFlags(std::ios::fmtflags& flags, std::ios::fmtflags addFlag,
std::ios::fmtflags clearFlag)
{
    flags &= ~clearFlag;
    flags |= addFlag;
}


/// Don't do anything to range of chars for input streams
inline void
consume(std::istream&, const char*, size_t)
{
}


/** Get a char from \a in stream and compare to \a *expect.
    \exception failure if failed to read any char from stream, or wrong char read
    \exception std::ios::failure if in.exceptions() and some failure occurs
    */
inline void
processChar(std::istream& in, const char* expect)
{
    char icc = EOF;
    in.get(icc); // could throw if in.exceptions()

    // if !in.exceptions(), could get here and have bad stream
    if (!in)
        throw iof_private::failure(expect);

    // succeeded; is it the right char?
    assert(icc != EOF);
    if (icc != *expect) // error
    {
        in.unget();
        in.setstate(std::ios::failbit);// could throw if in.exceptions()
        throw iof_private::failure(expect);
    }
}


/// Send the whole range of chars to stream
inline void
consume(std::ostream& strm, const char* start, size_t count)
{
    strm.write(start, std::streamsize(count));
}


/// Nothing to process for output
inline void
processChar(std::ostream&, const char*)
{
}


/** Handle the backslash f (\f) for input streams. This char does nothing
    special for input, so this function must have the same lines as the
    innermost while loop in processToNextMarker.
    */
inline void
handleBackslashF(std::istream& in, const char*&, const char*& str)
{
    processChar(in, str);
    str++;
}


/** Handle the backslash f (\f) for output streams. This must output the
    text parsed so far (dropping the \f) and flush the stream.
    */
inline void
handleBackslashF(std::ostream& out, const char*& start, const char*& str)
{
    // process all chars up to but not incl \f, then flush:
    consume(out, start, str-start);
    out.flush();
    // move past it, reset and continue to read
    str ++;
    start = str;
}


/** Process everything to first marker, and return position of first formatting
    code (first char after a %), but process '%%' as '%' and (for output) \f as
    a stream flush. Returns NULL if formatting code not found, meaning all of
    str has been used up.

    \exception iof_private::failure (for input stream only) if failed to read
    expected characters from stream.
    \exception ios::failure if \a strm.exceptions() is not 0 and failed to read
    from stream.
    */
template <class Stream>
inline
const char* processToNextMarker(Stream& strm, const char* str)
{
    assert(str != NULL);
    const char* start = str;
    // look at all chars for the '%' or '\0':
    while (*str != '\0')
    {
        while (*str != '%' && *str != '\0' && *str != '\f')
        {
            processChar(strm, str);
            str++;
        }

        if (*str == '\0')
        // got to end of string without any '%', consume all
        {
            if (str > start)
                consume(strm, start, str-start);
            return NULL;
        }
        else if (*str == '\f')
        {
            handleBackslashF(strm, start, str);
            continue;
        }

        // ok, found a '%', move past it
        assert(*str=='%');
        str++;
        assert(str>start);

        if (*str=='%') // a second % means we want '%' printed,
        // so print it then continue searching for "real" %
        {
            processChar(strm, str);
            consume(strm, start, str-start);
            str ++;
            start = str;
        }
        else  /* we've found it! A real '%' for format
                 we output everything to just before the '%' */
        {
            if (str > (start+1))
            {
                consume(strm, start, str-start-1);
            }
            else // first % was at beg of string, so nothing to output
                assert(str == (start+1));
            return str;
        }
    }
    return NULL;
}


} // namespace iof_private

unsigned countMarkers(const char* fmt)
{
    unsigned count = 0;
    std::ostringstream ss;

    while((fmt = iof_private::processToNextMarker(ss, fmt)))
        count++;

    return count;
}

} // namespace iof


#endif // IOF_PROCESS_ALL_HPP_
