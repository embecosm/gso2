#ifndef IOF_FMTR_IN_HPP_
#define IOF_FMTR_IN_HPP_

/** \file
    Provides private functions to input stuff via the fmtr_in class. 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include "iof/except.hpp"
#include "iof/fmt_spec.hpp"
#include "iof/skip_past.hpp"
#include "iof/validity.hpp"


namespace iof 
{


namespace iof_private
{


/** Consume all consecutive characters from current position in 
    \a in stream, using in.fill() to specify which character. 
    When returns, the next read is garanteed not to yield the 
    in.fill() character. 
    */
inline void consumeFill(std::istream& in)
{
    int cc = in.peek();
    assert(in.fill() != EOF);
    while (cc == in.fill()) 
    {
        in.get();
        cc = in.peek();
    }
}


/** Scan from input stream into \a obj. Just consumes 
    any fill characters, then does input. 
    */
template <typename TT> 
inline void 
doScan(std::istream& in, TT& obj, const char)
{
    if (in.fill() != ' ') consumeFill(in);
    in >> obj;
}


/// Default delimiter for delimited string input
static const char defaultDelim = ' ';


/** Specialization of doScan() for strings. The only difference is 
    that we can use ::getline(in, obj, delim) if delim is not 
    \a defaultDelim. In this case, the delimiter character
    remains in the stream, *except* if it is '\n'. 
    */
inline void 
doScan(std::istream& in, std::string& obj, const char delim)
{
    if (in.fill() != ' ') consumeFill(in);
    if (delim != defaultDelim)
    {
        getline(in, obj, delim); // uses in.width()
        if (in && delim != '\n') in.unget();
    }
    else
        in >> obj;
}


class fmtr_in
{
public:
    /** Note: it is better to have \a in.exceptions() == 0 because we're in constructor; if it is non-zero,
        the std::ios::exception is not caught, causing failure of construction, so error info can't
        be stored anywhere. 
        */
    fmtr_in(std::istream& in, const std::string& fmt)
        : my_in(in), 
          my_saveFmt(in),
          my_fmt(fmt), 
          my_next(NULL), 
          my_saveFlags(my_in.flags()), 
          my_width(my_in.width()), 
          my_fillChar(my_in.fill()),
          my_delim(defaultDelim),
          my_delimDynFmt(0),
          my_useSavedWidth(true),
          my_throwing(false),
          my_problemPos(NULL)
    {
        if (my_in.fail()) 
        // if failure upon entry, can't do anything, signal that end of format
        // reached (my_next=NULL) and set my_problemPos to beginning of fmt
        {
            my_problemPos = my_fmt.c_str();
        }
        else
        {
            my_next = my_fmt.c_str();
            assert(my_next);
            processToNextFmtMarker();
        }
    }

   ~fmtr_in()
    {
        // if no problem and not throwing, check valid final state:
        if (!my_problemPos && !my_throwing) 
            checkFinalState();
        my_saveFmt.applyTo(my_in);
    }
    
    operator bool()   const 
    {
        assert(((my_in != NULL) && (my_problemPos == NULL)) || (my_in == NULL));
        return my_in != NULL;
    }
    bool operator !() const 
    {
        assert(((my_in != NULL) && (my_problemPos == NULL)) || (my_in == NULL));
        return !my_in;
    }
    /// Current position of parser in format string such that fmt[pos] is 
    /// the next fmt char that will be read; returns -1 if at end.
    int pos() const {return my_next ? int(my_next - my_fmt.c_str()) : -1;}
    
    
    template <typename TT>
    fmtr_in& operator>>(TT& obj) {return input(obj);}
    template <typename TT>
    fmtr_in& operator&(TT& obj)  {return input(obj);}
    
    // overloads for iof::skip_past and fmt_spec since those are typically given as 
    // temporaries, so they have to be const so non-const ref not possible
    fmtr_in& operator>>(skip_past ign) {return input(ign);}
    fmtr_in& operator& (skip_past ign) {return input(ign);}
    fmtr_in& operator>>(fmt_spec spec) {return input(spec);}
    fmtr_in& operator& (fmt_spec spec) {return input(spec);}
    
    // manipulators (endl, etc)
    typedef std::istream&  (* imanip_t)     (std::istream&);
    typedef std::ios&      (* is_manip_t)   (std::ios&);
    typedef std::ios_base& (* ibase_manip_t)(std::ios_base&);
    
    inline fmtr_in&
    operator>>( imanip_t pf )     {return input(pf);}
    inline fmtr_in&
    operator& ( imanip_t pf )     {return input(pf);}
    
    inline fmtr_in&
    operator>>( is_manip_t pf )   {return input(pf);}
    inline fmtr_in&
    operator& ( is_manip_t pf )   {return input(pf);}
      
    inline fmtr_in&
    operator>>( ibase_manip_t pf) {return input(pf);}
    inline fmtr_in&
    operator& ( ibase_manip_t pf) {return input(pf);}
    
private:
    /// return true if "in formatting mode", ie if format 
    /// string hasn't been used up yet
    bool formatting() const {return my_next != NULL;}
    
