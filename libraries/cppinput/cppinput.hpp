// CPPInput library header file
// Copyright (c) James Pallister, All rights reserved.

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 3.0 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library.

#ifndef __CPPINPUT_HPP__
#define __CPPINPUT_HPP__

#include <iostream>
#include <string>
#include <sstream>

#include <assert.h>

namespace cppinput
{

namespace ci_private
{
    // Split the format string into a test string before the specifier, the
    // specifier, and the remainder of the string.
    inline bool nextSpecifier(std::string fmt, std::string &pre_match,
        std::string &specifier, std::string &post_match)
    {
        unsigned i = 0, j = 0;

        bool found_start = false;
        bool found_end = false;

        for(; i < fmt.size(); ++i)
        {
            if(fmt[i] == '{')
            {
                if(i < fmt.size()-1)
                {
                    if(fmt[i+1] != '{')
                    {
                        found_start = true;
                        break;
                    }
                }
                else
                {
                    // TODO: raise proper exception
                    assert(!"Invalid format string. TODO - raise proper exception");
                }
            }
        }

        if(!found_start)
        {
            pre_match = fmt;
            return false;
        }

        pre_match = fmt.substr(0, i);

        for(j = i; j < fmt.size(); ++j)
        {
            if(fmt[j] == '}')
            {
                found_end = true;
                break;
            }
        }

        if(!found_end)
        {
            // TODO: raise proper exception
            assert(!"Invalid format string. TODO - raise proper exception");
        }

        specifier = fmt.substr(i, j-i+1);
        post_match = fmt.substr(j+1);

        return true;
    }
}; // namespace private

inline bool input(std::istream &in, std::string fmt)
{
    std::string pre_match, specifier, post_match;
    bool have_specifier = ci_private::nextSpecifier(fmt, pre_match, specifier, post_match);

    if(have_specifier)
    {
        // Error, we have no types left!
        return false;
    }

    for(int i = 0; i < fmt.size(); ++i)
    {
        char c;

        in.get(c);
        if(c != fmt[i])
        {
            in.setstate(std::ios::failbit);
            return false;
        }
    }

    return true;
}

// Input from the given stream, as specified by the format string fmt, storing
// the values in the params provided.
template<typename V, typename... Values>
bool input(std::istream &in, std::string fmt, V &value, Values &... params)
{
    std::string pre_match, specifier, post_match;
    bool have_specifier;

    have_specifier = ci_private::nextSpecifier(fmt, pre_match, specifier, post_match);

    // First, match prematch
    for(int i = 0; i < pre_match.size(); ++i)
    {
        char c;

        // in >> c;
        in.get(c);
        if(c != pre_match[i])
        {
            in.setstate(std::ios::failbit);
            return false;
        }
    }

    if(have_specifier)
    {
        // Then, input as per the specifier
        in >> value;

        // Finally, recurse with the rest of the string
        return input(in, post_match, params...);
    }

    return true;
}

template<typename V, typename... Values>
bool input(std::istream &in, std::string fmt)
{
    // Just match fmt
    for(int i = 0; i < fmt.size(); ++i)
    {
        char c;

        in.get(c);
        if(c != fmt[i])
        {
            in.setstate(std::ios::failbit);
            return false;
        }
    }
}

template<typename... Values>
bool input(std::string &in, std::string fmt, Values &... params)
{
    std::stringstream ss(in);
    return input(ss, fmt, params...);
}


}; // namespace cppinput

#endif
