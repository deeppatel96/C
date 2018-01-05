#ifndef __LIBNETFILES_H__
#define __LIBNETFILES_H__

#define unrestricted 0
#define exclusive 1
#define transaction 2


int netserverinit(char *hostname, int filemode);
int netopen(const char *pathname, int flags);
ssize_t netread(int fildes, void *buf, size_t nbyte);
ssize_t netwrite(int fildes, const void *buf, size_t nbyte);
int netclose(const char *pathname, int flags);


#endif // __LIBNETFILES_H__