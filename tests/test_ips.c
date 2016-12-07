#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <maxminddb.h>
#include "vmod_geo.h"

// this is a command line program to process a file of ipaddresses.
// you can specify -i file_of_ips.txt - I assume a \n terminated file
// and did little to verify you don't screw that up.
// -m CityIP2.mmdb file
// -d file_of_errors.csv - for all instances where there were 3 or more - in the lookup result
// -o file_of_ip_lookup.csv - file to see what we got back from maxmind
// again, this is a utility script for diagnosing what comes back from
// maxmind

static char* MMDB_CITY_PATH = MAX_CITY_DB;

char *ipdatafile, *outputfile, *debugfile;


int main(int argc, char **argv) {
	int c, errno = 0;

	while ((c = getopt (argc, argv, "i:m:d:o:")) != -1)
		switch (c) {
		case 'm' :
			MMDB_CITY_PATH = optarg;
			break;
		case 'i' :
			ipdatafile = optarg;
			break;
		case 'd' :
			debugfile = optarg;
			break;
		case 'o' :
			outputfile = optarg;
			break;
		case '?' :
			if (optopt == 'm' || optopt == 'i' || optopt == 'o' || optopt == 'd')
				fprintf(stderr, "Option -%c requires you give the location of the file to use.\n", optopt);
			else if (isprint (optopt))
			   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			 else
			   fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
		default :
			return 1;
		}

	if (MMDB_CITY_PATH == NULL) {
		fprintf(stderr, "I need you to tell me where the mmdb file is with -m\n");
		return 1;
	}

	if (ipdatafile == NULL) {
		fprintf(stderr, "I need you to tell me where the file with the ip address is with -i\n");
		return 1;
	}

	if (debugfile == NULL) {
		fprintf(stderr, "I need you to tell me where to write the debug data to with -d <debug/path/file>\n");
		return 1;
	}

	MMDB_s mmdb_handle;
	int mmdb_baddb = open_mmdb(&mmdb_handle);
	if (mmdb_baddb) {
		fprintf(stderr, "There was a problem opening %s\n", MMDB_CITY_PATH);
		return 1;
	}

	FILE *f = fopen(ipdatafile, "r");
	if (f == NULL) {
		fprintf(stderr, "Unable to open the ip file: %s\n", ipdatafile);
		return errno;
	}

	// this will make sure the debug file is clean.
	FILE *debug = fopen(debugfile, "w+");
	if (debug == NULL) {
		fprintf(stderr,"Unable to open the debug file: %s\n", ipdatafile);
		fclose(f);
		return errno;
	}

	// dump function will open this file when we call it.
	fclose(debug);

	FILE *output = NULL;
	if (outputfile != NULL) {
		output = fopen(outputfile, "w+");
		if (output == NULL) {
			fprintf(stderr, "Unable to open the output file: %s\n", ipdatafile);
			fclose(f);
			return errno;
		}

	}
	char *ipaddr;
	ipaddr = calloc( 255, sizeof(char) );
	do {

		ipaddr = fgets(ipaddr,255,f);
		if (ipaddr == NULL)
			break;

		// consume the new line
		char *c = strrchr(ipaddr,'\n');
		if (c != NULL) {
			*c = '\0';
		}
		c = geo_lookup_weather(&mmdb_handle, ipaddr, 0);
		int index, end, count;

		count = 0;
		end   = strlen(c);
		// if there's more than 2 - in the result, let's dump it to file
		// for closer examination.
		for (index = 0; index < end; ++index) {
			if (c[index] == '-')
				count+=1;
			if (count > 2) {
				dump_failed_lookup(&mmdb_handle, ipaddr, debugfile);
				break;
			}
		}
		if (output == NULL)
			fprintf(stdout, "%s,%s\n", ipaddr, c);
		else
			fprintf(output, "%s,%s\n", ipaddr, c);

	} while (ipaddr != NULL);
	free(ipaddr);
	close_mmdb(&mmdb_handle);
	fclose(f);
}
