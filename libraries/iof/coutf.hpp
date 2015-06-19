#ifndef IOF_COUTF_H
#define IOF_COUTF_H


/** \file
    Provides formatted output to the std::cout stream. 
    
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

template <typename TT> 
inline
void coutf(const TT& msg)
{
    prints(std::cout, msg);
}


template <typename T1>
inline
void coutf(const char* fmt, const T1& obj)
{
    prints(std::cout, fmt, obj);
}


template <typename T1, typename T2>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2)
{
    prints(std::cout, fmt, obj1, obj2);
}


template <typename T1, typename T2, typename T3>
inline
void coutf(const char* fmt,
const T1& obj1, const T2& obj2, const T3& obj3)
{
    prints(std::cout, fmt, obj1, obj2, obj3);
}


template <typename T1, typename T2, typename T3, typename T4>
inline
void coutf(const char* fmt,
const T1& obj1, const T2& obj2, const T3& obj3, const T4& obj4)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5, obj6);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8, typename T9>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8, const T9& obj9)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9);
}


template <typename T1, typename T2, typename T3, typename T4, typename T5,
    typename T6, typename T7, typename T8, typename T9, typename T10>
inline
void coutf(const char* fmt, const T1& obj1, const T2& obj2,
const T3& obj3, const T4& obj4, const T5& obj5, const T6& obj6, const T7& obj7, 
const T8& obj8, const T9& obj9, const T10& obj10)
{
    prints(std::cout, fmt, obj1, obj2, obj3, obj4, obj5, obj6, obj7, obj8, obj9, obj10);
}

} // namespace iof

#endif // IOF_COUTF_H

