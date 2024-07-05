#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif  /* _GNU_SOURCE */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <errno.h>
#include <sys/epoll.h>
#include <time.h>
#include <sys/timerfd.h>
#include <stdarg.h>

#include "cjson.h"
#include "net.h"
#include "iperf.h"
#include "iperf_api.h"
#include "iperf_locale.h"

static int send_parameters(struct iperf_test *test);
static int get_parameters(struct iperf_test *test);
static int JSON_write(int fd, cJSON *json);
// static void print_interval_results(struct iperf_test *test, struct iperf_stream *sp, cJSON *json_interval_streams);
static cJSON *JSON_read(int fd);

/************** Getter routines for some fields inside iperf_test *************/

int
iperf_get_verbose(struct iperf_test *ipt)
{
    return ipt->verbose;
}

int
iperf_get_control_socket(struct iperf_test *ipt)
{
    return ipt->ctrl_sck;
}

int
iperf_get_control_socket_mss(struct iperf_test *ipt)
{
    return ipt->ctrl_sck_mss;
}

int
iperf_get_test_duration(struct iperf_test *ipt)
{
    return ipt->duration;
}

uint64_t
iperf_get_test_rate(struct iperf_test *ipt)
{
    return ipt->settings->rate;
}

uint64_t
iperf_get_test_fqrate(struct iperf_test *ipt)
{
    return ipt->settings->fqrate;
}

int
iperf_get_test_burst(struct iperf_test *ipt)
{
    return ipt->settings->burst;
}

char
iperf_get_test_role(struct iperf_test *ipt)
{
    return ipt->role;
}

int
iperf_get_test_reverse(struct iperf_test *ipt)
{
    return ipt->reverse;
}

int
iperf_get_test_blksize(struct iperf_test *ipt)
{
    return ipt->settings->blksize;
}

FILE *
iperf_get_test_outfile (struct iperf_test *ipt)
{
    return ipt->outfile;
}

int
iperf_get_test_socket_bufsize(struct iperf_test *ipt)
{
    return ipt->settings->socket_bufsize;
}

int
iperf_get_test_num_streams(struct iperf_test *ipt)
{
    return ipt->num_streams;
}

int
iperf_get_test_server_port(struct iperf_test *ipt)
{
    return ipt->server_port;
}

char*
iperf_get_test_server_hostname(struct iperf_test *ipt)
{
    return ipt->server_hostname;
}

int
iperf_get_test_protocol_id(struct iperf_test *ipt)
{
    return ipt->protocol->id;
}

int
iperf_get_test_json_output(struct iperf_test *ipt)
{
    return ipt->json_output;
}

char *
iperf_get_test_json_output_string(struct iperf_test *ipt)
{
    return ipt->json_output_string;
}

int
iperf_get_test_zerocopy(struct iperf_test *ipt)
{
    return ipt->zerocopy;
}

int
iperf_get_test_get_server_output(struct iperf_test *ipt)
{
    return ipt->get_server_output;
}

char
iperf_get_test_unit_format(struct iperf_test *ipt)
{
    return ipt->settings->unit_format;
}

char *
iperf_get_test_bind_address(struct iperf_test *ipt)
{
    return ipt->bind_address;
}

int
iperf_get_test_udp_counters_64bit(struct iperf_test *ipt)
{
    return ipt->udp_counters_64bit;
}

int
iperf_get_test_one_off(struct iperf_test *ipt)
{
    return ipt->one_off;
}

/************** Setter routines for some fields inside iperf_test *************/

void
iperf_set_verbose(struct iperf_test *ipt, int verbose)
{
    ipt->verbose = verbose;
}

void
iperf_set_control_socket(struct iperf_test *ipt, int ctrl_sck)
{
    ipt->ctrl_sck = ctrl_sck;
}

void
iperf_set_test_duration(struct iperf_test *ipt, int duration)
{
    ipt->duration = duration;
}

