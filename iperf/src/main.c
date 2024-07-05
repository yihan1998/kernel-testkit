#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "utils.h"
#include "iperf.h"
#include "iperf_api.h"

int iperf_parse_arguments(struct iperf_test *test, int argc, char **argv) {
    static struct option longopts[] =
    {
        {"port", required_argument, NULL, 'p'},
        {"format", required_argument, NULL, 'f'},
        {"verbose", no_argument, NULL, 'V'},
        {"json", no_argument, NULL, 'J'},
        {"version", no_argument, NULL, 'v'},
        {"server", no_argument, NULL, 's'},
        {"client", required_argument, NULL, 'c'},
        {"udp", no_argument, NULL, 'u'},
        {"bitrate", required_argument, NULL, 'b'},
        {"bandwidth", required_argument, NULL, 'b'},
        {"time", required_argument, NULL, 't'},
        {"bytes", required_argument, NULL, 'n'},
        {"blockcount", required_argument, NULL, 'k'},
        {"length", required_argument, NULL, 'l'},
        {"parallel", required_argument, NULL, 'P'},
        {"reverse", no_argument, NULL, 'R'},
        {"window", required_argument, NULL, 'w'},
        {"bind", required_argument, NULL, 'B'},
        {"cport", required_argument, NULL, OPT_CLIENT_PORT},
        {"set-mss", required_argument, NULL, 'M'},
        {"no-delay", no_argument, NULL, 'N'},
        {"zerocopy", no_argument, NULL, 'Z'},
        {"affinity", required_argument, NULL, 'A'},
        {"pidfile", required_argument, NULL, 'I'},
        {"logfile", required_argument, NULL, OPT_LOGFILE},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    int flag;

    while ((flag = getopt_long(argc, argv, "p:f:D1VJvsc:ub:t:n:k:l:P:Rw:B:M:N46S:L:ZO:F:A:T:C:dI:hX:", longopts, NULL)) != -1) {
        switch (flag) {
            case 'p':
                test->server_port = atoi(optarg);
                break;
            case 'f':
                test->settings->unit_format = *optarg;
                if (test->settings->unit_format == 'k' ||
                    test->settings->unit_format == 'K' ||
                    test->settings->unit_format == 'm' ||
                    test->settings->unit_format == 'M' ||
                    test->settings->unit_format == 'g' ||
                    test->settings->unit_format == 'G' ||
                    test->settings->unit_format == 't' ||
                    test->settings->unit_format == 'T') {
                    break;
                } else {
                    return -1;
                }
                break;
            case 'V':
                test->verbose = 1;
                break;
            case 's':
                if (test->role == 'c') {
                    printf("Already set as client\n");
                    return -1;
                }
		        iperf_set_test_role(test, 's');
                break;
            case 'c':
                if (test->role == 's') {
                    printf("Already set as server\n");
                    return -1;
                }
                iperf_set_test_role(test, 'c');
                iperf_set_test_server_hostname(test, optarg);
                break;
            case 'u':
                set_protocol(test, Pudp);
                break;
            case 'b':
                test->settings->rate = unit_atof_rate(optarg);
                break;
            case 't':
                test->duration = atoi(optarg);
                break;
            case OPT_CLIENT_PORT:
                test->bind_port = atoi(optarg);
                break;
            case 'M':
                test->settings->mss = atoi(optarg);
                if (test->settings->mss > MAX_MSS) {
                    printf("Exceed max MSS!\n");
                    return -1;
                }
                break;
            default:
                break;
        }
    }

    if ((test->role != 'c') && (test->role != 's')) {
        printf("No role specified!");
        return -1;
    }

    return 0;
}

/**************************************************************************/
static void __attribute__ ((noreturn)) sighandler(int sig)
{
    printf("End\n");
    exit(0);
}

static int run(struct iperf_test *test)
{
    int rc;

    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);
    signal(SIGHUP, sighandler);

    switch (test->role) {
        case 's':
    		rc = iperf_run_server(test);
            if (rc < 0) {
                printf("error - %s\n", strerror(errno));
            }
            break;
        case 'c':
	        rc = iperf_run_client(test);
            if (rc < 0) {
                printf("error - %s\n", strerror(errno));
            }
            break;
    }

    return 0;
}

int main(int argc, char **argv)
{
    struct iperf_test * test;
    test = iperf_new_test();
    if (!test) {
        printf("create new test error - %s", strerror(errno));
        return -1;
    }

    iperf_defaults(test);	/* sets defaults */
    iperf_parse_arguments(test, argc, argv);

    if (run(test) < 0) {
        printf("error - %s", strerror(errno));
    }

    return 0;
}