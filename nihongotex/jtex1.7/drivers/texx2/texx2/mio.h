/*
 *	This program is Copyright (C) 1987 by the Board of Trustees of the
 *	University of Illinois, and by the author Dirk Grunwald.
 */

/*
 * Memory I/O: numbers.
 *
 */

#ifdef __STDC__

static 
inline int mGetByte(char **m)
{
    unsigned char foo = **m;
    unsigned int retval = foo & 0xff;
    (*m)++;
    return retval;
}

static 
inline void mGetWord( char **m, i32 *r)
{
    int x = mGetByte( m ) << 8;
    x |= mGetByte(m); 
    *r = x;
}

static 
inline void mGet3Byte( char **m, i32 *r)
{
    long x = mGetByte( m ) << 16;
    x |= ( mGetByte(m ) << 8 ); 
    x |= mGetByte(m);
    *r = x;
}

static 
inline void mGetLong( char **m, i32 *r)
{
    long x = mGetByte( m ) << 24;
    x |= ( mGetByte(m) << 16 ); 
    x |= ( mGetByte(m) << 8 ); 
    x |= mGetByte(m); 
    *r = x;
}

#else

#define	mGetByte(m)	( *((*m)++) & 0xff )

#define mGetWord(m, r)	{*(r)  = mGetByte(m) << 8;  *(r) |= mGetByte(m);}

#define mGet3Byte(m,r) {*(r)  = mGetByte(m) << 16;\
			 *(r) |= mGetByte(m) << 8;\
			 *(r) |= mGetByte(m);}

#define mGetLong(m, r)	{*(r)  = mGetByte(m) << 24;\
			 *(r) |= mGetByte(m) << 16;\
			 *(r) |= mGetByte(m) << 8;\
			 *(r) |= mGetByte(m);}

#endif