void
iperf_set_test_state(struct iperf_test *ipt, signed char state)
{
    ipt->state = state;
}

void
iperf_set_test_blksize(struct iperf_test *ipt, int blksize)
{
    ipt->settings->blksize = blksize;
}

void
iperf_set_test_rate(struct iperf_test *ipt, uint64_t rate)
{
    ipt->settings->rate = rate;
}

void
iperf_set_test_fqrate(struct iperf_test *ipt, uint64_t fqrate)
{
    ipt->settings->fqrate = fqrate;
}

void
iperf_set_test_burst(struct iperf_test *ipt, int burst)
{
    ipt->settings->burst = burst;
}

void
iperf_set_test_server_port(struct iperf_test *ipt, int srv_port)
{
    ipt->server_port = srv_port;
}

void
iperf_set_test_socket_bufsize(struct iperf_test *ipt, int socket_bufsize)
{
    ipt->settings->socket_bufsize = socket_bufsize;
}

void
iperf_set_test_num_streams(struct iperf_test *ipt, int num_streams)
{
    ipt->num_streams = num_streams;
}

static void
check_sender_has_retransmits(struct iperf_test *ipt)
{
    if (ipt->sender && ipt->protocol->id == Ptcp)
	ipt->sender_has_retransmits = 1;
    else
	ipt->sender_has_retransmits = 0;
}

void
iperf_set_test_role(struct iperf_test *ipt, char role)
{
    ipt->role = role;
    if (role == 'c')
	ipt->sender = 1;
    else if (role == 's')
	ipt->sender = 0;
    if (ipt->reverse)
        ipt->sender = ! ipt->sender;
    check_sender_has_retransmits(ipt);
}

void
iperf_set_test_server_hostname(struct iperf_test *ipt, char *server_hostname)
{
    ipt->server_hostname = strdup(server_hostname);
}

void
iperf_set_test_reverse(struct iperf_test *ipt, int reverse)
{
    ipt->reverse = reverse;
    if (ipt->reverse)
        ipt->sender = ! ipt->sender;
    check_sender_has_retransmits(ipt);
}

void
iperf_set_test_json_output(struct iperf_test *ipt, int json_output)
{
    ipt->json_output = json_output;
}

int
iperf_has_zerocopy( void )
{
    return 1;
}

void
iperf_set_test_zerocopy(struct iperf_test *ipt, int zerocopy)
{
    ipt->zerocopy = zerocopy;
}

void
iperf_set_test_get_server_output(struct iperf_test *ipt, int get_server_output)
{
    ipt->get_server_output = get_server_output;
}

void
iperf_set_test_unit_format(struct iperf_test *ipt, char unit_format)
{
    ipt->settings->unit_format = unit_format;
}

void
iperf_set_test_bind_address(struct iperf_test *ipt, char *bnd_address)
{
    ipt->bind_address = strdup(bnd_address);
}

void
iperf_set_test_udp_counters_64bit(struct iperf_test *ipt, int udp_counters_64bit)
{
    ipt->udp_counters_64bit = udp_counters_64bit;
}

void
iperf_set_test_one_off(struct iperf_test *ipt, int one_off)
{
    ipt->one_off = one_off;
}

int iperf_create_epoll(struct iperf_test *ipt) 
{
    ipt->epoll_fd = epoll_create1(0);
    if (ipt->epoll_fd == -1) {
        perror("epoll_create1 failed");
        return 1;
    }
    return 0;
}

int iperf_create_timerfd(struct iperf_test *ipt) 
{
    ipt->timer_fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK);
    if (ipt->timer_fd == -1) {
        perror("timerfd_create failed");
        return -1;
    }
    return 0;
}

