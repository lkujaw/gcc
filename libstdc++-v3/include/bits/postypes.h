// Position types -*- C++ -*-

// Copyright (C) 1997, 1998, 1999, 2000, 2001, 2003 
// Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License along
// with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307,
// USA.

// As a special exception, you may use this file as part of a free software
// library without restriction.  Specifically, if other files instantiate
// templates or use macros or inline functions from this file, or you compile
// this file and link it with other files to produce an executable, this
// file does not by itself cause the resulting executable to be covered by
// the GNU General Public License.  This exception does not however
// invalidate any other reasons why the executable file might be covered by
// the GNU General Public License.

//
// ISO C++ 14882: 27.4.1 - Types
// ISO C++ 14882: 27.4.3 - Template class fpos
//

/** @file postypes.h
 *  This is an internal header file, included by other library headers.
 *  You should not attempt to use it directly.
 */

#ifndef _GLIBCXX_POSTYPES_H
#define _GLIBCXX_POSTYPES_H 1

#pragma GCC system_header

#include <cwchar> // For mbstate_t

namespace std
{
  // The types streamoff, streampos and wstreampos and the class
  // template fpos<> are described in clauses 21.1.2, 21.1.3, 27.1.2,
  // 27.2, 27.4.1, 27.4.3 and D.6. Despite all this verbage, the
  // behaviour of these types is mostly implementation defined or
  // unspecified. The behaviour in this implementation is as noted
  // below.  
  typedef long  	__streamoff_base_type;
  typedef ptrdiff_t	streamsize; // Signed integral type

  template<typename _StateT>
    class fpos;

  // Class streamoff is an implementation defined type that meets the
  // requirements for streamoff. It stores an offset as a signed
  // integer.  Note: this class is an implementation detail.
  class streamoff
  {
  private:
    __streamoff_base_type _M_off;

  public:
    // Nothing in the standard requires that streamoff can be default
    // constructed. In this implementation a default constructor that
    // stores the value 0 is provided.
    streamoff()
    : _M_off(0) { }

    // The standard only requires that streamoff can be constructed
    // from streamsize using the constructor syntax. This
    // implementation also allows implicit conversion from integer
    // types to streamoff.
    streamoff(__streamoff_base_type __off)
    : _M_off(__off) { }

    // The standard requires that streamoff can be constructed from
    // instances of fpos using the constructor syntax, but gives no
    // semantics for this construction. In this implementation it
    // extracts the offset stored by the fpos object.
    // Note: In versions of GCC up to and including GCC 3.3, implicit
    // conversion from fpos to streamoff was allowed. This constructor
    // has now been made explicit to improve type safety.
    template<typename _StateT>
      explicit
      streamoff(const fpos<_StateT>&);

    // The standard requires that streamsize can be constructed from
    // streamoff using the constructor syntax. This implementation
    // also allows implicit conversion. This allows streamoff objects
    // to be used in arithmetic expressions and to be compared against
    // each other and integer types.
    operator __streamoff_base_type() const
    { return _M_off; }

    // This implementation allows the use of operators +=, -=, ++ and
    // -- on streamoff objects.
    streamoff&
    operator+=(__streamoff_base_type __off)
    {
      _M_off += __off;
      return *this;
    }

    streamoff&
    operator-=(__streamoff_base_type __off)
    {
      _M_off -= __off;
      return *this;
    }
  };

  // In clauses 21.1.3.1 and 27.4.1 streamoff is described as an
  // implementation defined type. In this implementation it is a
  // distinct class type.
  // Note: In versions of GCC up to and including GCC 3.3, streamoff
  // was typedef long.
  typedef class streamoff streamoff;

  // The standard fails to place any requiremens on the template
  // argument StateT. In this implementation StateT must be
  // DefaultConstructible, CopyConstructible and Assignable.  The
  // standard only requires that fpos should contain a member of type
  // StateT. In this implementation it also contains an offset stored
  // as a signed integer.
  template<typename _StateT>
    class fpos
    {
    private:
      friend class streamoff;

      __streamoff_base_type _M_off;
      _StateT _M_state;

    public:
      // The standard doesn't require that fpos objects can be default
      // constructed. This implementation provides a default
      // constructor that initializes the offset to 0 and default
      // constructs the state.
      fpos()
      : _M_off(0), _M_state() { }

      // The standard requires implicit conversion from integers to
      // fpos, but gives no meaningful semantics for this
      // conversion. In this implementation this constructor stores
      // the integer as the offset and default constructs the state.
      fpos(__streamoff_base_type __off)
      : _M_off(__off), _M_state() { }

      // The standard requires that fpos objects can be constructed
      // from streamoff objects using the constructor syntax, and
      // fails to give any meaningful semantics. In this
      // implementation implicit conversion is also allowed, and this
      // constructor stores the streamoff as the offset and default
      // constructs the state.
      fpos(const streamoff& __off)
      : _M_off(__off), _M_state() { }

      void
      state(_StateT __st)
      { _M_state = __st; }

      _StateT
      state() const
      { return _M_state; }

      // The standard only requires that operator== must be an
      // equivalence relation. In this implementation two fpos<StateT>
      // objects belong to the same equivalence class if the contained
      // offsets compare equal.
      bool
      operator==(const fpos& __other) const
      { return _M_off == __other._M_off; }

      bool
      operator!=(const fpos& __other) const
      { return _M_off != __other._M_off; }

      // The standard requires that this operator must be defined, but
      // gives no semantics. In this implemenation it just adds it's
      // argument to the stored offset and returns *this.
      fpos&
      operator+=(const streamoff& __off)
      {
	_M_off += __off;
	return *this;
      }

      // The standard requires that this operator must be defined, but
      // gives no semantics. In this implemenation it just subtracts
      // it's argument from the stored offset and returns *this.
      fpos&
      operator-=(const streamoff& __off)
      {
	_M_off -= __off;
	return *this;
      }

      // The standard requires that this operator must be defined, but
      // defines it's semantics only in terms of operator-. In this
      // implementation it constructs a copy of *this, adds the
      // argument to that copy using operator+= and then returns the
      // copy.
      fpos
      operator+(const streamoff& __off) const
      {
	fpos __pos(*this);
	__pos += __off;
	return __pos;
      }

      // The standard requires that this operator must be defined, but
      // defines it's semantics only in terms of operator+. In this
      // implementation it constructs a copy of *this, subtracts the
      // argument from that copy using operator-= and then returns the
      // copy.
      fpos
      operator-(const streamoff& __off) const
      {
	fpos __pos(*this);
	__pos -= __off;
	return __pos;
      }

      // The standard requires that this operator must be defined, but
      // defines it's semantics only in terms of operator+. In this
      // implementation it returns the difference between the offset
      // stored in *this and in the argument.
      streamoff
      operator-(const fpos& __other) const
      { return _M_off - __other._M_off; }
    };

  template<typename _StateT>
    inline
    streamoff::streamoff(const fpos<_StateT>& __pos)
    : _M_off(__pos._M_off) { }

  // Clauses 21.1.3.1 and 21.1.3.2 describe streampos and wstreampos
  // as implementation defined types, but clause 27.2 requires that
  // they must both be typedefs for fpos<mbstate_t>
  typedef fpos<mbstate_t> streampos;
  typedef fpos<mbstate_t> wstreampos;
} // namespace std

#endif