    /// Parse the format spec and activate it. Returns true only if succesful,
    /// which implies the format spec was properly closed (not end of string)
    void activateFmtSpec(bool& restoreFmt, char& delim)
    {
        // parse format spec and set it
        try {
            const int oldWidthTmp = my_in.width(); // needed so can check if formatting changes width
            assert(delim != defaultDelim);
            my_next = process1FmtSpec(my_in, my_next, restoreFmt, delim);
            // if delim not changed, use the dynamic fmt_spec one (if any)
            if (delim == 0) delim = my_delimDynFmt;
            // set width to old if not set by \a next
            if (my_useSavedWidth && oldWidthTmp == my_in.width()) my_in.width(my_width); 
            
            // need to save width if won't be restoring format, 
            // since >> sets width to 0 after op
            if (!restoreFmt) my_width = my_in.width();
            my_useSavedWidth = true;
        }
        catch (const iof_private::failure& fail)
        {
            assert(fail.errType == failure::MARKER_NOT_CLOSED);
            assert(my_next != NULL);
            assert(my_fmt.size() > 0);
            assert(my_next > my_fmt.c_str() && my_next <= (my_fmt.c_str() + my_fmt.size()));
            throw marker_not_closed_fmt(fail, my_fmt);
        }
    }
    
    
    void deactivateFmtSpec(bool restoreFmt, char newDelim)
    {
        // ok, all was fine so handle format restoration
        if (restoreFmt)
        {
            //doRestoreFmt(in, saveFlags, oldWidth);
            my_in.flags(my_saveFlags);
            my_in.width(my_width); 
            my_in.fill(my_fillChar);
            my_delimDynFmt = 0;
        }
        else // save new formatting so it will be used for next input's restore
        {
            my_saveFlags = my_in.flags();
            if (my_in.width() != 0)  // then obj is a fmt_spec object
                my_width = my_in.width();
            my_fillChar = my_in.fill();
            my_delim = newDelim;
        }
        
    }

        
    template <typename TT>
    fmtr_in& input(TT& obj)
    {
        if (my_in.fail()) 
            return *this;
            
        try {
            my_throwing = false;
            const bool formatting_ = formatting();
            // parse format spec and set it
            if (formatting_) 
            {
                bool restoreFmt = true;
                char delim = 0;
                activateFmtSpec(restoreFmt, delim);
                
                const char nowDelim = (delim == 0 ? my_delim : delim);
                assert(nowDelim != 0);
                assert(my_in);
                doScan(my_in, obj, nowDelim);
                
                deactivateFmtSpec(restoreFmt, nowDelim);
            }
            else my_in >> obj;
            
            if (my_in.fail()) 
            {
                if (my_next) my_problemPos = my_next;
                else my_problemPos = my_fmt.c_str() + my_fmt.size();
                assert(my_problemPos > my_fmt.c_str());
                if (formatting_) my_problemPos --; // so point to closing char of format marker
            }
            else if (my_next) 
                // output till next marker (or end of string)
                processToNextFmtMarker(); 
                
            return *this;
        }
        catch (...) 
        {
            my_throwing = true;
            throw;
        }
    }


    /** We don't consume a %s when inputting format, we just change current format 
        state as though it was part of current %s. Doesn't throw because
        process1FmtSpec(..., false) doesn't throw anything 
        */
    fmtr_in& input(fmt_spec obj)
    {
        if (!my_in.fail())
        {
            bool restore; // not used
            iof_private::process1FmtSpec(my_in, obj.str().c_str(), restore, my_delimDynFmt, false);
            my_useSavedWidth = false;
        }
        return *this;
    }
    
    
    /// Input to manipulators doesn't consume a format marker either
    fmtr_in& input( imanip_t pf )      {my_in >> pf; return *this;}
    fmtr_in& input( is_manip_t pf )    {my_in >> pf; return *this;}
    fmtr_in& input( ibase_manip_t pf ) {my_in >> pf; return *this;}
    
    
    /// Dump validity state of stream into \a obj
    fmtr_in& input(validity& obj)
    {
        obj.set(my_problemPos, my_fmt);
        return *this;
    }

    
    /** Process format string to next format marker, which includes
        processing skip markers.  
        \exception marker_not_closed_skip if a skip marker not closed
        */
    void processToNextFmtMarker()
    {
        // try to find next format marker
        try {
            my_next = iof_private::processToNextFmtMarker(my_in, my_next);
        }
        catch (const iof_private::failure& fail)
        {
            if (fail.errType == failure::MARKER_NOT_CLOSED)
                throw marker_not_closed_skip(fail, my_fmt);
            my_problemPos = fail.problem;
        }
    }
        
        
    /// same principle as fmtr_out::outRemaining
    bool checkFinalState()
    {
        #ifndef EXTRA_MARKERS_NO_ASSERT
        assert(my_next == NULL);
        #endif
        #ifndef EXTRA_MARKERS_NO_THROW
        if (my_next) 
            throw too_many_markers(pos(), my_fmt.c_str());
        #endif
        
        return my_in != NULL;
    }


private:
    std::istream& my_in;       ///< stream we're using
    stream_fmt my_saveFmt;     ///< format of stream, to restore upon destruction

    const std::string my_fmt;  ///< format string for objects to read from stream
    const char* my_next;       ///< at what pos in my_fmt are we
    std::ios::fmtflags my_saveFlags;
    int my_width;
    char my_fillChar;          ///< which char to consume before input is attempted
    char my_delim;             ///< up to which char to read, if input into string
    char my_delimDynFmt;       ///< used ofr dynamic format specs
    bool my_useSavedWidth;     ///< used for dynamic format specs
    
    bool my_throwing;          ///< used to protect from destructor throw
    const char* my_problemPos; ///< if there is an I/O problem, at what pos in fmt
};


} // namespace iof_private


} // namespace iof


#endif // IOF_FMTR_IN_HPP_
