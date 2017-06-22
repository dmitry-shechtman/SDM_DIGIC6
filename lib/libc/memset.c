/* memset
   This implementation is in the public domain.  */



void*
memset (void* dest, register int val, register long len)
{
  register unsigned char *ptr = (unsigned char*)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}
