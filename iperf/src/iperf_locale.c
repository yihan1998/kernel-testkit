#include "iperf_locale.h"

/* -------------------------------------------------------------------
 * usage
 * ------------------------------------------------------------------- */

const char usage_shortstr[] = "Usage: iperf3 [-s|-c host] [options]\n"
                           "Try `iperf3 --help' for more information.\n";

const char usage_longstr[] = "Usage: iperf3 [-s|-c host] [options]\n"
                           "       iperf3 [-h|--help]\n\n"
                           "Server or Client:\n"
                           "  -p, --port      #         server port to listen on/connect to\n"
                           "  -f, --format   [kmgtKMGT] format to report: Kbits, Mbits, Gbits, Tbits\n"
                           "  -i, --interval  #         seconds between periodic throughput reports\n"
                           "  -F, --file name           xmit/recv the specified file\n"
                           "  -A, --affinity n/n,m      set CPU affinity\n"
                           "  -B, --bind      <host>    bind to a specific interface\n"
                           "  -V, --verbose             more detailed output\n"
                           "  -J, --json                output in JSON format\n"
                           "  --logfile f               send output to a log file\n"
                           "  --forceflush              force flushing output at every interval\n"
                           "  -d, --debug               emit debugging output\n"
                           "  -h, --help                show this message and quit\n"
                           "Server specific:\n"
                           "  -s, --server              run in server mode\n"
                           "  -D, --daemon              run the server as a daemon\n"
                           "  -I, --pidfile file        write PID file\n"
                           "  -1, --one-off             handle one client connection then exit\n"
                           "Client specific:\n"
                           "  -c, --client    <host>    run in client mode, connecting to <host>\n"
                           "  -u, --udp                 use UDP rather than TCP\n"
                           "  --connect-timeout #       timeout for control connection setup (ms)\n"
                           "  -b, --bitrate #[KMG][/#]  target bitrate in bits/sec (0 for unlimited)\n"
                           "                            (default %d Mbit/sec for UDP, unlimited for TCP)\n"
                           "                            (optional slash and packet count for burst mode)\n"
			   "  --pacing-timer #[KMG]     set the timing for pacing, in microseconds (default 1000)\n"
                           "  -t, --time      #         time in seconds to transmit for (default %d secs)\n"
                           "  -n, --bytes     #[KMG]    number of bytes to transmit (instead of -t)\n"
                           "  -k, --blockcount #[KMG]   number of blocks (packets) to transmit (instead of -t or -n)\n"
                           "  -l, --len       #[KMG]    length of buffer to read or write\n"
			   "                            (default %d KB for TCP, dynamic or %d for UDP)\n"
                           "  --cport         <port>    bind to a specific client port (TCP and UDP, default: ephemeral port)\n"
                           "  -P, --parallel  #         number of parallel client streams to run\n"
                           "  -R, --reverse             run in reverse mode (server sends, client receives)\n"
                           "  -w, --window    #[KMG]    set window size / socket buffer size\n"
                           "  -M, --set-mss   #         set TCP/SCTP maximum segment size (MTU - 40 bytes)\n"
                           "  -N, --no-delay            set TCP/SCTP no delay, disabling Nagle's Algorithm\n"
                           "  -4, --version4            only use IPv4\n"
                           "  -6, --version6            only use IPv6\n"
                           "  -S, --tos N               set the IP type of service, 0-255\n"
                           "  --dscp N or --dscp val    set the IP dscp value, either 0-63 or symbolic\n"
                           "  -Z, --zerocopy            use a 'zero copy' method of sending data\n"
                           "  -T, --title str           prefix every output line with this string\n"
                           "  --get-server-output       get results from server\n"
                           "  --udp-counters-64bit      use 64-bit counters in UDP test packets\n"
			   "\n"
                           "[KMG] indicates options that support a K/M/G suffix for kilo-, mega-, or giga-\n"
			   "\n"
			   ;

/* -------------------------------------------------------------------
 * settings
 * ------------------------------------------------------------------- */

const char seperator_line[] =
"------------------------------------------------------------\n";

const char server_port[] =
"Server listening on %s port %d\n";

const char client_port[] =
"Client connecting to %s, %s port %d\n";

const char bind_address[] =
"Binding to local address %s\n";

const char bind_port[] =
"Binding to local port %s\n";

const char multicast_ttl[] =
"Setting multicast TTL to %d\n";

const char join_multicast[] =
"Joining multicast group  %s\n";

const char client_datagram_size[] =
"Sending %d byte datagrams\n";

const char server_datagram_size[] =
"Receiving %d byte datagrams\n";

const char tcp_window_size[] =
"TCP window size";

const char udp_buffer_size[] =
"UDP buffer size";

const char window_default[] =
"(default)";

