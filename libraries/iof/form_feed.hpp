#ifndef IOF_FORM_FEED_HPP_
#define IOF_FORM_FEED_HPP_


/** \file
    Provides a character constant that can be used when you need to send 
    a form-feed to a stream. It is only useful for clarity since the
    iof::fmtr uses \f to do a stream flush. 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


namespace iof 
{

/** Pipe this in output formating if want \f effect (form feed), 
    since iof::fmtr uses \f to do a stream flush:
    
    cout << iof::fmtr("blabla\f") << '\f' << iof::form_feed;
    
    In the above, the \f in format string actually does a stream 
    flush. Only the \f sent to the stream after the fmtr, and the 
    form_feed, both cause a form-feed char to be sent to the stream.
*/
static const char form_feed = '\f';

}


#endif // IOF_FORM_FEED_HPP_
