------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                               O U T P U T                                --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 1992-2006, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 2,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License --
-- for  more details.  You should have  received  a copy of the GNU General --
-- Public License  distributed with GNAT;  see file COPYING.  If not, write --
-- to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, --
-- Boston, MA 02110-1301, USA.                                              --
--                                                                          --
--
--
--
--
--
--
--
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This package contains low level output routines used by the compiler
--  for writing error messages and informational output. It is also used
--  by the debug source file output routines (see Sprintf.Print_Eol).

with Hostparm; use Hostparm;
with Types;    use Types;

package Output is
   pragma Elaborate_Body;

   type Output_Proc is access procedure (S : String);
   --  This type is used for the Set_Special_Output procedure. If this
   --  procedure is called, then instead of lines being written to
   --  standard error or standard output, a call is made to the given
   --  procedure for each line, passing the line with an end of line
   --  character (which is a single ASCII.LF character, even in systems
   --  which normally use CR/LF or some other sequence for line end).

   -----------------
   -- Subprograms --
   -----------------

   procedure Set_Special_Output (P : Output_Proc);
   --  Sets subsequent output to call procedure P. If P is null, then
   --  the call cancels the effect of a previous call, reverting the
   --  output to standard error or standard output depending on the
   --  mode at the time of previous call. Any exception generated by
   --  by calls to P is simply propagated to the caller of the routine
   --  causing the write operation.

   procedure Cancel_Special_Output;
   --  Cancels the effect of a call to Set_Special_Output, if any.
   --  The output is then directed to standard error or standard output
   --  depending on the last call to Set_Standard_Error or Set_Standard_Output.
   --  It is never an error to call Cancel_Special_Output. It has the same
   --  effect as calling Set_Special_Output (null).

   procedure Set_Standard_Error;
   --  Sets subsequent output to appear on the standard error file (whatever
   --  that might mean for the host operating system, if anything) when
   --  no special output is in effect. When a special output is in effect,
   --  the output will appear on standard error only after special output
   --  has been cancelled.

   procedure Set_Standard_Output;
   --  Sets subsequent output to appear on the standard output file (whatever
   --  that might mean for the host operating system, if anything) when
   --  no special output is in effect. When a special output is in effect,
   --  the output will appear on standard output only after special output
   --  has been cancelled. Output to standard output is the default mode
   --  before any call to either of the Set procedures.

   procedure Write_Char (C : Character);
   --  Write one character to the standard output file. Note that the
   --  character should not be LF or CR (use Write_Eol for end of line)

   procedure Write_Erase_Char (C : Character);
   --  If last character in buffer matches C, erase it, otherwise no effect

   procedure Write_Eol;
   --  Write an end of line (whatever is required by the system in use,
   --  e.g. CR/LF for DOS, or LF for Unix) to the standard output file.
   --  This routine also empties the line buffer, actually writing it
   --  to the file. Note that Write_Eol is the only routine that causes
   --  any actual output to be written.

   procedure Write_Int (Val : Int);
   --  Write an integer value with no leading blanks or zeroes. Negative
   --  values are preceded by a minus sign).

   procedure Write_Spaces (N : Nat);
   --  Write N spaces

   procedure Write_Str (S : String);
   --  Write a string of characters to the standard output file. Note that
   --  end of line is normally handled separately using WRITE_EOL, but it
   --  is allowed for the string to contain LF (but not CR) characters,
   --  which are properly interpreted as end of line characters. The string
   --  may also contain horizontal tab characters.

   procedure Write_Line (S : String);
   --  Equivalent to Write_Str (S) followed by Write_Eol;

   function Column return Pos;
   pragma Inline (Column);
   --  Returns the number of the column about to be written (e.g. a value
   --  of 1 means the current line is empty).

   -------------------------
   -- Buffer Save/Restore --
   -------------------------

   --  This facility allows the current line buffer to be saved and restored

   type Saved_Output_Buffer is private;
   --  Type used for Save/Restore_Buffer

   Buffer_Max : constant := Hostparm.Max_Line_Length;
   --  Maximal size of a buffered output line

   function Save_Output_Buffer return Saved_Output_Buffer;
   --  Save current line buffer and reset line buffer to empty

   procedure Restore_Output_Buffer (S : Saved_Output_Buffer);
   --  Restore previously saved output buffer. The value in S is not affected
   --  so it is legtimate to restore a buffer more than once.

   --------------------------
   -- Debugging Procedures --
   --------------------------

   --  The following procedures are intended only for debugging purposes,
   --  for temporary insertion into the text in environments where a debugger
   --  is not available. They all have non-standard very short lower case
   --  names, precisely to make sure that they are only used for debugging!

   procedure w (C : Character);
   --  Dump quote, character, quote, followed by line return

   procedure w (S : String);
   --  Dump string followed by line return

   procedure w (V : Int);
   --  Dump integer followed by line return

   procedure w (B : Boolean);
   --  Dump Boolean followed by line return

   procedure w (L : String; C : Character);
   --  Dump contents of string followed by blank, quote, character, quote

   procedure w (L : String; S : String);
   --  Dump two strings separated by blanks, followed by line return

   procedure w (L : String; V : Int);
   --  Dump contents of string followed by blank, integer, line return

   procedure w (L : String; B : Boolean);
   --  Dump contents of string followed by blank, Boolean, line return

private
   --  Note: the following buffer and column position are maintained by the
   --  subprograms defined in this package, and cannot be directly modified or
   --  accessed by a client.

   Buffer : String (1 .. Buffer_Max + 1);
   for Buffer'Alignment use 4;
   --  Buffer used to build output line. We do line buffering because it
   --  is needed for the support of the debug-generated-code option (-gnatD).
   --  Historically it was first added because on VMS, line buffering is
   --  needed with certain file formats. So in any case line buffering must
   --  be retained for this purpose, even if other reasons disappear. Note
   --  any attempt to write more output to a line than can fit in the buffer
   --  will be silently ignored. The alignment clause improves the efficiency
   --  of the save/restore procedures.

   Next_Col : Positive range 1 .. Buffer'Length + 1 := 1;
   --  Column about to be written

   type Saved_Output_Buffer is record
      Buffer   : String (1 .. Buffer_Max + 1);
      Next_Col : Positive;
   end record;

end Output;