const char wait_server_threads[] =
"Waiting for server threads to complete. Interrupt again to force quit.\n";

const char test_start_time[] =
"Starting Test: protocol: %s, %d streams, %d byte blocks, %d second test, tos %d\n";

const char test_start_bytes[] =
"Starting Test: protocol: %s, %d streams, %d byte blocks, %llu bytes to send, tos %d\n";

const char test_start_blocks[] =
"Starting Test: protocol: %s, %d streams, %d byte blocks, %d blocks to send, tos %d\n";


/* -------------------------------------------------------------------
 * reports
 * ------------------------------------------------------------------- */

const char report_time[] =
"Time: %s\n";

const char report_connecting[] =
"Connecting to host %s, port %d\n";

const char report_authetication_successed[] =
"Authentication successed for user '%s' ts %ld\n";

const char report_authetication_failed[] =
"Authentication failed for user '%s' ts %ld\n";

const char report_reverse[] =
"Reverse mode, remote host %s is sending\n";

const char report_accepted[] =
"Accepted connection from %s, port %d\n";

const char report_cookie[] =
"      Cookie: %s\n";

const char report_connected[] =
"[%3d] local %s port %d connected to %s port %d\n";

const char report_window[] =
"TCP window size: %s\n";

const char report_autotune[] =
"Using TCP Autotuning\n";

const char report_diskfile[] =
"        Sent %s / %s (%d%%) of %s\n";

const char report_done[] =
"iperf Done.\n";

const char report_read_lengths[] =
"[%3d] Read lengths occurring in more than 5%% of reads:\n";

const char report_read_length_times[] =
"[%3d] %5d bytes read %5d times (%.3g%%)\n";

const char report_bw_header[] =
"[ ID] Interval           Transfer     Bitrate\n";

const char report_bw_retrans_header[] =
"[ ID] Interval           Transfer     Bitrate         Retr\n";

const char report_bw_retrans_cwnd_header[] =
"[ ID] Interval           Transfer     Bitrate         Retr  Cwnd\n";

const char report_bw_udp_header[] =
"[ ID] Interval           Transfer     Bitrate         Jitter    Lost/Total Datagrams\n";

const char report_bw_udp_sender_header[] =
"[ ID] Interval           Transfer     Bitrate         Total Datagrams\n";

const char report_bw_format[] =
"[%3d] %6.2f-%-6.2f sec  %ss  %ss/sec                  %s\n";

const char report_bw_retrans_format[] =
"[%3d] %6.2f-%-6.2f sec  %ss  %ss/sec  %3u             %s\n";

const char report_bw_retrans_cwnd_format[] =
"[%3d] %6.2f-%-6.2f sec  %ss  %ss/sec  %3u   %ss       %s\n";

const char report_bw_udp_format[] =
"[%3d] %6.2f-%-6.2f sec  %ss  %ss/sec  %5.3f ms  %d/%d (%.2g%%)  %s\n";

const char report_bw_udp_sender_format[] =
"[%3d] %6.2f-%-6.2f sec  %ss  %ss/sec  %d  %s\n";

const char report_summary[] =
"Test Complete. Summary Results:\n";

const char report_sum_bw_format[] =
"[SUM] %6.2f-%-6.2f sec  %ss  %ss/sec                  %s\n";

const char report_sum_bw_retrans_format[] =
"[SUM] %6.2f-%-6.2f sec  %ss  %ss/sec  %3d             %s\n";

const char report_sum_bw_udp_format[] =
"[SUM] %6.2f-%-6.2f sec  %ss  %ss/sec  %5.3f ms  %d/%d (%.2g%%)  %s\n";

const char report_sum_bw_udp_sender_format[] =
"[SUM] %6.2f-%-6.2f sec  %ss  %ss/sec  %d  %s\n";

const char report_bw_separator[] =
"- - - - - - - - - - - - - - - - - - - - - - - - -\n";

const char report_outoforder[] =
"[%3d] %4.1f-%4.1f sec  %d datagrams received out-of-order\n";

const char report_sum_outoforder[] =
"[SUM] %4.1f-%4.1f sec  %d datagrams received out-of-order\n";

const char report_peer[] =
"[%3d] local %s port %u connected with %s port %u\n";

const char report_mss_unsupported[] =
"[%3d] MSS and MTU size unknown (TCP_MAXSEG not supported by OS?)\n";

const char report_mss[] =
"[%3d] MSS size %d bytes (MTU %d bytes, %s)\n";

const char report_datagrams[] =
"[%3d] Sent %d datagrams\n";

const char report_sum_datagrams[] =
"[SUM] Sent %d datagrams\n";

const char server_reporting[] =
"[%3d] Server Report:\n";

const char reportCSV_peer[] =
"%s,%u,%s,%u";

