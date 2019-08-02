/*
 * Global Datastructures too hard to translate automatically from Pascal.
 */

typedef union {
    struct {
	halfword RH, LH;
    } v;
    struct {
	halfword junk_space;	/* Make B0,B1 overlap LH in memory */
	quarterword B0, B1;
    } u;
} twohalves;
#define	b0	u.B0
#define	b1	u.B1

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
    twohalves hh;
    fourquarters qqqq;
} memoryword;
