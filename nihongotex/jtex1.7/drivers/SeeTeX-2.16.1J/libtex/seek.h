/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Declarations for seek.c: SeekFile(FILE *fp) returns a
 * version of `fp' on which `fseek' functions properly;
 * CopyFile(FILE *fp) returns a temporary copy of fp.
 */

FILE *SeekFile(/* FILE *fp */), *CopyFile(/* FILE *fp */);