const char report_cpu[] =
"CPU Utilization: %s/%s %.1f%% (%.1f%%u/%.1f%%s), %s/%s %.1f%% (%.1f%%u/%.1f%%s)\n";

const char report_local[] = "local";
const char report_remote[] = "remote";
const char report_sender[] = "sender";
const char report_receiver[] = "receiver";
const char report_sender_not_available_format[] = "[%3d] (sender statistics not available)\n";
const char report_receiver_not_available_format[] = "[%3d] (receiver statistics not available)\n";

#if defined(linux)
const char report_tcpInfo[] =
"event=TCP_Info CWND=%u SND_SSTHRESH=%u RCV_SSTHRESH=%u UNACKED=%u SACK=%u LOST=%u RETRANS=%u FACK=%u RTT=%u REORDERING=%u\n";
#endif
#if defined(__FreeBSD__)
const char report_tcpInfo[] =
"event=TCP_Info CWND=%u RCV_WIND=%u SND_SSTHRESH=%u RTT=%u\n";
#endif
#if defined(__NetBSD__)
const char report_tcpInfo[] =
"event=TCP_Info CWND=%u RCV_WIND=%u SND_SSTHRESH=%u RTT=%u\n";
#endif


#ifdef HAVE_QUAD_SUPPORT
#ifdef HAVE_PRINTF_QD
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%qd,%qd\n";

const char reportCSV_bw_udp_format[] =
"%s,%s,%d,%.1f-%.1f,%qd,%qd,%.3f,%d,%d,%.3f,%d\n";
#else // HAVE_PRINTF_QD
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%lld,%lld\n";

const char reportCSV_bw_udp_format[] =
"%s,%s,%d,%.1f-%.1f,%lld,%lld,%.3f,%d,%d,%.3f,%d\n";
#endif // HAVE_PRINTF_QD
#else // HAVE_QUAD_SUPPORT
#ifdef WIN32
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%I64d,%I64d\n";

const char reportCSV_bw_udp_format[] =
"%s,%s,%d,%.1f-%.1f,%I64d,%I64d,%.3f,%d,%d,%.3f,%d\n";
#else
const char reportCSV_bw_format[] =
"%s,%s,%d,%.1f-%.1f,%d,%d\n";

const char reportCSV_bw_udp_format[] =
"%s,%s,%d,%.1f-%.1f,%d,%d,%.3f,%d,%d,%.3f,%d\n";
#endif //WIN32
#endif //HAVE_QUAD_SUPPORT
/* -------------------------------------------------------------------
 * warnings
 * ------------------------------------------------------------------- */

const char warn_window_requested[] =
" (WARNING: requested %s)";

const char warn_window_small[] =
"WARNING: TCP window size set to %d bytes. A small window size\n\
will give poor performance. See the Iperf documentation.\n";

const char warn_delay_large[] =
"WARNING: delay too large, reducing from %.1f to 1.0 seconds.\n";

const char warn_no_pathmtu[] =
"WARNING: Path MTU Discovery may not be enabled.\n";

const char warn_no_ack[]=
"[%3d] WARNING: did not receive ack of last datagram after %d tries.\n";

const char warn_ack_failed[]=
"[%3d] WARNING: ack of last datagram failed after %d tries.\n";

const char warn_fileopen_failed[]=
"WARNING: Unable to open file stream for transfer\n\
Using default data stream. \n";

const char unable_to_change_win[]=
"WARNING: Unable to change the window size\n";

const char opt_estimate[]=
"Optimal Estimate\n";

const char report_interval_small[] =
"WARNING: interval too small, increasing from %3.2f to 0.5 seconds.\n";

const char warn_invalid_server_option[] =
"WARNING: option -%c is not valid for server mode\n";

const char warn_invalid_client_option[] =
"WARNING: option -%c is not valid for client mode\n";

const char warn_invalid_compatibility_option[] =
"WARNING: option -%c is not valid in compatibility mode\n";

const char warn_implied_udp[] =
"WARNING: option -%c implies udp testing\n";

const char warn_implied_compatibility[] =
"WARNING: option -%c has implied compatibility mode\n";

const char warn_buffer_too_small[] =
"WARNING: the UDP buffer was increased to %d for proper operation\n";

const char warn_invalid_single_threaded[] =
"WARNING: option -%c is not valid in single threaded versions\n";

const char warn_invalid_report_style[] =
"WARNING: unknown reporting style \"%s\", switching to default\n";

const char warn_invalid_report[] =
"WARNING: unknown reporting type \"%c\", ignored\n valid options are:\n\t exclude: C(connection) D(data) M(multicast) S(settings) V(server) report\n\n";

#ifdef __cplusplus
} /* end extern "C" */
#endif
