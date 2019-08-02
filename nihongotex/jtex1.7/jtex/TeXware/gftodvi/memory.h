/*
 * Global Datastructures too hard to translate automatically from Pascal.
 */

#define b0 u.B0
#define b1 u.B1

typedef struct {
    struct {
	quarterword B0;
	quarterword B1;
    } u;
    quarterword b2;
    quarterword b3;
} fourquarters;

typedef union {
    integer cint;
    glueratio gr;
    fourquarters qqqq;
} memoryword;
