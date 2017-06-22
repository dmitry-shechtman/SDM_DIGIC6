/* strncmp -- compare two strings, stop after n bytes.
   This function is in the public domain.  */






int
strncmp(const char *s1, const char *s2, register long n)
{
  register unsigned char u1, u2;

  while (n-- > 0)
    {
      u1 = (unsigned char) *s1++;
      u2 = (unsigned char) *s2++;
      if (u1 != u2)
	return u1 - u2;
      if (u1 == '\0')
	return 0;
    }
  return 0;
}
