#ifndef IOF_FMTR_OUT_HPP_
#define IOF_FMTR_OUT_HPP_

/** \file
 	Provides private functions to output stuff via the fmtr_out class.
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/

#include "iof/except.hpp"
#include "iof/fmt_spec.hpp"
#include <sstream>


namespace iof 
{


namespace iof_private
{


class fmtr_out
{
private:
    /** Order of calls should always be active(), activateFmtSpec() 
        (optional), and finally deactivate().
        */
    class fmt_settings
    {
    public:
        fmt_settings(const std::ostream& out):
            my_saveFlags(out.flags()), 
            my_fill(out.fill()), 
            my_prec(out.precision()), 
            my_width(out.width()), 
            my_useSavedWidth(true) {}

        fmt_settings(const fmt_settings& rhs):
            my_saveFlags(rhs.my_saveFlags), 
            my_fill(rhs.my_fill),
            my_prec(rhs.my_prec), 
            my_width(rhs.my_width), 
            my_useSavedWidth(true),
            my_fmtExtensions(rhs.my_fmtExtensions) {}
            
        bool operator==(const fmt_settings& rhs) const
        {
            const bool eq = 
                   my_saveFlags     == rhs.my_saveFlags
                && my_fill          == rhs.my_fill
                && my_prec          == rhs.my_prec
                && my_width         == rhs.my_width
                && my_useSavedWidth == rhs.my_useSavedWidth
                && my_fmtExtensions == rhs.my_fmtExtensions;
            return eq;
        }

        inline void
        activate(std::ostream& out, const char*& next, const char* dynFmt)
        {
            if (my_useSavedWidth) 
                out.width(my_width); 
            my_fmtExtensions.save();
            //my_oldNeedCenter = my_needCentering;
            my_restoreFmt = true;
            next = process1FmtSpec(out, next, my_fmtExtensions, my_restoreFmt);
            if (dynFmt) 
                process1FmtSpec(out, dynFmt, my_fmtExtensions, my_restoreFmt, false);
                
            if (!my_restoreFmt) 
                my_width = out.width();
            my_useSavedWidth = true;
        }

        /** Activates the settings in \a spec, ie the format specification 
            in \a spec instead of the that in *this. This spec will override 
            the settings from most recent activate().
            */
        inline void 
        activateFmtSpec(std::ostream& out, const std::string& spec)
        {
            bool restore; // not used in fmt spec
            iof_private::process1FmtSpec(out, spec.c_str(), my_fmtExtensions, restore, false);
            my_useSavedWidth = false;
        }

        inline void
        deactivate(std::ostream& out)
        {
            if (my_restoreFmt)
            // restore flag state of stream
            {
                out.flags(my_saveFlags);
                out.precision(my_prec);
                out.fill(my_fill);
                my_fmtExtensions.restore();
                //my_needCentering = my_oldNeedCenter;
            }
            else // don't restore, commit new values instead
            {
                my_saveFlags = out.flags();
                my_prec      = out.precision();
                my_fill      = out.fill();
            }
            // stream cancels width after output, so it should always be zero;
            // could only be fail if somehow obj is a fmt_spec, which is not allowed:
            assert(out.width() == 0); 
        }
        
        const fmt_extensions& extensions() const {return my_fmtExtensions;}
    
    private:
        std::ios::fmtflags my_saveFlags;
        char my_fill;
        int  my_prec;
        int  my_width;
        bool my_useSavedWidth;
        fmt_extensions my_fmtExtensions;
        
        bool my_restoreFmt;
    };
            
public:
    /** Formatter for stream \a out, using format string \a fmt. If 
        \a needCleanDestruct is true (default), then destructor will do extra 
        cleanup checks and may throw an exception. 
        */
    fmtr_out(std::ostream& out, const std::string& fmt, bool needCleanDestruct = true)
        : my_out(out), 
          my_saveFmt(out),
          my_fmt(fmt), 
          my_next(NULL), 
          my_persistFmt(my_out),
          my_validateDestruct(needCleanDestruct), 
          my_throwing(false)
    {
        if (!my_out.fail())
        {
            my_next = my_fmt.c_str();
            assert(my_next);
            my_next = iof_private::processToNextFmtMarker(my_out, my_next);
        }
    }

    /// used for stringizer
    fmtr_out(std::ostream& out, const fmtr_out& fmtr)
        : my_out(out), 
          my_saveFmt(out),
          my_fmt(fmtr.my_fmt), 
          my_next(my_fmt.c_str() + fmtr.pos()), 
          my_persistFmt(fmtr.my_persistFmt),
          my_validateDestruct(fmtr.my_validateDestruct), 
          my_throwing(false)
    {
    }

    /** May throw if needCleanDestruct was true at construction time, and 
        certain checks fail (see compile time #defines, EXTRA_MARKERS_NO_ASSERT
        and EXTRA_MARKERS_NO_THROW). Note that it is fine for destructor 
        to throw since fmtr_out is always a temporary.
        */
    ~fmtr_out()
    {
        if (!my_throwing && my_validateDestruct) outRemaining();
        my_saveFmt.applyTo(my_out);
    }
    
    /// Return the format string given at construction
    std::string fmt() const {return my_fmt;}
    operator bool()   const {return my_out != NULL;}
    bool operator !() const {return !my_out;}
    /// Current position of parser in format string such that fmt[pos] is 
    /// the next fmt char that will be read; returns -1 if at end.
    int pos() const {return my_next ? int(my_next - my_fmt.c_str()) : -1;}


    bool operator==(const fmtr_out& rhs) const 
    {
        bool result = (    my_saveFmt   == rhs.my_saveFmt
                        && my_fmt       == rhs.my_fmt
                        && my_persistFmt == rhs.my_persistFmt
                        && my_validateDestruct == rhs.my_validateDestruct);
        if (my_next)
        {
            result &= ( pos() == rhs.pos() );
        }
        else
            result &= !rhs.my_next;
        return result;
    }
    
    
    /// This can be called if fmtr_out obj is not destroyed when output 
    /// completed; this is the case of stringizer
    void throwIfRemaining() 
    {
        //my_validateDestruct = true;
        try {
            my_throwing = false;  
            outRemaining();
        }
        catch (...) {
            my_throwing = true;  
            throw;
        }
    }
    
    
    template <typename TT>
    fmtr_out& operator<<(const TT& obj) {return output(obj);}
    template <typename TT>
    fmtr_out& operator&(const TT& obj)  {return output(obj);}
    
    // manipulators (endl, etc)
    typedef std::ostream&  (* omanip_t)     (std::ostream&);
    typedef std::ios&      (* os_manip_t)   (std::ios&);
    typedef std::ios_base& (* obase_manip_t)(std::ios_base&);
    
    inline fmtr_out&
    operator<<( omanip_t pf )     {return output(pf);}
    inline fmtr_out&
    operator& ( omanip_t pf )     {return output(pf);}
    
    inline fmtr_out&
    operator<<( os_manip_t pf )   {return output(pf);}
    inline fmtr_out&
    operator& ( os_manip_t pf )   {return output(pf);}
      
    inline fmtr_out&
    operator<<( obase_manip_t pf) {return output(pf);}
    inline fmtr_out&
    operator& ( obase_manip_t pf) {return output(pf);}
    
    
