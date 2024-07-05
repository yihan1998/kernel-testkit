#ifndef _IPERF_API_H_
#define _IPERF_API_H_

#ifdef __cplusplus
extern "C" { /* open extern "C" */
#endif

#include <sys/time.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdint.h>

struct iperf_test;

/* short option equivalents, used to support options that only have long form */
#define OPT_SCTP 1
#define OPT_LOGFILE 2
#define OPT_GET_SERVER_OUTPUT 3
#define OPT_UDP_COUNTERS_64BIT 4
#define OPT_CLIENT_PORT 5
#define OPT_NUMSTREAMS 6
#define OPT_FORCEFLUSH 7
#define OPT_NO_FQ_SOCKET_PACING 9 /* UNUSED */
#define OPT_FQ_RATE 10
#define OPT_DSCP 11
#define OPT_CLIENT_USERNAME 12
#define OPT_CLIENT_RSA_PUBLIC_KEY 13
#define OPT_SERVER_RSA_PRIVATE_KEY 14
#define OPT_SERVER_AUTHORIZED_USERS 15
#define OPT_PACING_TIMER 16
#define OPT_CONNECT_TIMEOUT 17

/* states */
#define TEST_START 1
#define TEST_RUNNING 2
#define RESULT_REQUEST 3
#define TEST_END 4
#define STREAM_BEGIN 5
#define STREAM_RUNNING 6
#define STREAM_END 7
#define ALL_STREAMS_END 8
#define PARAM_EXCHANGE 9
#define CREATE_STREAMS 10
#define SERVER_TERMINATE 11
#define CLIENT_TERMINATE 12
#define EXCHANGE_RESULTS 13
#define DISPLAY_RESULTS 14
#define IPERF_START 15
#define IPERF_DONE 16
#define ACCESS_DENIED (-1)
#define SERVER_ERROR (-2)

/* Getter routines for some fields inside iperf_test. */
int	iperf_get_verbose( struct iperf_test* ipt );
int	iperf_get_control_socket( struct iperf_test* ipt );
int	iperf_get_test_omit( struct iperf_test* ipt );
int	iperf_get_test_duration( struct iperf_test* ipt );
char	iperf_get_test_role( struct iperf_test* ipt );
int	iperf_get_test_reverse( struct iperf_test* ipt );
int	iperf_get_test_blksize( struct iperf_test* ipt );
FILE*	iperf_get_test_outfile( struct iperf_test* ipt );
uint64_t iperf_get_test_rate( struct iperf_test* ipt );
int     iperf_get_test_burst( struct iperf_test* ipt );
int	iperf_get_test_socket_bufsize( struct iperf_test* ipt );
double	iperf_get_test_reporter_interval( struct iperf_test* ipt );
double	iperf_get_test_stats_interval( struct iperf_test* ipt );
int	iperf_get_test_num_streams( struct iperf_test* ipt );
int	iperf_get_test_server_port( struct iperf_test* ipt );
char*	iperf_get_test_server_hostname( struct iperf_test* ipt );
char*	iperf_get_test_template( struct iperf_test* ipt );
int	iperf_get_test_protocol_id( struct iperf_test* ipt );
int	iperf_get_test_json_output( struct iperf_test* ipt );
char*	iperf_get_test_json_output_string ( struct iperf_test* ipt );
int	iperf_get_test_zerocopy( struct iperf_test* ipt );
int	iperf_get_test_get_server_output( struct iperf_test* ipt );
char*	iperf_get_test_bind_address ( struct iperf_test* ipt );
int	iperf_get_test_udp_counters_64bit( struct iperf_test* ipt );
int	iperf_get_test_one_off( struct iperf_test* ipt );

/* Setter routines for some fields inside iperf_test. */
void	iperf_set_verbose( struct iperf_test* ipt, int verbose );
void	iperf_set_control_socket( struct iperf_test* ipt, int ctrl_sck );
void	iperf_set_test_omit( struct iperf_test* ipt, int omit );
void	iperf_set_test_duration( struct iperf_test* ipt, int duration );
void	iperf_set_test_reporter_interval( struct iperf_test* ipt, double reporter_interval );
void	iperf_set_test_stats_interval( struct iperf_test* ipt, double stats_interval );
void	iperf_set_test_state( struct iperf_test* ipt, signed char state );
void	iperf_set_test_blksize( struct iperf_test* ipt, int blksize );
void	iperf_set_test_rate( struct iperf_test* ipt, uint64_t rate );
void	iperf_set_test_burst( struct iperf_test* ipt, int burst );
void	iperf_set_test_server_port( struct iperf_test* ipt, int server_port );
void	iperf_set_test_socket_bufsize( struct iperf_test* ipt, int socket_bufsize );
void	iperf_set_test_num_streams( struct iperf_test* ipt, int num_streams );
void	iperf_set_test_role( struct iperf_test* ipt, char role );
void	iperf_set_test_server_hostname( struct iperf_test* ipt, char* server_hostname );
void    iperf_set_test_template( struct iperf_test *ipt, char *tmp_template );
void	iperf_set_test_reverse( struct iperf_test* ipt, int reverse );
void	iperf_set_test_json_output( struct iperf_test* ipt, int json_output );
int	    iperf_has_zerocopy( void );
void	iperf_set_test_zerocopy( struct iperf_test* ipt, int zerocopy );
void	iperf_set_test_get_server_output( struct iperf_test* ipt, int get_server_output );
void	iperf_set_test_bind_address( struct iperf_test* ipt, char *bind_address );
void	iperf_set_test_udp_counters_64bit( struct iperf_test* ipt, int udp_counters_64bit );
void	iperf_set_test_one_off( struct iperf_test* ipt, int one_off );

/* default settings */
#define Ptcp SOCK_STREAM
#define Pudp SOCK_DGRAM
#define DEFAULT_UDP_BLKSIZE 1460 /* default is dynamically set, else this */
#define DEFAULT_TCP_BLKSIZE (128 * 1024)  /* default read/write block size */
#define DEFAULT_SCTP_BLKSIZE (64 * 1024)

int iperf_create_epoll(struct iperf_test *ipt);
int iperf_create_timerfd(struct iperf_test *ipt);

struct iperf_test *iperf_new_test(void);
int iperf_defaults(struct iperf_test * testp);

struct protocol *get_protocol(struct iperf_test *, int);
int set_protocol(struct iperf_test *, int);

/* Client routines. */
int iperf_run_client(struct iperf_test *);

/* Server routines. */
int iperf_run_server(struct iperf_test *);

int iperf_set_send_state(struct iperf_test *test, signed char state);
int iperf_printf(struct iperf_test *test, const char *format, ...) __attribute__ ((format(printf,2,3)));

int iperf_exchange_parameters(struct iperf_test * test);

#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif  /* _IPERF_API_H_ */
