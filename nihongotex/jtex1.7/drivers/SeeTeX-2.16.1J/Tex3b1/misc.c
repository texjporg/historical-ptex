/* Berkeley type bcopy, allows for overlapped strings (copies backward). */
bcopy (s,d,n)
char *s, *d;
int n;
{
   register char *from, *to, *base;

   base = s;
   from = base + n;
   to = d + n;
   while (from > base)
      *--to = *--from;
}

bzero (s,n)
char *s;
int n;
{
   register char *start, *end;

   start = s;
   end = start + n;

   while (start < end)
      *start++ = 0;
}
