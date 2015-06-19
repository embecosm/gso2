#ifndef IOF_STRINGIZER_HPP_
#define IOF_STRINGIZER_HPP_


/** \file
    Provides the stringizer class, which allows a more STL-like syntax for the 
    creation of a string from a format.  
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include "iof/fmtr.hpp"

namespace iof
{
	

/** Creates a string from a format string and objects. E.g.:
    
    std::string name = iof::stringizer("%s, %s") & first & last;
    
    This does the same as the tostr() function, but using same 
    syntax as iof::fmtr.
    */
class stringizer
{
public:
	stringizer(const std::string& fmt): my_fmtr(my_out, fmt, false) {}
	
    // note that std::ios_base::ate does not work with VC++, must use <<
	stringizer(const stringizer& rhs)
		: my_fmtr(my_out, rhs.my_fmtr) {my_out << rhs.my_out.str();}
	
	//~stringizer() {std::cout << "dest " << (void*)this << std::endl;}
	
	bool operator==(const stringizer& rhs) const 
	{
		return my_out.str() == rhs.my_out.str() && my_fmtr == rhs.my_fmtr;
	}
	bool operator!=(const stringizer& rhs) const {return !(*this == rhs);}
	
	template <typename TT>
	stringizer& operator<<(const TT& obj) {my_fmtr << obj; return *this;}
	
	template <typename TT>
	stringizer& operator&(const TT& obj)  {my_fmtr &  obj; return *this;}
	
	operator std::string() 
	{
		// check that all markers have been used up!
		my_fmtr.throwIfRemaining();
		return my_out.str();
	}

    std::string fmt() const {return my_fmtr.fmt();}
	
private:
	std::ostringstream my_out;
	iof_private::fmtr_out my_fmtr;
};


} // namespace iof


#endif // IOF_STRINGIZER_HPP_
