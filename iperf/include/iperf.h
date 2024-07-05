#ifndef _IPERF_H_
#define _IPERF_H_

#ifdef __cplusplus
extern "C" { /* open extern "C" */
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  /* _GNU_SOURCE */

#include <sched.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <stdio.h>

#include "list.h"

struct iperf_test;

struct iperf_stream
{
    struct iperf_test* test;

    /* configurable members */
    int       local_port;
    int       remote_port;
    int       socket;
    int       id;
	/* XXX: is settings just a pointer to the same struct in iperf_test? if not, 
		should it be? */
    struct iperf_settings *settings;	/* pointer to structure settings */

    /* non configurable members */
    struct iperf_stream_result *result;	/* structure pointer to result */
    int     *send_timerfd;
    int       green_light;
    int       buffer_fd;	/* data to send, file descriptor */
    char      *buffer;		/* data to send, mmapped */

    /*
     * for udp measurements - This can be a structure outside stream, and
     * stream can have a pointer to this
     */
    int       packet_count;
    int	      peer_packet_count;
    double    jitter;
    double    prev_transit;
    int       outoforder_packets;
    int       cnt_error;
    uint64_t  target;

    struct sockaddr_storage local_addr;
    struct sockaddr_storage remote_addr;

    int       (*rcv) (struct iperf_stream * stream);
    int       (*snd) (struct iperf_stream * stream);

    void     *data;
};


struct iperf_settings
{
    int         domain;               /* AF_INET or AF_INET6 */
    int         socket_bufsize;       /* window size for TCP */
    int         blksize;              /* size of read/writes (-l) */
    uint64_t    rate;                 /* target data rate for application pacing*/
    uint64_t    fqrate;               /* target data rate for FQ pacing*/
    int         pacing_timer;	    /* pacing timer in microseconds */
    int         burst;                /* packets per burst */
    int         mss;                  /* for TCP MSS */
    int         ttl;                  /* IP TTL option */
    int         tos;                  /* type of service bit */
    uint64_t    bytes;             /* number of bytes to send */
    uint64_t    blocks;            /* number of blocks (packets) to send */
    char        unit_format;          /* -f */
    int         num_ostreams;         /* SCTP initmsg settings */
    int	        connect_timeout;	    /* socket connection timeout, in ms */
};

struct protocol {
    struct list_head list;
    int       id;
    char      *name;
    int       (*accept)(struct iperf_test *);
    int       (*listen)(struct iperf_test *);
    int       (*connect)(struct iperf_test *);
    int       (*send)(struct iperf_stream *);
    int       (*recv)(struct iperf_stream *);
    int       (*init)(struct iperf_test *);
};

struct iperf_test
{
    char      role;                             /* 'c' lient or 's' erver */
    int       sender;                           /* client & !reverse or server & reverse */
    int       sender_has_retransmits;
    struct protocol *protocol;
    signed char state;
    char     *server_hostname;                  /* -c option */
    char     *bind_address;                     /* first -B option */

    int       bind_port;                        /* --cport option */
    int       server_port;

    int       duration;                         /* total duration of test (-t flag) */
    int       affinity, server_affinity;	/* -A option */
    cpu_set_t cpumask;

    char     *title;				/* -T option */
    char     *congestion;			/* -C option */
    char     *congestion_used;			/* what was actually used */
    char     *remote_congestion_used;		/* what the other side used */
    char     *pidfile;				/* -P option */

    char     *logfile;				/* --logfile option */
    FILE     *outfile;

    int       ctrl_sck;
    int       listener;
    int       epoll_fd;
    int       timer_fd;

    int	      ctrl_sck_mss;			/* MSS for the control channel */
    char     *server_rsa_private_key;
    char     *server_authorized_users;

    /* boolean variables for Options */
    int       daemon;                           /* -D option */
    int       one_off;                          /* -1 option */
    int       no_delay;                         /* -N option */
    int       reverse;                          /* -R option */
    int	      verbose;                          /* -V option - verbose mode */
    int	      json_output;                      /* -J option - JSON output */
    int	      zerocopy;                         /* -Z option - use sendfile */
    int       debug;				/* -d option - enable debug */
    int	      get_server_output;		/* --get-server-output */
    int	      udp_counters_64bit;		/* --use-64-bit-udp-counters */
    int       forceflush; /* --forceflush - flushing output at every interval */
    int	      multisend;

    char     *json_output_string; /* rendered JSON output if json_output is set */

    int       num_streams;                      /* total streams in the test (-P) */
#if 0
    iperf_size_t bytes_sent;
    iperf_size_t blocks_sent;
#endif
    struct list_head streams;

    struct iperf_settings *settings;

    struct list_head protocols;

    /* callback functions */
    void      (*on_new_stream)(struct iperf_stream *);
    void      (*on_test_start)(struct iperf_test *);
    void      (*on_connect)(struct iperf_test *);
    void      (*on_test_finish)(struct iperf_test *);
#if 0
    /* cJSON handles for use when in -J mode */\
    cJSON *json_top;
    cJSON *json_start;
    cJSON *json_connected;
    cJSON *json_intervals;
    cJSON *json_end;

    /* Server output (use on client side only) */
    char *server_output_text;
    cJSON *json_server_output;

    /* Server output (use on server side only) */
    TAILQ_HEAD(iperf_textlisthead, iperf_textline) server_output_list;
#endif
};

#define PORT 5201  /* default port to listen on (don't use the same port as iperf2) */
#define SEC_TO_US 1000000LL
#define DURATION 10 /* seconds */

/* constants for command line arg sanity checks */
#define MB (1024 * 1024)
#define MAX_TCP_BUFFER (512 * MB)
#define MAX_BLOCKSIZE MB
/* Minimum size UDP send is the size of two 32-bit ints followed by a 64-bit int */
#define MIN_UDP_BLOCKSIZE (4 + 4 + 8)
/* Maximum size UDP send is (64K - 1) - IP and UDP header sizes */
#define MAX_UDP_BLOCKSIZE (65535 - 8 - 20)
#define MIN_INTERVAL 0.1
#define MAX_INTERVAL 60.0
#define MAX_TIME 86400
#define MAX_BURST 1000
#define MAX_MSS (9 * 1024)
#define MAX_STREAMS 128

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif  /* _IPERF_H_ */