int iperf_register_timerfd(struct iperf_test *ipt) 
{
    struct epoll_event event;
    struct itimerspec timer_spec = {0};
    timer_spec.it_interval.tv_sec = 1;
    timer_spec.it_interval.tv_nsec = 0;
    timer_spec.it_value = timer_spec.it_interval;
    if (timerfd_settime(ipt->timer_fd, 0, &timer_spec, NULL) == -1) {
        perror("Failed to set timerfd");
        close(ipt->timer_fd);
        close(ipt->epoll_fd);
        return -1;
    }

    event.events = EPOLLIN;
    event.data.fd = ipt->timer_fd;
    if (epoll_ctl(ipt->epoll_fd, EPOLL_CTL_ADD, ipt->timer_fd, &event) == -1) {
        perror("Epoll_ctl (timer_fd) failed");
        close(ipt->timer_fd);
        close(ipt->epoll_fd);
        return -1;
    }

    return 0;
}

int iperf_deregister_timerfd(struct iperf_test *ipt)
{
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLOUT;  // Specify the events if needed
    event.data.fd = ipt->timer_fd;

    if (epoll_ctl(ipt->epoll_fd, EPOLL_CTL_DEL, ipt->timer_fd, &event) == -1) {
        perror("epoll_ctl EPOLL_CTL_DEL failed");
        close(ipt->timer_fd);
        return -1;
    }

    return 0;
}

int iperf_set_send_state(struct iperf_test *test, signed char state) {
    test->state = state;
    if (Nwrite(test->ctrl_sck, (char*) &state, sizeof(state), Ptcp) < 0) {
	    errno = ECONNREFUSED;
	    return -1;
    }
    return 0;
}

/**
 * iperf_exchange_parameters - handles the param_Exchange part for client
 *
 */

