#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <float.h>
#include <maxminddb.h>
#include "vmod_geo.h"

//**********************************************************************
// NOTICE!! All functions that return pointers use calloc so you must
// free the return values.
//**********************************************************************

static char *MMDB_CITY_PATH       = MAX_CITY_DB;
static char *DEFAULT_WEATHER_CODE = "New YorkNYUS";
static char *DEFAULT_LOCATION     = "{\"response_code\":404}";
static char *ERROR_LOCATION       = "{\"response_code\":500}";
static char *DEFAULT_TIMEZONE     = "{\"timezone\":\"America/New_York\"}";
static char *DEFAULT_LATLON       = "404";
static char *ERROR_LATLON         = "500";

// close gets called by varnish when then the treads destroyed
void close_mmdb(void *mmdb_handle)
{
	// don't do anything if the db didn't open correctly.
	if (mmdb_handle == NULL) {
		return;
	}
	MMDB_s *handle = (MMDB_s *)mmdb_handle;
	MMDB_close(handle);
}

/**
 * open_mmdb - Open the maxmind db file.
 *
 * @return 0 for success, 1 for failure.
 */
int
open_mmdb(MMDB_s *mmdb_handle)
{
	int mmdb_baddb = MMDB_open(MMDB_CITY_PATH, MMDB_MODE_MMAP, mmdb_handle);
	if (mmdb_baddb != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] MMDB_open: Can't open %s - %s\n",
				MMDB_CITY_PATH, MMDB_strerror(mmdb_baddb));
		if (MMDB_IO_ERROR == mmdb_baddb) {
			fprintf(stderr,
					"[ERROR] MMDB_open: IO error: %s\n",
					strerror(mmdb_baddb));
		}
#endif
		mmdb_handle = NULL;
		return 1;
	}
	return 0;
}

/**
 * geo_lookup takes a handle to the mmdb, an IP address and a lookup path
 * and returns a string of the value or NULL.
 * lookup_path is described in this doc: http://maxmind.github.io/MaxMind-DB/
 */
char *
geo_lookup(MMDB_s *const mmdb_handle, const char *ipstr, const char **lookup_path)
{
	char *data = NULL;
	// Lookup IP in the DB
	int gai_error = 0;
	int mmdb_error = 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &gai_error, &mmdb_error);

	if (0 != gai_error) {
#ifdef DEBUG
		fprintf(stderr,
				"[INFO] Error from MMDB_lookup_string for %s - %s\n\n",
				ipstr, gai_strerror(gai_error));
#endif
		return NULL;
	}

	if (MMDB_SUCCESS != mmdb_error) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] Got an error from libmaxminddb: %s\n\n",
				MMDB_strerror(mmdb_error));
#endif
		return NULL;
	}

	// Parse results
	MMDB_entry_data_s entry_data;

	if (result.found_entry) {
		int status = MMDB_aget_value(&result.entry, &entry_data, lookup_path);

		if (MMDB_SUCCESS != status) {
#ifdef DEBUG
			fprintf(
				stderr,
				"[WARN] Got an error looking up the entry data. Make sure \
 the lookup_path is correct. %s\n",
				MMDB_strerror(status));
#endif
			return NULL;
		}

		if (entry_data.has_data) {
			switch(entry_data.type) {
			case MMDB_DATA_TYPE_UTF8_STRING: {
				data = strndup(entry_data.utf8_string, entry_data.data_size);
				break;
			}
			case MMDB_DATA_TYPE_UINT16: {
				uint16_t num = UINT16_MAX;
				int len      = (int)((ceil(log10(num)))*sizeof(char));
				data         = calloc(sizeof(char), len+1);
				if (data != NULL) {
					snprintf(data, len+1, "%u", entry_data.uint16);
				}
				break;
			}
			default:
#ifdef DEBUG
				fprintf(
					stderr,
					"[WARN] No handler for entry data type (%d) was found\n",
					entry_data.type);
#endif
				break;
			}
		} else {
			return NULL;
		}
	} else {
#ifdef DEBUG
		fprintf(
			stderr,
			"[INFO] No entry for this IP address (%s) was found\n",
			ipstr);
#endif
		return NULL;
	}
	return data;
}

