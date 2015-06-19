#ifndef IOF_PRINT_H
#define IOF_PRINT_H

/** \file
    Provides all the prints() function overloads for printsing to a stream, as
    well as the two iof-compatible manipulators that replace std::endl 
    and std::flush. 
    
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


namespace iof_private
{


} // namespace iof_private

// -------------------- prints template functions ---------------------


/// If just one obj to output, no formatting assumed
template <typename TT> 
inline
bool prints(std::ostream& out, const TT& obj)
{
    return out << obj;
}


template <typename T1>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj)
{
    return iof_private::fmtr_out(out, fmt) << obj; 
}


template <typename T1, typename T2>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2; 
}


template <typename T1, typename T2, typename T3>
inline
bool prints(std::ostream& out, const char* fmt,
    const T1& obj1, const T2& obj2, const T3& obj3)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3; 
}


template <typename T1, typename T2, typename T3, typename T4>
inline
bool prints(std::ostream& out, const char* fmt,
    const T1& obj1, const T2& obj2, const T3& obj3, const T4& obj4)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5, 
    const T6& obj6)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5 << obj6; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5, 
    const T6& obj6, const T7& obj7)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5 << obj6 << obj7; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5, 
    const T6& obj6, const T7& obj7, const T8& obj8)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5 << obj6 << obj7 << obj8; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5, 
    const T6& obj6, const T7& obj7, const T8& obj8, const T9& obj9)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5 << obj6 << obj7 << obj8 << obj9; 
}


template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, 
    typename T7, typename T8, typename T9, typename T10>
inline
bool prints(std::ostream& out, const char* fmt, const T1& obj1, 
    const T2& obj2, const T3& obj3, const T4& obj4, const T5& obj5, 
    const T6& obj6, const T7& obj7, const T8& obj8, const T9& obj9, const T10& obj10)
{
    return iof_private::fmtr_out(out, fmt) << obj1 << obj2 << obj3 << obj4 << obj5 << obj6 << obj7 << obj8 << obj9 << obj10; 
}


} // namespace iof

#endif // IOF_PRINT_H


