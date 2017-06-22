

long strlen(const char *str)
{
  const char *start = str;

  while (*str)
    str++;

  return str - start;
}