/**
 * Given a valid maxmind result and some entry data, lookup a value.
 *
 * NOTE: You must free() the return value if != NULL
 *
 * @result - pointer to a result after calling MMDB_lookup_string
 * @path - lookup value for MMDB_aget_value
 * @return - NULL if there's a failure or we didn't get the field
 */
char *
get_value(MMDB_lookup_result_s *result, const char **path)
{
	MMDB_entry_data_s entry_data;
	int status = MMDB_aget_value(&(*result).entry, &entry_data, path);
	if (MMDB_SUCCESS != status) {
#ifdef DEBUG
		fprintf(
			stderr,
			"[WARN] MMDB_aget_value got an error looking up the entry data. Make sure you use the correct path - %s\n",
			MMDB_strerror(status));
#endif
		return NULL;
	}
	char *value = NULL;
	if (entry_data.has_data) {
		switch(entry_data.type) {
		case MMDB_DATA_TYPE_UTF8_STRING: {
			value = strndup(entry_data.utf8_string, entry_data.data_size);
			break;
		}
		case MMDB_DATA_TYPE_UINT16: {
			uint16_t num = UINT16_MAX;
			int len      = (int)((ceil(log10(num)))*sizeof(char));
			value        = calloc(sizeof(char), len+1);
			if (value != NULL) {
				snprintf(value, len+1, "%u", entry_data.uint16);
			}
			break;
		}
		case MMDB_DATA_TYPE_DOUBLE: {
			double num = DBL_MAX;
			int len    = (int)((ceil(log10(num)))*sizeof(char));
			len        = len * 2;
			value      = calloc(sizeof(char), len+1);
			if (value != NULL) {
				snprintf(value, len+1, "%lf", entry_data.double_value);
			}
			break;
		}
		case MMDB_DATA_TYPE_BOOLEAN: {
			// i'm assuming true == 1 and false == 0
			value   = calloc(sizeof(char), 2);
			if (value != NULL) {
				snprintf(value, 2, "%d", entry_data.boolean);
			}
			break;
		}
		default:
#ifdef DEBUG
			fprintf(
				stderr,
				"[WARN] MMDB_aget_value: No handler for entry data type (%d) was found. \n",
				entry_data.type);
#endif
			break;
		}
	}
	return value;
}

/**
 * Given a handle to the mmdb file and an IP string, attempt to get the
 * location and format it as json. If use_default is set to true, we will
 * return the DEFAULT_LOCATION if we can't get the city, state or country
 * during the mmdb lookup. A success will return json.
 *
 * @param mmdb_handle - handle to the mmdb file
 * @param ipstr - ip address in string format e.g. 4.4.4.4
 * @param use_default - if true, return default on failures
 * @return string - json e.g.
 *    {"city":"New York","state":"NY","country":"US"}
 *    {"city":"Hanoi","state":"","country":"VN"}
 */
char *
geo_lookup_location(MMDB_s *const mmdb_handle, const char *ipstr)
{
	if (mmdb_handle == NULL || ipstr == NULL) {
		fprintf(stderr, "[WARN] geo vmod given NULL maxmind db handle");
		return strdup(ERROR_LOCATION);
	}

	char *data           = NULL;
	int ip_lookup_failed = 0;
	int db_status        = 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &ip_lookup_failed, &db_status);

	if (ip_lookup_failed) {
#ifdef DEBUG
		fprintf(stderr,
				"[WARN] geo_lookup_location: Error from getaddrinfo for IP: %s Error Message: %s\n",
				ipstr, gai_strerror(ip_lookup_failed));
#endif
		return strdup(ERROR_LOCATION);
	}

	if (db_status != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] geo_lookup_location: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
				MMDB_CITY_PATH,
				MMDB_strerror(db_status));