int iperf_exchange_parameters(struct iperf_test *test)
{

    int s;
    int32_t err;

    if (test->role == 'c') {
        if (send_parameters(test) < 0) {
            return -1;
        }
    } else {
        if (get_parameters(test) < 0) {
            return -1;
        }

        if ((s = test->protocol->listen(test)) < 0) {
	        if (iperf_set_send_state(test, SERVER_ERROR) != 0) {
                return -1;
            }
            err = htonl(errno);
            if (Nwrite(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                errno = EPROTOTYPE;
                return -1;
            }
            err = htonl(errno);
            if (Nwrite(test->ctrl_sck, (char*) &err, sizeof(err), Ptcp) < 0) {
                errno = EPROTOTYPE;
                return -1;
            }
            return -1;
        }

        // Send the control message to create streams and start the test
    	if (iperf_set_send_state(test, CREATE_STREAMS) != 0) {
            return -1;
        }
    }

    return 0;
}

/**************************************************************************/

struct iperf_test * iperf_new_test()
{
    struct iperf_test *test;

    test = (struct iperf_test *)calloc(1, sizeof(struct iperf_test));
    if (!test) {
        printf("Failed to create new test!\n");
        return NULL;
    }

    test->settings = (struct iperf_settings *)calloc(1, sizeof(struct iperf_settings));
    if (!test->settings) {
        printf("Failed to create new test setting!\n");
        free(test);
	    return NULL;
    }

    /* By default all output goes to stdout */
    test->outfile = stdout;

    return test;
}


/**************************************************************************/

struct protocol *
protocol_new(void)
{
    struct protocol *proto;

    proto = (struct protocol *)calloc(1, sizeof(struct protocol));
    if(!proto) {
        return NULL;
    }

    return proto;
}

void protocol_free(struct protocol *proto)
{
    free(proto); 
}

/********************** Get/set test protocol structure ***********************/

struct protocol * get_protocol(struct iperf_test *test, int prot_id)
{
    struct protocol *prot;

    list_for_each_entry(prot, &test->protocols, list) {
        if (prot->id == prot_id)
            break;
    }

    if (prot == NULL) {
        printf("Failed to get protocol!\n");
    }

    return prot;
}

int set_protocol(struct iperf_test *test, int prot_id)
{
    struct protocol *prot = NULL;

    list_for_each_entry(prot, &test->protocols, list) {
        if (prot->id == prot_id) {
            test->protocol = prot;
	        check_sender_has_retransmits(test);
            return 0;
        }
    }

    return -1;
}

/************************** Iperf callback functions **************************/

void iperf_on_new_stream(struct iperf_stream *sp)
{
    // connect_msg(sp);
	return;
}

void iperf_on_test_start(struct iperf_test *test)
{
	return;
}

void iperf_on_connect(struct iperf_test *test)
{
    time_t now_secs;
    const char* rfc1123_fmt = "%a, %d %b %Y %H:%M:%S GMT";
    char now_str[100];
    char ipr[INET6_ADDRSTRLEN];
    int port;
    struct sockaddr_storage sa;
    struct sockaddr_in *sa_inP;
    socklen_t len;

    now_secs = time((time_t*) 0);
    (void) strftime(now_str, sizeof(now_str), rfc1123_fmt, gmtime(&now_secs));

    if (test->role == 'c') {
        iperf_printf(test, report_connecting, test->server_hostname, test->server_port);
    } else {
        len = sizeof(sa);
        getpeername(test->ctrl_sck, (struct sockaddr *) &sa, &len);
        if (getsockdomain(test->ctrl_sck) == AF_INET) {
            sa_inP = (struct sockaddr_in *) &sa;
            inet_ntop(AF_INET, &sa_inP->sin_addr, ipr, sizeof(ipr));
	        port = ntohs(sa_inP->sin_port);
        }
	    iperf_printf(test, report_accepted, ipr, port);
    }
}

void
iperf_on_test_finish(struct iperf_test *test)
{
}

/**************************************************************************/
int iperf_defaults(struct iperf_test *testp)
{
    // struct protocol *tcp, *udp;

    testp->duration = DURATION;
    testp->affinity = -1;
    testp->server_affinity = -1;
    // TAILQ_INIT(&testp->xbind_addrs);
    CPU_ZERO(&testp->cpumask);
    testp->server_port = PORT;
    testp->ctrl_sck = -1;
    testp->epoll_fd = -1;
    testp->timer_fd = -1;

    testp->num_streams = 1;

    testp->settings->domain = AF_UNSPEC;
    testp->settings->unit_format = 'a';
    testp->settings->socket_bufsize = 0;    /* use autotuning */
    testp->settings->blksize = DEFAULT_TCP_BLKSIZE;
    testp->settings->rate = 0;
    testp->settings->fqrate = 0;
    testp->settings->pacing_timer = 1000;
    testp->settings->burst = 0;
    testp->settings->mss = 0;
    testp->settings->bytes = 0;
    testp->settings->blocks = 0;
    testp->settings->connect_timeout = -1;

    /* Set up protocol list */
    init_list_head(&testp->streams);
    init_list_head(&testp->protocols);

#if 0
    tcp = protocol_new();
    if (!tcp) {
        return -1;
    }

    tcp->id = Ptcp;
    tcp->name = "TCP";
    tcp->accept = iperf_tcp_accept;
    tcp->listen = iperf_tcp_listen;
    tcp->connect = iperf_tcp_connect;
    tcp->send = iperf_tcp_send;
    tcp->recv = iperf_tcp_recv;
    tcp->init = NULL;
    SLIST_INSERT_HEAD(&testp->protocols, tcp, protocols);

    udp = protocol_new();
    if (!udp) {
        protocol_free(tcp);
        return -1;
    }

    udp->id = Pudp;
    udp->name = "UDP";
    udp->accept = iperf_udp_accept;
    udp->listen = iperf_udp_listen;
    udp->connect = iperf_udp_connect;
    udp->send = iperf_udp_send;
    udp->recv = iperf_udp_recv;
    udp->init = iperf_udp_init;
    SLIST_INSERT_AFTER(tcp, udp, protocols);

    set_protocol(testp, Ptcp);
#endif

    testp->on_new_stream = iperf_on_new_stream;
    testp->on_test_start = iperf_on_test_start;
    testp->on_connect = iperf_on_connect;
    testp->on_test_finish = iperf_on_test_finish;

    // TAILQ_INIT(&testp->server_output_list);
    return 0;
}

/*************************************************************/

static int
send_parameters(struct iperf_test *test)
{
    int r = 0;
    cJSON *j;

    j = cJSON_CreateObject();
    if (j == NULL) {
	    r = -1;
    } else {
	if (test->protocol->id == Ptcp)
	    cJSON_AddTrueToObject(j, "tcp");
	else if (test->protocol->id == Pudp)
	    cJSON_AddTrueToObject(j, "udp");

	if (test->server_affinity != -1)
	    cJSON_AddNumberToObject(j, "server_affinity", test->server_affinity);
	if (test->duration)
	    cJSON_AddNumberToObject(j, "time", test->duration);
	if (test->settings->bytes)
	    cJSON_AddNumberToObject(j, "num", test->settings->bytes);
	if (test->settings->blocks)
	    cJSON_AddNumberToObject(j, "blockcount", test->settings->blocks);
	if (test->settings->mss)
	    cJSON_AddNumberToObject(j, "MSS", test->settings->mss);
	if (test->no_delay)
	    cJSON_AddTrueToObject(j, "nodelay");
	cJSON_AddNumberToObject(j, "parallel", test->num_streams);
	if (test->reverse)
	    cJSON_AddTrueToObject(j, "reverse");
	if (test->settings->socket_bufsize)
	    cJSON_AddNumberToObject(j, "window", test->settings->socket_bufsize);
	if (test->settings->blksize)
	    cJSON_AddNumberToObject(j, "len", test->settings->blksize);
	if (test->settings->rate)
	    cJSON_AddNumberToObject(j, "bandwidth", test->settings->rate);
	if (test->settings->fqrate)
	    cJSON_AddNumberToObject(j, "fqrate", test->settings->fqrate);
	if (test->settings->pacing_timer)
	    cJSON_AddNumberToObject(j, "pacing_timer", test->settings->pacing_timer);
	if (test->settings->burst)
	    cJSON_AddNumberToObject(j, "burst", test->settings->burst);
	if (test->settings->tos)
	    cJSON_AddNumberToObject(j, "TOS", test->settings->tos);
	if (test->title)
	    cJSON_AddStringToObject(j, "title", test->title);
	if (test->congestion)
	    cJSON_AddStringToObject(j, "congestion", test->congestion);
	if (test->congestion_used)
	    cJSON_AddStringToObject(j, "congestion_used", test->congestion_used);
	if (test->get_server_output)
	    cJSON_AddNumberToObject(j, "get_server_output", iperf_get_test_get_server_output(test));
	if (test->udp_counters_64bit)
	    cJSON_AddNumberToObject(j, "udp_counters_64bit", iperf_get_test_udp_counters_64bit(test));

	if (test->debug) {
	    printf("send_parameters:\n%s\n", cJSON_Print(j));
	}

	if (JSON_write(test->ctrl_sck, j) < 0) {
	    r = -1;
	}
	cJSON_Delete(j);
    }
    return r;
}

/*************************************************************/

static int
get_parameters(struct iperf_test *test)
{
    int r = 0;
    cJSON *j;
    cJSON *j_p;

    j = JSON_read(test->ctrl_sck);
    if (j == NULL) {
        r = -1;
    } else {
	if (test->debug) {
	    printf("get_parameters:\n%s\n", cJSON_Print(j));
	}

	if ((j_p = cJSON_GetObjectItem(j, "tcp")) != NULL)
	    set_protocol(test, Ptcp);
	if ((j_p = cJSON_GetObjectItem(j, "udp")) != NULL)
	    set_protocol(test, Pudp);
	if ((j_p = cJSON_GetObjectItem(j, "server_affinity")) != NULL)
	    test->server_affinity = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "time")) != NULL)
	    test->duration = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "num")) != NULL)
	    test->settings->bytes = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "blockcount")) != NULL)
	    test->settings->blocks = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "MSS")) != NULL)
	    test->settings->mss = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "nodelay")) != NULL)
	    test->no_delay = 1;
	if ((j_p = cJSON_GetObjectItem(j, "parallel")) != NULL)
	    test->num_streams = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "reverse")) != NULL)
	    iperf_set_test_reverse(test, 1);
	if ((j_p = cJSON_GetObjectItem(j, "window")) != NULL)
	    test->settings->socket_bufsize = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "len")) != NULL)
	    test->settings->blksize = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "bandwidth")) != NULL)
	    test->settings->rate = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "fqrate")) != NULL)
	    test->settings->fqrate = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "pacing_timer")) != NULL)
	    test->settings->pacing_timer = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "burst")) != NULL)
	    test->settings->burst = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "TOS")) != NULL)
	    test->settings->tos = j_p->valueint;
	if ((j_p = cJSON_GetObjectItem(j, "title")) != NULL)
	    test->title = strdup(j_p->valuestring);
	if ((j_p = cJSON_GetObjectItem(j, "congestion")) != NULL)
	    test->congestion = strdup(j_p->valuestring);
	if ((j_p = cJSON_GetObjectItem(j, "congestion_used")) != NULL)
	    test->congestion_used = strdup(j_p->valuestring);
	if ((j_p = cJSON_GetObjectItem(j, "get_server_output")) != NULL)
	    iperf_set_test_get_server_output(test, 1);
	if ((j_p = cJSON_GetObjectItem(j, "udp_counters_64bit")) != NULL)
	    iperf_set_test_udp_counters_64bit(test, 1);
	if (test->sender && test->protocol->id == Ptcp)
	    test->sender_has_retransmits = 1;
	cJSON_Delete(j);
    }
    return r;
}


