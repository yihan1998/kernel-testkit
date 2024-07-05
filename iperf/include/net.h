#ifndef _NET_H_
#define _NET_H_

#ifdef __cplusplus
extern "C" { /* open extern "C" */
#endif

int netannounce(int domain, int proto, char *local, int port);
int Nread(int fd, char *buf, size_t count, int prot);
int Nwrite(int fd, const char *buf, size_t count, int prot);
int getsock_tcp_mss(int inSock);
int set_tcp_options(int sock, int no_delay, int mss);
int setnonblocking(int fd, int nonblocking);
int getsockdomain(int sock);

#define NET_SOFTERROR -1
#define NET_HARDERROR -2

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif  /* _NET_H_ */
