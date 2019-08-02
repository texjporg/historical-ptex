/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: common.h,v 1.5 89/12/28 21:05:59 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:05:59 $
 */

/*
 * Some type definitions for font libraries
 */
#ifndef ulong
#define ulong  unsigned long
#endif
#ifndef uint
#define uint   unsigned int
#endif
#ifndef ushort
#define ushort unsigned short
#endif
#ifndef uchar
#define uchar  unsigned char
#endif
#ifndef bool
#define bool   int
#endif
#ifndef True
#define True 1
#endif
#ifndef False
#define False 0
#endif
#ifndef fix
#define fix(x) (int)(x + 0.5)
#endif

/*
 * public data structures to each font functions
 */
typedef struct k_char_entry {
  CharEntry ch;
  ushort code;
  struct k_char_entry *left, *right;
} KCharEntry;

/*
 * public functions to each font functions
 */
extern FILE *open_file();
extern void close_file();
extern int search_font_file();
extern int search_tfm_file();
extern char *AllocMemory();
extern int get_kchar_entry();
