/****************************************************************************
 *                                                                          *
 *                            GNATMEM COMPONENTS                            *
 *                                                                          *
 *                                 G M E M                                  *
 *                                                                          *
 *                          C Implementation File                           *
 *                                                                          *
 *         Copyright (C) 2000-2003 Free Software Foundation, Inc.           *
 *                                                                          *
 * GNAT is free software;  you can  redistribute it  and/or modify it under *
 * terms of the  GNU General Public License as published  by the Free Soft- *
 * ware  Foundation;  either version 2,  or (at your option) any later ver- *
 * sion.  GNAT is distributed in the hope that it will be useful, but WITH- *
 * OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY *
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License *
 * for  more details.  You should have  received  a copy of the GNU General *
 * Public License  distributed with GNAT;  see file COPYING.  If not, write *
 * to  the  Free Software Foundation,  51  Franklin  Street,  Fifth  Floor, *
 * Boston, MA 02110-1301, USA.                                              *
 *                                                                          *
--
--
--
--
--
--
 * GNAT was originally developed  by the GNAT team at  New York University. *
 * Extensive contributions were provided by Ada Core Technologies Inc.      *
 *                                                                          *
 ****************************************************************************/

/*  This unit reads the allocation tracking log produced by augmented
    __gnat_malloc and __gnat_free procedures (see file a-raise.c) and
    provides GNATMEM tool with gdb-compliant output. The output is
    processed by GNATMEM to detect dynamic memory allocation errors.

    See GNATMEM section in GNAT User's Guide for more information.

    NOTE: This capability is currently supported on the following targets:

      DEC Unix
      GNU/Linux x86
      Solaris (sparc and x86) (*)
      Windows 98/95/NT (x86)

    (*) on these targets, the compilation must be done with -funwind-tables to
    be able to build the stack backtrace.
*/

#include <stdio.h>

static FILE *gmemfile;

/* tb_len is the number of call level supported by this module */
#define tb_len 200
static char * tracebk [tb_len];
static int cur_tb_len, cur_tb_pos;

#define LOG_EOF   '*'
#define LOG_ALLOC 'A'
#define LOG_DEALL 'D'

struct struct_storage_elmt {
  char   Elmt;
  void * Address;
  size_t Size;
};

extern void
convert_addresses (char *addrs[], int n_addr, void *buf, int *len);

/* reads backtrace information from gmemfile placing them in tracebk
   array. cur_tb_len is the size of this array
*/

static void
gmem_read_backtrace (void)
{
  fread (&cur_tb_len, sizeof (int), 1, gmemfile);
  fread (tracebk, sizeof (char *), cur_tb_len, gmemfile);
  cur_tb_pos = 0;
}

/* initialize gmem feature from the dumpname file. It returns 1 if the
   dumpname has been generated by GMEM (instrumented malloc/free) and 0 if not
   (i.e. probably a GDB generated file).
*/

int __gnat_gmem_initialize (char *dumpname)
{
  char header [10];

  gmemfile = fopen (dumpname, "rb");
  fread (header, 10, 1, gmemfile);

  /* check for GMEM magic-tag */
  if (memcmp (header, "GMEM DUMP\n", 10))
    {
      fclose (gmemfile);
      return 0;
    }

  return 1;
}

/* initialize addr2line library */

void __gnat_gmem_a2l_initialize (char *exename)
{
  extern char **gnat_argv;
  char s [100];
  int l;

  gnat_argv [0] = exename;
  convert_addresses (tracebk, 1, s, &l);
}

/* Read next allocation of deallocation information from the GMEM file and
   write an alloc/free information in buf to be processed by gnatmem */

void
__gnat_gmem_read_next (struct struct_storage_elmt *buf)
{
  void *addr;
  size_t size;
  int j;

  j = fgetc (gmemfile);
  if (j == EOF)
    {
      fclose (gmemfile);
      buf->Elmt = LOG_EOF;
    }
  else
    {
      switch (j)
        {
          case 'A' :
            buf->Elmt = LOG_ALLOC;
            fread (&(buf->Address), sizeof (void *), 1, gmemfile);
            fread (&(buf->Size), sizeof (size_t), 1, gmemfile);
            break;
          case 'D' :
            buf->Elmt = LOG_DEALL;
            fread (&(buf->Address), sizeof (void *), 1, gmemfile);
            break;
          default:
            puts ("GNATMEM dump file corrupt");
            __gnat_os_exit (1);
        }

      gmem_read_backtrace ();
    }
}

/* Read the next frame from the current traceback, and move the cursor to the
   next frame */

void __gnat_gmem_read_next_frame (void** addr)
{
  if (cur_tb_pos >= cur_tb_len) {
    *addr = NULL;
  } else {
    *addr = (void*)*(tracebk + cur_tb_pos);
    ++cur_tb_pos;
  }
}

/* Converts addr into a symbolic traceback, and stores the result in buf
   with a format suitable for gnatmem */

void __gnat_gmem_symbolic (void * addr, char* buf, int* length)
{
  char* addresses [] = { (char*)addr };
  extern char** gnat_argv;

  convert_addresses (addresses, 1, buf, length);
}