#endif
		return strdup(ERROR_LOCATION);
	}

	// these varaibles will hold our results
	char *country = NULL;
	char *city    = NULL;
	char *state   = NULL;
	char *lat     = NULL;
	char *lon     = NULL;

	// these are used to extract values from the mmdb
	const char *country_lookup[]	= {"country", "iso_code", NULL};
	const char *city_lookup[]		= {"city", "names", "en", NULL};
	const char *state_lookup[]		= {"subdivisions", "0", "iso_code", NULL};
	const char *lat_lookup[]		= {"location", "latitude", NULL};
	const char *lon_lookup[]		= {"location", "longitude", NULL};

	if (result.found_entry) {

		country = get_value(&result, country_lookup);
		city    = get_value(&result, city_lookup);
		lat		= get_value(&result, lat_lookup);
		lon		= get_value(&result, lon_lookup);

		if (country != NULL && strcmp(country,"US") == 0) {
			state = get_value(&result, state_lookup);
		} else {
			state = strdup("");
		}

		if (country == NULL) {
			country = strdup("");
		}
		if (city == NULL) {
			city = strdup("");
		}
		if (state == NULL) {
			state = strdup("");
		}
		if (lat == NULL) {
			lat = strdup("");
		}
		if (lon == NULL) {
			lon = strdup("");
		}

		size_t chars = strlen(country) + strlen(city) + strlen(state) + strlen(lat) + strlen(lon) + strlen(ipstr);
		const char* format = "{\"city\":\"%s\",\"state\":\"%s\",\"country\":\"%s\",\"lat\":%s,\"lon\":%s,\"ip\":\"%s\",\"response_code\":200}";
		chars += strlen(format);
		chars -= 12; // reduce by the number of %s
		data  =  calloc(sizeof(char), chars+1);
		if (data != NULL) {
			snprintf(data, chars+1, format, city, state, country, lat, lon, ipstr);
		} else {
			data = strdup(ERROR_LOCATION);
		}

	} else {
#ifdef DEBUG
		fprintf(
			stderr,
			"[INFO] No entry for this IP address (%s) was found\n",
			ipstr);
#endif
		size_t chars = strlen(ipstr);
		const char *format = "{\"response_code\":404,\"ip\":\"%s\"}";
		chars += strlen(format);
		chars -= 2;
		data  =  calloc(sizeof(char), chars+1);
		if (data != NULL) {
			snprintf(data, chars+1, format, ipstr);
		} else {
			data = strdup(DEFAULT_LOCATION);
		}
	}

	if (country != NULL) {
		free(country);
	}

	if (city != NULL) {
		free(city);
	}

	if (state != NULL) {
		free(state);
	}

	if (lat != NULL) {
		free(lat);
	}

	if (lon != NULL) {
		free(lon);
	}

	return data;
}

/**
 * Given a handle to the mmdb file and an IP string, attempt to get the
 * timezone and format it as json. If use_default is set to true, we will
 * return the DEFAULT_TIMEZONE if we can't get the data
 * during the mmdb lookup. A success will return json.
 *
 * @param mmdb_handle - handle to the mmdb file
 * @param ipstr - ip address in string format e.g. 4.4.4.4
 * @param use_default - if true, return default on failures
 * @return string - json e.g.
 *     {"timezone":"America/New_York"}
 *     {"timezone":"America/Los_Angeles"}.
 */
char *
geo_lookup_timezone(MMDB_s *const mmdb_handle, const char *ipstr, int use_default)
{
	if (mmdb_handle == NULL || ipstr == NULL) {
		fprintf(stderr, "[WARN] geo vmod given NULL maxmind db handle");
		return strdup(DEFAULT_TIMEZONE);
	}

	char *data = NULL;
	// Lookup IP in the DB
	int ip_lookup_failed = 0;
	int db_status = 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &ip_lookup_failed, &db_status);

	if (ip_lookup_failed) {
#ifdef DEBUG
		fprintf(stderr,
				"[WARN] geo_lookup_timezone: Error from getaddrinfo for IP: %s Error Message: %s\n",
				ipstr, gai_strerror(ip_lookup_failed));
#endif
		// we don't want null, if we're not using default
		if (use_default) {
			return strdup(DEFAULT_TIMEZONE);
		} else {
			return strdup("{}");
		}
	}

	if (db_status != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] geo_lookup_timezone: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
				MMDB_CITY_PATH,
				MMDB_strerror(db_status));
