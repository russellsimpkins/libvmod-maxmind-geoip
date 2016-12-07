#include <stdlib.h>
#include <maxminddb.h>

#include "vcl.h"
#include "vrt.h"
#include "cache/cache.h"
#include "vqueue.h"
#include "vcc_if.h"
#include "vmod_geo.h"

static MMDB_s mmdb_handle;
//**********************************************************************
// This has all of our vmod function definitions. This calls the
// functions defined in geo_functions.c
//**********************************************************************

// open the maxmind db once, during initialization.
int
init_function(VRT_CTX, struct vmod_priv *priv, enum vcl_event_e e)
{
    switch (e) {
    case VCL_EVENT_LOAD: {
        int mmdb_baddb = open_mmdb(&mmdb_handle);
        if (!mmdb_baddb) {
            priv->priv = (void *)&mmdb_handle;
            priv->free = close_mmdb;
        }
        return mmdb_baddb;
        break;
    }
    case VCL_EVENT_WARM: {
        break;
    }
    case VCL_EVENT_USE: {
        break;
    }
    case VCL_EVENT_COLD: {
        break;
    }
    case VCL_EVENT_DISCARD: {
        break;
    }
    }
    return 0;
}

// Lookup a field
VCL_STRING
vmod_lookup(VRT_CTX, struct vmod_priv *global, const char *ipstr, const char **lookup_path)
{
    char *data = NULL;
    char *cp   = NULL;
    MMDB_s * mmdb_handle = (struct MMDB_s *)global->priv;

    if (mmdb_handle == NULL) {
        fprintf(stderr, "[WARN] varnish gave NULL maxmind db handle");
        return cp;
    }
    data = geo_lookup(mmdb_handle, ipstr, lookup_path);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

// Lookup up a weather code
VCL_STRING
vmod_lookup_weathercode(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    char *data           = NULL;
    char *cp             = NULL;
    MMDB_s * mmdb_handle = (struct MMDB_s *)global->priv;

    if (mmdb_handle == NULL) {
        fprintf(stderr, "[WARN] varnish gave NULL maxmind db handle");
        return NULL;
    }

    data = geo_lookup_weather(mmdb_handle, ipstr, 1);
    int len = 0;

    if (data != NULL && (len = strlen(data)) > 0) {
        cp = WS_Copy(ctx->ws, data, len+1);
        if (cp == NULL) {
            cp = "";
        } else {
            cp[len] = '\0';
        }
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

// Lookup up a timezone
VCL_STRING
vmod_lookup_timezone(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    char *data           = NULL;
    char *cp             = NULL;
    MMDB_s * mmdb_handle = (struct MMDB_s *)global->priv;

    if (mmdb_handle == NULL) {
        fprintf(stderr, "[WARN] varnish gave NULL maxmind db handle");
        return NULL;
    }
    data = geo_lookup_timezone(mmdb_handle, ipstr, 1);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

// Lookup up a location
VCL_STRING
vmod_lookup_location(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    char *data           = NULL;
    char *cp             = NULL;
    MMDB_s * mmdb_handle = (struct MMDB_s *)global->priv;

    if (mmdb_handle == NULL) {
        fprintf(stderr, "[WARN] varnish gave NULL maxmind db handle");
        return NULL;
    }
    data = geo_lookup_location(mmdb_handle, ipstr);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

// lookup a city
VCL_STRING
vmod_city(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    const char *lookup_path[] = {"city", "names", "en", NULL};
    return vmod_lookup(ctx, global, ipstr, lookup_path);
}

// lookup a country
VCL_STRING
vmod_country(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    const char *lookup_path[] = {"country", "names", "en", NULL};
    return vmod_lookup(ctx, global, ipstr, lookup_path);
}

// lookup a metro code
VCL_STRING
vmod_metro_code(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    const char *lookup_path[] = {"location", "metro_code", NULL};
    return vmod_lookup(ctx, global, ipstr, lookup_path);
}

// lookup a region
VCL_STRING
vmod_region(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    const char *lookup_path[] = {"subdivisions", "0", "iso_code", NULL};
    return vmod_lookup(ctx, global, ipstr, lookup_path);
}

// lookup a country
VCL_STRING
vmod_country_code(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    const char *lookup_path[] = {"country", "iso_code", NULL};
    return vmod_lookup(ctx, global, ipstr, lookup_path);
}

// lookup an NYT weather code from ip address
VCL_STRING
vmod_weather_code(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    return vmod_lookup_weathercode(ctx, global, ipstr);
}

// lookup a Timezone from ip address
VCL_STRING
vmod_timezone(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    return vmod_lookup_timezone(ctx, global, ipstr);
}

// lookup an location from ip address
VCL_STRING
vmod_location(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    return vmod_lookup_location(ctx, global, ipstr);
}

// get the NYT weather cookie value from the cookie header
VCL_STRING
vmod_get_weather_cookie(VRT_CTX, const char *cookiestr, const char *cookiename)
{
    char *data = NULL;
    char *cp   = NULL;
    data       = get_weather_code_from_cookie(cookiestr, cookiename);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

// get a cookie value by name from the cookiestr
VCL_STRING
vmod_get_cookie(VRT_CTX, const char *cookiestr, const char *cookiename)
{
    char *data = NULL;
    char *cp   = NULL;
    data = get_cookie(cookiestr, cookiename);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}

int
vmod_init_mmdb(struct sess *sp, struct vmod_priv *global, const char *mmdb_path)
{
    int mmdb_baddb = MMDB_open(mmdb_path, MMDB_MODE_MMAP, &mmdb_handle);
    if (mmdb_baddb != MMDB_SUCCESS) {
#ifdef DEBUG
        fprintf(stderr, "[ERROR] MMDB_open: Can't open %s - %s\n",
                mmdb_path, MMDB_strerror(mmdb_baddb));
        if (MMDB_IO_ERROR == mmdb_baddb) {
            fprintf(stderr,
                    "[ERROR] MMDB_open: IO error: %s\n",
                    strerror(mmdb_baddb));
        }
#endif
        return 1;
    }
    return 0;
}

// lookup a Timezone from ip address
VCL_STRING
vmod_latlon(VRT_CTX, struct vmod_priv *global, const char *ipstr)
{
    char *data           = NULL;
    char *cp             = NULL;
    MMDB_s * mmdb_handle = (struct MMDB_s *)global->priv;

    if (mmdb_handle == NULL) {
        fprintf(stderr, "[WARN] varnish gave NULL maxmind db handle");
        return NULL;
    }
    data = geo_lookup_latlon(mmdb_handle, ipstr);

    if (data != NULL) {
        cp = WS_Copy(ctx->ws, data, strlen(data)+1);
        free(data);
    } else {
        cp = WS_Copy(ctx->ws, "", 1);
    }

    return cp;
}
