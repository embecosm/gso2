#ifndef IOF_TOSTR_H
#define IOF_TOSTR_H

/** \file
    Provides formatted output to a string via std::ostringstream. 
    
    \verbatim
    **  Copyright (c) 2006, Oliver Schoenborn
    **  Distributed under the BSD Software License (see accompanying 
    **  LICENSE_IOF_1_0.txt in root folder of library for complete 
    **  terms and conditions). 
    \endverbatim
*/


#include "iof/prints.hpp"

namespace iof 
{

#define GET_STR_FROM_PRINT(printsFmtObj) \
    std::ostringstream out; \
    printsFmtObj;           \
    return out.str();
    

/// Returns a std::string of \a str
inline
std::string tostr(const char* msg)
{
    return msg;
}


/// Returns a copy of \a str
inline
std::string tostr(const std::string& msg)
{
    return msg;
}


/// Note that for this overload, str=fmt_spec is allowed but doesn't make sense
template <typename TT>
inline
std::string tostr(const TT& msg)
{
    GET_STR_FROM_PRINT( prints(out, msg) );
}


template <typename T1>
inline
std::string tostr(const char* fmt, const T1& obj)
{
    GET_STR_FROM_PRINT( prints(out, fmt, obj) );
}


template <typename T1, typename T2>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2)
{
    GET_STR_FROM_PRINT( prints(out, fmt, obj1, obj2) );
}


template <typename T1, typename T2, typename T3>
inline
std::string tostr(const char* fmt,
const T1& obj1, const T2& obj2, const T3& obj3)
{
    GET_STR_FROM_PRINT( prints(out, fmt, obj1, obj2, obj3) );
}


template <typename T1, typename T2, typename T3, typename T4>
inline
std::string tostr(const char* fmt,
const T1& obj1, const T2& obj2, const T3& obj3, const T4& obj4)
{
    GET_STR_FROM_PRINT( prints(out, fmt, obj1, obj2, obj3, obj4) );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5)
{
    GET_STR_FROM_PRINT( prints(out, fmt, obj1, obj2, obj3, obj4, obj5) );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6)
{
    GET_STR_FROM_PRINT( 
        prints(out, fmt, obj1, obj2, obj3, obj4, obj5, obj6)  );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7)
{
    GET_STR_FROM_PRINT(
        prints(out, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7) );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8)
{
    GET_STR_FROM_PRINT( 
        prints(out, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8) );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8, typename T9>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8, const T9& obj9)
{
    GET_STR_FROM_PRINT(
        prints(out, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9) );
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8, typename T9, typename T10>
inline
std::string tostr(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8, const T9& obj9, const T10& obj10)
{
    GET_STR_FROM_PRINT(
        prints(out, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9, obj10) );
}

} // namespace iof

#endif // IOF_TOSTR_H