#endif
		if (use_default) {
			return strdup(DEFAULT_TIMEZONE);
		} else {
			return strdup("{}");
		}
	}

	// these varaibles will hold our results
	char *timezone = NULL;

	// these are used to extract values from the mmdb
	const char *timezone_lookup[] = {"location", "time_zone", NULL};

	if (result.found_entry) {
		timezone = get_value(&result, timezone_lookup);

		// we should always return new york
		if (timezone == NULL) {
			data = strdup(DEFAULT_TIMEZONE);
		} else {
			size_t chars = sizeof(char) * strlen(timezone);
			const char *format = "{\"timezone\":\"%s\"}";
			chars += sizeof(char) * (strlen(format) - 2); // less %s
			data = calloc(sizeof(char), chars+1);
			if (data != NULL) {
				snprintf(data, chars+1, format, timezone);
			}
		}

	} else {
#ifdef DEBUG
		fprintf(
			stderr,
			"[INFO] No entry for this IP address (%s) was found\n",
			ipstr);
#endif
		data = strdup(DEFAULT_TIMEZONE);
	}

	if (timezone != NULL) {
		free(timezone);
	}

	return data;
}

/**
 * geo_lookup_weather -  Given IP address (e.g. "107.77.70.111") returns weather location string.
 *     iso-BrooklynNYUS.
 *
 * Builds up a weather code string, we need to lookup weather using Accuweather data.
 *   country code (e.g. US)
 *   city (e.g. Beverly Hills)
 * If country code == US, get region (e.g. CA)
 * And then return "Beverly HillsCAUS" if a US address or
 * "Paris--FR" if non-US address.
 *
 * @param mmdb_handle - handle to the mmdb file
 * @param ipstr - ip address in string format e.g. 4.4.4.4
 * @param use_default - if true, return default on failures
 * @return string - Accuweather code e.g. "Beverly HillsCAUS"
 */
char *
geo_lookup_weather(MMDB_s *const mmdb_handle, const char *ipstr, int use_default)
{
	if (mmdb_handle == NULL) {
		fprintf(stderr, "[WARN] geo vmod given NULL maxmind db handle");
		return strdup(DEFAULT_WEATHER_CODE);
	}

	char *data = NULL;
	// Lookup IP in the DB
	int ip_lookup_failed = 0;
	int db_status = 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &ip_lookup_failed, &db_status);

	if (ip_lookup_failed) {
#ifdef DEBUG
		fprintf(stderr,
				"[WARN] geo_lookup_weather: Error from getaddrinfo for IP: %s Error Message: %s\n",
				ipstr, gai_strerror(ip_lookup_failed));
#endif
		// we don't want null, if we're not using default
		if (use_default) {
			return strdup(DEFAULT_WEATHER_CODE);
		} else {
			return strdup("--");
		}
	}

	if (db_status != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] geo_lookup_weather: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
				MMDB_CITY_PATH,
				MMDB_strerror(db_status));
#endif
		if (use_default) {
			return strdup(DEFAULT_WEATHER_CODE);
		} else {
			return strdup("--");
		}
	}

	// these varaibles will hold our results
	char *country = NULL;
	char *city    = NULL;
	char *state   = NULL;

	// these are used to extract values from the mmdb
	const char *country_lookup[] = {"country", "iso_code", NULL};
	const char *city_lookup[]    = {"city", "names", "en", NULL};
	const char *state_lookup[]   = {"subdivisions", "0", "iso_code", NULL};

	if (result.found_entry) {
		country = get_value(&result, country_lookup);
		city    = get_value(&result, city_lookup);

		if (country != NULL && strcmp(country, "US") == 0) {
			state = get_value(&result, state_lookup);
		} else {
			state = strdup("--");
		}

		if ((country == NULL || city == NULL || state == NULL) && (use_default)) {
			data = strdup(DEFAULT_WEATHER_CODE);
		} else {
			if (country == NULL) {
				country = strdup("--");
			}
			if (city == NULL) {
				city = strdup("--");
			}
			if (state == NULL) {
				state = strdup("--");
			}
			const char * iso = "iso-";
			size_t chars = sizeof(char) * (strlen(iso) + strlen(country) + strlen(city) + strlen(state));
			data = calloc(sizeof(char), chars+1);
			if (data != NULL) {
				int written = snprintf(data, chars+1, "%s%s%s%s", iso, city, state, country);
				if (written != chars) {
					if (use_default) {
						free(data);
						data = strdup(DEFAULT_WEATHER_CODE);
					} else {
						data[0] = '\0';
					}
				}
			}
		}
	} else {
#ifdef DEBUG
		fprintf(
			stderr,
			"[INFO] No entry for this IP address (%s) was found\n",
			ipstr);
#endif
		data = strdup(DEFAULT_WEATHER_CODE);
	}

	if (country != NULL) {
		free(country);
	}

	if (city != NULL) {
		free(city);
	}

	if (state != NULL) {
		free(state);
	}

	return data;
}



