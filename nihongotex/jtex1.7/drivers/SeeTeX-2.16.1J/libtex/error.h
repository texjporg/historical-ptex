/*
 * Copyright (c) 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * Errors.
 */
void	error(/* int quit, int e, char *fmt, ... */);
void	panic(/* char *fmt, ... */);

void	SetErrorTrap(/* void (*fn)(int quit, char *text) */);
