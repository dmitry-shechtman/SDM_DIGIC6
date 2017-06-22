#include "stdlib.h" // for NULL

static int ignore_current_write=0; //used by the platform routine to check whether to write the current file
#ifdef CAM_DRYOS
static long fwt_bytes_written = 0; // need to track this independently of ptp
volatile int current_write_ignored=0; //needed to prevent nasty surprises with the current override code
#endif //CAM_DRYOS

// wrapper functions for use in filewritetask
#ifdef CAM_DRYOS
int fwt_open(const char *name, int flags, int mode) 
{
    if (!ignore_current_write) 
    {
        return _Open(name, flags, mode);
    }
    current_write_ignored=1;
    return 255; // fake, invalid file descriptor
}

int fwt_write(int fd, const void *buffer, long nbytes) 
{
    if (!current_write_ignored) 
    {
        return _Write(fd, buffer, nbytes);
    }
    fwt_bytes_written += nbytes;
    return (int)nbytes; // "everything's written"
}

#ifdef CAM_FILEWRITETASK_SEEKS
int fwt_lseek(int fd, long offset, int whence) {
    if (!current_write_ignored) {
        return _lseek(fd, offset, whence);
    }
    return (int)offset; // "file position set to the requested value"
}
#endif // CAM_FILEWRITETASK_SEEKS

int fwt_close (int fd) {
    if (!current_write_ignored) 
    {
        int ret = _Close(fd);
        fwt_bytes_written = 0;
        return ret;
    }
 //   filewrite_jpeg_complete();
    return 0;
}

void filewrite_set_discard_jpeg(int state) 
{
 ignore_current_write = state;
}

#endif //CAM_DRYOS