/**
 * a utility function for doing large scale testing
 */
void
dump_failed_lookup(MMDB_s *const mmdb_handle, const char *ipstr, const char *outputfile)
{
	if (mmdb_handle == NULL) {
		fprintf(stderr, "[WARN] geo vmod given NULL maxmind db handle");
		return;
	}

	// Lookup IP in the DB
	int ip_lookup_failed = 0;
	int db_status = 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &ip_lookup_failed, &db_status);
	if (ip_lookup_failed) {
#ifdef DEBUG
		fprintf(stderr,
				"[WARN] dump_failed_lookup: Error from getaddrinfo for IP: %s Error Message: %s\n",
				ipstr, gai_strerror(ip_lookup_failed));
#endif
		return;
	}

	if (db_status != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] dump_failed_lookup: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
				MMDB_CITY_PATH,
				MMDB_strerror(db_status));
#endif
		return;
	}

	FILE *f = fopen(outputfile, "a+");

	if (f == NULL) {
#ifdef DEBUG
		fprintf(stderr, "[ERROR] Unable to open the output file %s\n", outputfile);
#endif
		return;
	}

	MMDB_entry_data_list_s *entry_data_list = NULL;
	int status = MMDB_get_entry_data_list(&result.entry,
										  &entry_data_list);

	if (MMDB_SUCCESS != status) {
#ifndef DEBUG
		fprintf(
			stderr,
			"Got an error looking up the entry data - %s\n",
			MMDB_strerror(status));
#endif
		return;
	}

	if (entry_data_list != NULL) {
		const char *proxy_lookup[] = {"traits", "is_anonymous_proxy", NULL};
		const char *trait_lookup[] = {"traits", "is_satellite_provider", NULL};
		const char *lat_lookup[] = {"location", "latitude", NULL};
		const char *lon_lookup[] = {"location", "longitude", NULL};
		char *lat = get_value(&result, lat_lookup);
		char *lon = get_value(&result, lon_lookup);
		char *proxy = get_value(&result, proxy_lookup);
		char *satellite = get_value(&result, trait_lookup);
		if ((proxy     != NULL && strcmp(proxy, "true")) ||
			(satellite != NULL && strcmp(satellite, "true"))) {
			// we don't care about this.
			char *proxy_satelitte = (proxy == NULL) ? "satellite" : "proxy";
			fprintf(f,"%s,%s\n", ipstr, proxy_satelitte);
		} else if (lat != NULL && lon != NULL) {
			fprintf(f,"%s,%s,%s\n", ipstr, lat, lon);
		}
#ifdef DEBUG
		const char *reg_lookup[] = {"registered_country", "iso_code", NULL};
		char *reg_country = get_value(&result, reg_lookup);
		if (reg_country != NULL) {
			fprintf(f, "%s,%s\n", ipstr, reg_country);
			free(reg_country);
		}
		fprintf(f, "{\"%s\":", ipstr);
		MMDB_dump_entry_data_list(f, entry_data_list, 2);
		fprintf(f, "}\n");
#endif
		if (lat != NULL) {
			free(lat);
		}
		if (lon != NULL) {
			free(lon);
		}
		if (proxy != NULL) {
			free(proxy);
		}
		if (satellite != NULL) {
			free(satellite);
		}
	}
	fclose(f);
}

/**
 * Specific to our needs, given a cookie string and the name, return
 * the value of that string up to the first |
 *
 * @param string cookiestr - the http request header value of "Cookie:"
 * @param string cookiename - the cookie name you want a value of
 * @return string - the value or NULL
 */
char *
get_weather_code_from_cookie(const char *cookiestr, const char *cookiename)
{
	char* found = get_cookie(cookiestr, cookiename);
	if (found != NULL) {
		char* sep = strstr(found, "|");
		if (sep != NULL) {
			*sep = '\0';
		}
	}
	return found;
}

