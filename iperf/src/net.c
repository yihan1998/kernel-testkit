#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <assert.h>
#include <netdb.h>
#include <string.h>
#include <sys/fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "net.h"

/*************************************************************************/

/**
 * getsock_tcp_mss - Returns the MSS size for TCP
 *
 */

int getsock_tcp_mss(int inSock)
{
    int             mss = 0;

    int             rc;
    socklen_t       len;

    assert(inSock >= 0); /* print error and exit if this is not true */

    /* query for mss */
    len = sizeof(mss);
    rc = getsockopt(inSock, IPPROTO_TCP, TCP_MAXSEG, (char *)&mss, &len);
    if (rc == -1) {
        perror("getsockopt TCP_MAXSEG");
        return -1;
    }

    return mss;
}

/*************************************************************/

/* sets TCP_NODELAY and TCP_MAXSEG if requested */

int set_tcp_options(int sock, int no_delay, int mss)
{
    socklen_t len;
    int rc;
    int new_mss;

    if (no_delay == 1) {
        len = sizeof(no_delay);
        rc = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&no_delay, len);
        if (rc == -1) {
            perror("setsockopt TCP_NODELAY");
            return -1;
        }
    }

    if (mss > 0) {
        len = sizeof(new_mss);
        assert(sock != -1);

        /* set */
        new_mss = mss;
        len = sizeof(new_mss);
        rc = setsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, (char *)&new_mss, len);
        if (rc == -1) {
            perror("setsockopt TCP_MAXSEG");
            return -1;
        }
        /* verify results */
        rc = getsockopt(sock, IPPROTO_TCP, TCP_MAXSEG, (char *)&new_mss, &len);
        if (rc == -1) {
            perror("getsockopt TCP_MAXSEG");
            return -1;
        }
        if (new_mss != mss) {
            perror("setsockopt value mismatch");
            return -1;
        }
    }

    return 0;
}

/****************************************************************************/

int setnonblocking(int fd, int nonblocking)
{
    int flags, newflags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl(F_GETFL)");
        return -1;
    }

    if (nonblocking) {
    	newflags = flags | (int) O_NONBLOCK;
    } else {
    	newflags = flags & ~((int) O_NONBLOCK);
    }

    if (newflags != flags) {
        if (fcntl(fd, F_SETFL, newflags) < 0) {
            perror("fcntl(F_SETFL)");
            return -1;
        }
    }

    return 0;
}

int Nread(int fd, char *buf, size_t count, int prot)
{
    register ssize_t r;
    register size_t nleft = count;

    while (nleft > 0) {
        r = read(fd, buf, nleft);
        if (r < 0) {
            if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                return NET_HARDERROR;
            }
        } else if (r == 0) {
            break;
        }

        nleft -= r;
        buf += r;
    }
    return count - nleft;
}

int Nwrite(int fd, const char *buf, size_t count, int prot)
{
    register ssize_t r;
    register size_t nleft = count;

    while (nleft > 0) {
        r = write(fd, buf, nleft);
        if (r < 0) {
            switch (errno) {
                case EINTR:
                case EAGAIN:
                    return count - nleft;

                case ENOBUFS:
                    return NET_SOFTERROR;

                default:
                    return NET_HARDERROR;
            }
        } else if (r == 0)
            return NET_SOFTERROR;
        nleft -= r;
        buf += r;
    }
    return count;
}


int netannounce(int domain, int proto, char *local, int port)
{
    struct addrinfo hints, *res;
    char portstr[6];
    int s, opt;

    snprintf(portstr, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    /* 
     * If binding to the wildcard address with no explicit address
     * family specified, then force us to get an AF_INET6 socket.  On
     * CentOS 6 and MacOS, getaddrinfo(3) with AF_UNSPEC in ai_family,
     * and ai_flags containing AI_PASSIVE returns a result structure
     * with ai_family set to AF_INET, with the result that we create
     * and bind an IPv4 address wildcard address and by default, we
     * can't accept IPv6 connections.
     *
     * On FreeBSD, under the above circumstances, ai_family in the
     * result structure is set to AF_INET6.
     */
    if (domain == AF_UNSPEC && !local) {
	    hints.ai_family = AF_INET6;
    } else {
	    hints.ai_family = domain;
    }
    hints.ai_socktype = proto;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(local, portstr, &hints, &res) != 0) {
        return -1; 
    }

    s = socket(res->ai_family, proto, 0);
    if (s < 0) {
	    freeaddrinfo(res);
        return -1;
    }

    opt = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR,  (char *) &opt, sizeof(opt)) < 0) {
        close(s);
        freeaddrinfo(res);
        return -1;
    }

    if (bind(s, (struct sockaddr *) res->ai_addr, res->ai_addrlen) < 0) {
        close(s);
	    freeaddrinfo(res);
        return -1;
    }

    freeaddrinfo(res);
    
    if (proto == SOCK_STREAM) {
        if (listen(s, 5) < 0) {
	        close(s);
            return -1;
        }
    }

    return s;
}

/****************************************************************************/

int getsockdomain(int sock)
{
    struct sockaddr_storage sa;
    socklen_t len = sizeof(sa);

    if (getsockname(sock, (struct sockaddr *)&sa, &len) < 0) {
        return -1;
    }
    return ((struct sockaddr *) &sa)->sa_family;
}
