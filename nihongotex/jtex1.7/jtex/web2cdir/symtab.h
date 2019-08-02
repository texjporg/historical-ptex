/*
 * This is the one thing we need to include in web2c.c as well as
 * in other modules which include web2c.h.
 */

struct sym_entry {
  char *id;	/* points to the identifier */
  int typ;	/* token type */
  int next;	/* next symbol entry */
  long val;	/* constant : value
		   subrange type : lower bound */
  int upper; 	/* subrange type : upper bound
		   variable, function, type or field : type length */
  int val_sym, upper_sym;	/* Sym table entries of symbols for lower
				   and upper bounds
				 */
  char var_formal;	/* Is this a formal parameter by reference? */
  char var_not_needed;	/* TRUE if VAR token should be ignored for this type */
};
