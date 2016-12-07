#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <maxminddb.h>
#include "vmod_geo.h"

/*
 * This is a simple tool to validate a maxmind db file. We test validity by opening
 * the maxmind db file and then verifying 4.4.4.4 comes back with US as the country.
 * That may not be valid in the future, but it's currently one of Googles name
 * servers.
 */

void usage();
void usage() {
    fprintf(stdout, "\nPROGRAM: maxmind\n\n");
    fprintf(stdout, "maxmind is a simple utility to valdate the MaxMind GeoIP2 database file.\n\n");
    fprintf(stdout, "PARAMETERS:\n\n");
    fprintf(stdout, "-h : prints help.\n");
    fprintf(stdout, "-m <path to maxmind db file> : location to valid maxmind database file.\n");
    fprintf(stdout, "-t <ip> : US based IP address. Default is 4.4.4.4\n");
    fprintf(stdout, "-s : if set will silence output. otherwise this program writes to stdout.\n\n");
}


char * MMDB_PATH = NULL;
int verbose = 1;

int main(int argc, char **argv) {
    int c = 0;
    const char *testip = "4.4.4.4";
    while ((c = getopt(argc, argv, "hm:st:?")) != -1) {
        switch (c) {
        case 'h' :
            usage();
            return 0;
        case 'm' :
            MMDB_PATH = optarg;
            break;
        case 's' :
            verbose = 0;
            break;
        case 't':
            testip = optarg;
            break;
        case '?' : // make it here if we we get an invalid option
            usage();
            return 0;
        default :
            fprintf (stdout,
                     "Unknown option character... 1\\x%x'.\n",
                     optopt);
            return 1;
        }
    }

    if (c == 1) {
        usage();
        return 1;
    }
    if (MMDB_PATH == NULL) {
        usage();
        return 1;
    }

    MMDB_s mmdb_handle;
    int mmdb_baddb = MMDB_open(MMDB_PATH, MMDB_MODE_MMAP, &mmdb_handle);
    if (mmdb_baddb) {
        if (verbose) {
            fprintf(stderr, "Invalid maxmind db file: %s\n", MMDB_PATH);
        }
        return 1;
    }

    const char *country_lookup[] = {"country", "iso_code", NULL};
    const char *expected = "US";

    const char *actual = geo_lookup(&mmdb_handle, testip, country_lookup);
    if (strncmp(actual, expected, 2)) {
        if (verbose) {
            fprintf(stderr, "Invalid mamxind db file. Bad actual for %s - should be 'US' but was '%s'\n", testip, actual);
        }
        return 1;
    }
    MMDB_close(&mmdb_handle);
    if (verbose) {
        fprintf(stdout, "Valid maxmind db file.\n");
    }
    return 0;
}