int iperf_printf(struct iperf_test *test, const char* format, ...)
{
    va_list argp;
    int r = -1;

    if (test->role == 'c') {
        va_start(argp, format);
        r = vfprintf(test->outfile, format, argp);
        va_end(argp);
    } else if (test->role == 's') {
        char linebuffer[1024];
        va_start(argp, format);
        r = vsnprintf(linebuffer, sizeof(linebuffer), format, argp);
        va_end(argp);
        fprintf(test->outfile, "%s", linebuffer);
    }

    return r;
}

/*************************************************************/

static int
JSON_write(int fd, cJSON *json)
{
    uint32_t hsize, nsize;
    char *str;
    int r = 0;

    str = cJSON_PrintUnformatted(json);
    if (str == NULL) {
	    r = -1;
    } else {
        hsize = strlen(str);
        nsize = htonl(hsize);
        if (Nwrite(fd, (char*) &nsize, sizeof(nsize), Ptcp) < 0) {
            r = -1;
        } else {
            if (Nwrite(fd, str, hsize, Ptcp) < 0)
            r = -1;
        }
        free(str);
    }
    return r;
}

/*************************************************************/

static cJSON *
JSON_read(int fd)
{
    uint32_t hsize, nsize;
    char *str;
    cJSON *json = NULL;
    int rc;

    /*
     * Read a four-byte integer, which is the length of the JSON to follow.
     * Then read the JSON into a buffer and parse it.  Return a parsed JSON
     * structure, NULL if there was an error.
     */
    if (Nread(fd, (char*) &nsize, sizeof(nsize), Ptcp) >= 0) {
        hsize = ntohl(nsize);
        /* Allocate a buffer to hold the JSON */
        str = (char *) calloc(sizeof(char), hsize+1);	/* +1 for trailing null */
        if (str != NULL) {
            rc = Nread(fd, str, hsize, Ptcp);
            if (rc >= 0) {
                if (rc == hsize) {
                    json = cJSON_Parse(str);
                }
                else {
                    printf("WARNING:  Size of data read does not correspond to offered length\n");
                }
            }
        }
        free(str);
    }
    return json;
}