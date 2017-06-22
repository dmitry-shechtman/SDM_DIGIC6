/* memcpy (the standard C function)
   This function is in the public domain.  */



void *memmove (void*, const void*, long);

void*
memcpy (void* out, const void* in, long length)
{
    return memmove(out, in, length);
}
