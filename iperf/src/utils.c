#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include "utils.h"
#include "iperf.h"

const double KILO_UNIT = 1024.0;
const double MEGA_UNIT = 1024.0 * 1024.0;
const double GIGA_UNIT = 1024.0 * 1024.0 * 1024.0;
const double TERA_UNIT = 1024.0 * 1024.0 * 1024.0 * 1024.0;

const double KILO_RATE_UNIT = 1000.0;
const double MEGA_RATE_UNIT = 1000.0 * 1000.0;
const double GIGA_RATE_UNIT = 1000.0 * 1000.0 * 1000.0;
const double TERA_RATE_UNIT = 1000.0 * 1000.0 * 1000.0 * 1000.0;


/* -------------------------------------------------------------------
 * unit_atof_rate
 *
 * Similar to unit_atof, but uses 10-based rather than 2-based
 * suffixes.
 * ------------------------------------------------------------------- */

double unit_atof_rate(const char *s)
{
	double    n;
	char      suffix = '\0';

    assert(s != NULL);

	/* scan the number and any suffices */
    sscanf(s, "%lf%c", &n, &suffix);

	/* convert according to [Tt Gg Mm Kk] */
	switch    (suffix)
	{
	case 't': case 'T':
	    n *= TERA_RATE_UNIT;
	    break;
	case 'g': case 'G':
	    n *= GIGA_RATE_UNIT;
	    break;
	case 'm': case 'M':
	    n *= MEGA_RATE_UNIT;
	    break;
	case 'k': case 'K':
	    n *= KILO_RATE_UNIT;
	    break;
	default:
	    break;
	}

    return n;
}

