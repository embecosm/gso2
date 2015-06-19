#ifndef IOF_SCAN_H
#define IOF_SCAN_H

/** \file
    Provides all the scans() function overloads for scansning (reading) 
    from a stream. 
    
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


/// If just one obj to output, no formatting assumed
template <typename TT> 
inline
bool scans(std::istream& in, TT& obj)
{
    return in >> obj;
}


template <typename T1>
inline
bool scans(std::istream& in, const char* fmt, T1& obj)
{
    return iof_private::fmtr_in(in, fmt) >> obj;
}


template <typename T1, typename T2>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2;
}


template <typename T1, typename T2, typename T3>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3;
}


template <typename T1, typename T2, typename T3, typename T4>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, T5& obj5)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, T5& obj5, T6& obj6)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5 >> obj6;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, T5& obj5, T6& obj6, T7& obj7)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5 >> obj6 >> obj7;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, T5& obj5, T6& obj6, T7& obj7, T8& obj8)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5 >> obj6 >> obj7 >> obj8;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, T5& obj5, T6& obj6, T7& obj7, T8& obj8, T9& obj9)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5 >> obj6 >> obj7 
                                         >> obj8 >> obj9;
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, 
    typename T7, typename T8, typename T9, typename T10>
inline
bool scans(std::istream& in, const char* fmt, T1& obj1, T2& obj2, T3& obj3, T4& obj4, 
    T5& obj5, T6& obj6, T7& obj7, T8& obj8, T9& obj9, T10& obj10)
{
    return iof_private::fmtr_in(in, fmt) >> obj1 >> obj2 >> obj3 >> obj4 >> obj5 >> obj6 >> obj7 
                                         >> obj8 >> obj9 >> obj10;
}


} // namespace iof

#endif // IOF_SCAN_H

