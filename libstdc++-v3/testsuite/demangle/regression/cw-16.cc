// 2003-11-07  Carlo Wood  <carlo@alinoe.com>

// Copyright (C) 2003 Free Software Foundation, Inc.
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

// IA 64 C++ ABI - 5.1 External Names (a.k.a. Mangling)

#include <testsuite_hooks.h>

// Torturing by Carlo Wood.
int main()
{
  using namespace __gnu_test;

// 2003/11/07, libstdc++/12736
verify_demangle("_Z3fooIA6_KiEvA9_KT_rVPrS4_",
		"void foo<int const [6]>(int const [9][6], int const restrict (* volatile restrict) [9][6])");
// 2003/11/12, libstdc++/12947
verify_demangle("_Z1fILi5E1AEvN1CIXqugtT_Li0ELi1ELi2EEE1qE",
                "void f<5, A>(C<((5) > (0)) ? (1) : (2)>::q)");
verify_demangle("_Z1fILi5EEvN1AIXcvimlT_Li22EEE1qE",
                "void f<5>(A<(int)((5) * (22))>::q)");
verify_demangle("_Z1fPFYPFiiEiE",
                "f(int (*)(int) (*) [extern \"C\"] (int))");
verify_demangle("_Z1fI1XENT_1tES2_",
                "X::t f<X>(X::t)");
verify_demangle("_Z1fILi5E1AEvN1CIXstN1T1tEEXszsrS2_1tEE1qE",
                "void f<5, A>(C<sizeof (T::t), sizeof (T::t)>::q)");

  return 0;
}