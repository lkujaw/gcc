------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                              S Y M B O L S                               --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 2003 Free Software Foundation, Inc.               --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNAT;  see file COPYING.  If not, write --
-- to  the Free Software Foundation,  59 Temple Place - Suite 330,  Boston, --
-- MA 02111-1307, USA.                                                      --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This package allows the creation of symbol files to be used for linking
--  libraries. The format of symbol files depends on the platform, so there is
--  several implementations of the body.

with GNAT.Dynamic_Tables;
with GNAT.OS_Lib;         use GNAT.OS_Lib;

package Symbols is

   type Symbol_Kind is (Data, Proc);
   --  To distinguish between the different kinds of symbols

   type Symbol_Data is record
      Name    : String_Access;
      Kind    : Symbol_Kind := Data;
      Present : Boolean := True;
   end record;
   --  Data (name and kind) for each of the symbols

   package Symbol_Table is new GNAT.Dynamic_Tables
     (Table_Component_Type => Symbol_Data,
      Table_Index_Type     => Natural,
      Table_Low_Bound      => 0,
      Table_Initial        => 100,
      Table_Increment      => 100);
   --  The symbol tables

   Original_Symbols : Symbol_Table.Instance;
   --  The symbols, if any, found in the original symbol table

   Complete_Symbols : Symbol_Table.Instance;
   --  The symbols, if any, found in the objects files

   procedure Initialize
     (Symbol_File : String;
      Force       : Boolean;
      Quiet       : Boolean;
      Success     : out Boolean);
   --  Initialize a symbol file. This procedure must be called before
   --  Processing any object file. Depending on the platforms and the
   --  circumstances, additional messages may be issued if Quiet is False.

   procedure Process
     (Object_File : String;
      Success     : out Boolean);
   --  Get the symbols from an object file. Success is set to True if the
   --  object file exists and has the expected format.

   procedure Finalize
     (Quiet   : Boolean;
      Success : out Boolean);
   --  Finalize the symbol file. This procedure should be called after
   --  Initialize (once) and Process (one or more times). If Success is
   --  True, the symbol file is written and closed, ready to be used for
   --  linking the library. Depending on the platforms and the circumstances,
   --  additional messages may be issued if Quiet is False.

end Symbols;