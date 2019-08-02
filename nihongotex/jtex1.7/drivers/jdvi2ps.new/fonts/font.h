/*
 * Copyright (c) 1989 Matsushita Electric Industrial Co. Ltd.
 *
 * $Header: font.h,v 1.5 89/12/28 21:06:01 kakiuchi Exp $
 * $Author: kakiuchi $
 * $Date: 89/12/28 21:06:01 $
 */

/*
 * This is a common header file for font manipulation program.
*/
#include <stdio.h>
#include <sys/param.h>		/* For MAXPATHLEN */

/*
 * Constants.
 * =========
 */

/*
 * Max number of open font files.
 * MAXOPEN must be less than NOPEN defined sys/param.h.  But it is not sure
 * that NOFILE is REALLY the 'maximum number of open files per process'.
 * This is because the maximum number of open files is dependent on the
 * configuration of OS.
 * Furthermore, we must not exhaust all file descriptors, because applications
 * would open files for another purpose, for example dvi file descriptor, and
 * we will open tfm files for extracting metrics information.
 * But normally, maximum number of open files is larger than 20.
 */
#define MAXOPEN (NOFILE-10)

/* Bitmap construction */
#define ONEBYTES
#undef TWOBYTES
#undef FOURBYTES

#ifdef ONEBYTES
#define UTYPE unsigned char
#define USHIFT 3
#endif
#ifdef TWOBYTES
#define UTYPE unsigned short
#define USHIFT 4
#endif
#ifdef FOURBYTES
#define UTYPE unsigned int
#define USHIFT 5
#endif

#define BITMAPUNIT (0x1 << USHIFT)
#define UBYTES (BITMAPUNIT / 8)
#define UMASK (BITMAPUNIT - 1)
#define UWIDTH(w) (((w) + UMASK) >> USHIFT)
#define BYTEOFFSET(x) ((x) >> USHIFT)

/*
 * Macro functions
 */
/* pix_round(x,conv): return rounded number of pixels */
/*           x: in DVI units                          */
/*           conv: conversion factor                  */
#define pix_round(x,conv) ((int)(((x) + (conv >> 1)) / conv))
#define HConv(x) pix_round(x,hconv)
#define VConv(x) pix_round(x,vconv)

#define get_unsigned1(fp) ((unsigned int) getc(fp))
#define get_unsigned2(fp) get_unsigned(fp,2)
#define get_unsigned3(fp) get_unsigned(fp,3)
#define get_unsigned4(fp) get_unsigned(fp,4)
#define get_signed1(fp) getc(fp)
#define get_signed2(fp) get_signed(fp,2)
#define get_signed3(fp) get_signed(fp,3)
#define get_signed4(fp) get_signed(fp,4)

extern int hconv, vconv;

/*
 * Font information for common area of different font file formats.
 */
typedef struct {
  char *extension;
  double multiplier;
  int (*init_font)();
  int (*open_font)();
  struct char_entry *(*get_glyph)();
  int (*free_font)();
} FontOpStruct;

typedef struct font_ops {
  FontOpStruct op;
  struct font_ops *next;
} FontOps;

typedef struct {
  unsigned int k;               /* font number */
  unsigned int c;               /* check_sum of font file */
  unsigned int s;               /* space size */
  unsigned int d;               /* designsize */
  unsigned char a;              /* font area length */
  unsigned char l;              /* font name length */
  char *n;                      /* FNT_DEF command parameters  */
  double font_mag;              /* computed from s and d parameters */
  char *name;                   /* full name of font file */
  FILE *font_file_fd;           /* file identifier (0 if none) */
  FontOpStruct *font_ops;       /* font file operations */
  struct font_entry *next;
  unsigned short access_count;  /* count of file access */
  long app_attribute;           /* field for an application specific use */
} CommonEntry;

typedef struct char_entry {
  struct where_is {
    int isloaded;          /* whether glyph is already loaded on memory */
    union {
      long fileOffset;     /* offset to position in glyph file */
      char *bitmap;        /* pointer to the bitmap */
    } address;
  } where;
  unsigned short width;    /* width in pixels */
  unsigned short height;   /* height in pixels */
  short  xOffset;          /* x-offset in pixels */
  short  yOffset;          /* y-offset in pixels */
  int    tfmw;             /* character width in sp */
  long   glyph_attribute;  /* field for an application specific use */
} CharEntry;