private:
    /// return true if "in formatting mode", ie if format 
    /// string hasn't been used up yet
    bool formatting() const {return my_next != NULL;}
    
    
    /** Output an object of type TT. This reads the format string 
        at current position and interprets format specification, 
        then outputs \a obj, then reads remaining characters until
        next format marker encountered (or end of string). 
        */
    template <typename TT>
    fmtr_out& output(const TT& obj, const char* dynFmt = NULL)
    {
        if (my_out.fail())
            return *this;
            
        try {
            my_throwing = false;
            if (formatting())
            { 
                my_persistFmt.activate(my_out, my_next, dynFmt);
                outputAdvanced(obj, my_out, my_persistFmt.extensions());
                my_persistFmt.deactivate(my_out);    
    
                // send out the next sequence of characters (if any), 
                // up to next format marker (if any)
                if (my_next) 
                    my_next = processToNextFmtMarker(my_out, my_next);
            }
            else // consumed format string, so not formatting
                // never need centering if not formatting
                my_out << obj; //outputAdvanced(obj, my_out, false); 
        }
        catch (const iof_private::failure& fail)
        {
            assert(fail.errType == failure::MARKER_NOT_CLOSED);
            assert(my_next != NULL);
            assert(my_fmt.size() > 0);
            assert(my_next > my_fmt.c_str() && my_next <= (my_fmt.c_str() + my_fmt.size()));
            my_throwing = true;  
            throw marker_not_closed_fmt(fail, my_fmt);
        }
        catch (...) {
            my_throwing = true;  
            throw;
        }
        return *this;
    }
    
    
    /** We want special output for fmt_spec: don't consume a %s marker!
        It just applies format and tells next output op to not override width.
        Note that if no longer in formatting mode, \a obj is output directly
        to stream (so e.g. centering is not supported). 
        */
    fmtr_out& output(const fmt_spec& obj)
    {
        if (!my_out.fail())
        {
            if (formatting()) // in formatting mode
                my_persistFmt.activateFmtSpec(my_out, obj.str());
            else 
                my_out << obj;
        }
        return *this;
    }
        

    template <typename TT>
    fmtr_out& output(const formatted<TT>& obj)
    {
        return output(obj.obj, obj.fmt.c_str());
    }
    
    /// Outputting manipulators does not consume a format marker either
    fmtr_out& output( omanip_t pf )      {my_out << pf; return *this;}
    fmtr_out& output( os_manip_t pf )    {my_out << pf; return *this;}
    fmtr_out& output( obase_manip_t pf ) {my_out << pf; return *this;}
    
    
    /** Handle remaining format markers. These are markers for which 
        there is no object. Used to be that they were just output.
        However this would typically indicate a logic or runtime 
        error, so it is best to assert or throw an exception. The \a fmt
        is the full format string, where \a next is the next char 
        within that string. 
        */
    inline void outRemaining()
    {
        #ifndef EXTRA_MARKERS_NO_ASSERT
        assert(!formatting());
        #endif
        #ifndef EXTRA_MARKERS_NO_THROW
        if (formatting()) throw too_many_markers(pos(), my_fmt.c_str());
        #endif
    }
    
    
private:
    std::ostream& my_out;
    stream_fmt my_saveFmt;
    
    const std::string my_fmt;
    const char* my_next; ///< points into my_fmt
    fmt_settings my_persistFmt;
        
    bool my_validateDestruct;
    bool my_throwing; 
};


} // namespace iof_private


} // namespace iof




#endif //  IOF_FMTR_OUT_HPP_
