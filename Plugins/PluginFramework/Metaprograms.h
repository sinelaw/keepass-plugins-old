/* Plugin Framework for KeePass
Copyright 2009 Bill Rubin <rubin@contractor.net>
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE_1_0.txt or copy at
http://www.boost.org/LICENSE_1_0.txt) */
#pragma once

namespace Metaprogram {

// Causes compile error if instantiated with an declared but undefined class.
// The compile error will typically mention the name of the class.
template<class Compile_error_message> class IntentionalError
  {enum {VALUE = sizeof Compile_error_message};};


class Null {};  // Useful to allow "If" without "Else".  See following declaration.

// The following class template is a slightly-modified version of Thomas Becker's
// "IF" template described in C/C++ Users Journal, October 2002, pp. 38-44.

// If condition is true, then If<...>::Return is "Then"; else If<...>::Return is "Else":
template<bool condition, typename Then, typename Else = Null> struct If;
template<typename Then, typename Else> struct If< true, Then, Else> {typedef Then Return;};
template<typename Then, typename Else> struct If<false, Then, Else> {typedef Else Return;};


// If "condition" is true, emit compiler error mentioning declared but undefined class "Error".
// Otherwise, form an empty class.
template<bool condition, class Error> class ConditionalError
  : If<condition, IntentionalError<Error> >::Return {};


// value is true if A and B are the same type.  
// Uses VC++ 8.0 compiler support for type traits.
template<typename A, typename B> struct is_same {
    static const bool value = __is_base_of(A, B) 
                           && __is_base_of(B, A);
};

}