/**
 * Return cookie's value for specified cookie name.
 * @param string cookiestr - the http request header value of "Cookie:"
 * @param string cookiename - the cookie name you want a value of
 * @return string - the value or NULL
 */
char *
get_cookie(const char *cookiestr, const char *cookiename)
{
	const char *found = cookiestr;

	do {
		found = strstr(found, cookiename);

		if (found == NULL) {
			return NULL;
		}

		found += strlen(cookiename);

		// next character has to be equal or space
		if (*found == ' ' || *found == '=') {
			break;
		}

	} while(found);

	// cookies can have white space after the name, before the =
	while (*found && *found != '=') {
		++found; // move past the = sign
	}

	// should be at equal at this point
	if (*found != '=') {
		return NULL;
	}
	++found;

	// we should not have any white space after the = symbol
	// and if the next char is a ; there is no value for the cookie
	if (*found == '\0' || *found == ';' || *found == ' ') {
		return NULL;
	}

	// find the end of the cookie. cookies are name=value;
	char* end = (char *)found;
	while (*end && *end != ';' && *end != ' ') {
		++end;
	}

	int len      = end - found;
	char* result = NULL;
	result = calloc(sizeof(char), len+1);
	if (!result) {
		return NULL;
	}
	strncpy(result, found, len);
	return result;
}

/**
 * Given a handle to the mmdb file and an IP string, attempt to get the
 * latlon and format it as json. If use_default is set to true, we will
 * return the DEFAULT_LATLON if we can't get the data
 * during the mmdb lookup. A success will return json.
 *
 * @param mmdb_handle - handle to the mmdb file
 * @param ipstr - ip address in string format e.g. 4.4.4.4
 * @param use_default - if true, return default on failures
 * @return string - lat:lon e.g.
 *     "40.7561041:-73.9922971"
 */
char *
geo_lookup_latlon(MMDB_s *const mmdb_handle, const char *ipstr)
{
	if (mmdb_handle == NULL || ipstr == NULL) {
		fprintf(stderr, "[WARN] geo vmod given NULL maxmind db handle");
		return strdup(ERROR_LATLON);
	}

	char *data				= NULL;
	int ip_lookup_failed	= 0;
	int db_status			= 0;
	MMDB_lookup_result_s result =
		MMDB_lookup_string(mmdb_handle, ipstr, &ip_lookup_failed, &db_status);

	if (ip_lookup_failed) {
#ifdef DEBUG
		fprintf(stderr,
				"[WARN] geo_lookup_latlon: Error from getaddrinfo for IP: %s Error Message: %s\n",
				ipstr, gai_strerror(ip_lookup_failed));
#endif
		return strdup(ERROR_LATLON);
	}

	if (db_status != MMDB_SUCCESS) {
#ifdef DEBUG
		fprintf(stderr,
				"[ERROR] geo_lookup_latlon: libmaxminddb failure. \
Maybe there is something wrong with the file: %s libmaxmind error: %s\n",
				MMDB_CITY_PATH,
				MMDB_strerror(db_status));
#endif
		return strdup(ERROR_LATLON);
	}

	// these are used to extract values from the mmdb
	const char *lat_lookup[] = {"location", "latitude", NULL};
	const char *lon_lookup[] = {"location", "longitude", NULL};
	char *lat = NULL;
	char *lon = NULL;

	if (result.found_entry) {
		lat = get_value(&result, lat_lookup);
		lon = get_value(&result, lon_lookup);

		if (lat == NULL || lon == NULL) {
			data = strdup(DEFAULT_LATLON);
		} else {
			const char *format = "%s:%s";
			size_t chars = strlen(lat) + strlen(lon);
			chars += strlen(format) - 4; // less %s
			data = calloc(sizeof(char), chars+1);
			if (data != NULL) {
				snprintf(data, chars+1, format, lat, lon);
			} else {
				data = strdup(ERROR_LATLON);
			}
		}
	} else {
#ifdef DEBUG
		fprintf(
			stderr,
			"[INFO] No entry for this IP address (%s) was found\n",
			ipstr);
#endif
		data = strdup(DEFAULT_LATLON);
	}

	if (lat != NULL) {
		free(lat);
	}

	if (lon != NULL) {
		free(lon);
	}

	return data;
}
