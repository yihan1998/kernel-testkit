#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/epoll.h>
#include <sched.h>

#include "net.h"
#include "iperf.h"
#include "iperf_api.h"

#define MAX_EVENTS 128

int iperf_server_listen(struct iperf_test *test)
{
    if((test->listener = netannounce(test->settings->domain, Ptcp, test->bind_address, test->server_port)) < 0) {
        printf("Failed to listen!\n");
	    errno = ENETRESET;
	    return -1;
    }

	printf("-----------------------------------------------------------\n");
	printf("Server listening on %d\n", test->server_port);
	printf("-----------------------------------------------------------\n");

    return 0;
}

int iperf_accept(struct iperf_test *test)
{
    int s;
    signed char rbuf = ACCESS_DENIED;
    socklen_t len;
    struct sockaddr_storage addr;

    len = sizeof(addr);
    if ((s = accept(test->listener, (struct sockaddr *) &addr, &len)) < 0) {
        errno = ECONNRESET;
        return -1;
    }

    if (test->ctrl_sck == -1) {
        /* Server free, accept new client */
        test->ctrl_sck = s;
	    if (iperf_set_send_state(test, PARAM_EXCHANGE) != 0) {
            return -1;
        }

        if (iperf_exchange_parameters(test) < 0) {
            return -1;
        }

        if (test->on_connect) {
            test->on_connect(test);
        }
    } else {
	/*
	 * Don't try to read from the socket.  It could block an ongoing test. 
	 * Just send ACCESS_DENIED.
	 */
        if (Nwrite(s, (char*) &rbuf, sizeof(rbuf), Ptcp) < 0) {
            errno = ECONNREFUSED;
            return -1;
        }
        close(s);
    }

    return 0;
}


int iperf_run_server(struct iperf_test *test)
{
    int nevents;
    struct epoll_event events[MAX_EVENTS];

    // Open epoll
    if (iperf_create_epoll(test) < 0) {
        return -2;
    }

    // Open socket and listen
    if (iperf_server_listen(test) < 0) {
        return -2;
    }

    // Open record timer
    if (iperf_create_timerfd(test) < 0) {
        return -2;
    }

    while (test->state != IPERF_DONE) {

        if (test->state != CREATE_STREAMS) {
            if (iperf_accept(test) < 0) {
                return -1;
            }
        }

        nevents = epoll_wait(test->epoll_fd, events, MAX_EVENTS, -1);
        if (nevents == -1) {
            perror("epoll_wait failed");
            return 1;
        }

        for (int i = 0; i < nevents; i++) {
            if (events[i].data.fd == test->listener) {

            }
        }        
    }

    return 0;
}