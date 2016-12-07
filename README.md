Varnish MaxMind GeoIP vmod
--------------------------

This vmod provides the ability to lookup location information based on IP address. The GeoIP database is a mmap'd red black tree implementation that's read-only. The code opens the database when the vmod is initialized and shutdown when the thread is terminated, leaving the database open for the life of the thread. The most expensive operation is opening the MaxMind database.

The vmod uses the MaxMind City database. 

## Usage

```
import geo
   // ....
   sub vcl_recv {
	 set req.http.X-Country     = geo.country("170.149.100.10")
	 set req.http.X-CountryCode = geo.country_code("170.149.100.10")
	 set req.http.X-Region      = geo.region("170.149.100.10")
	 set req.http.X-MetroCode   = geo.metro_code("170.149.100.10")
	 set req.http.X-City        = geo.city("170.149.100.10")
	 set req.http.X-Timezone    = geo.timezone("170.149.100.10")
	 set req.http.X-Location    = geo.location("170.149.100.10")

	 //# nytimes specific stuff
	 set req.http.Weather-Code   = geo.weather_code("170.149.100.10")
	 set req.http.Weather-Cookie = geo.get_weather_cookie(req.http.Cookie, "NYT_W2)
	 set req.http.Cookie-Value   = geo.get_cookie(req.http.Cookie, "NYT_W2")

	}
	// ....
}
```
The location call generates json e.g. geo.location("199.254.0.98") would return

``{"city":"Beverly Hills","state":"CA","country":"US"}"``

## Testing
There are unit tests and varnishtest scripts. The unit tests are in the tests folder. Edit tests/tests.c and rerun make && make test
The unit tests are done with unity https://mark-vandervoord-yxrv.squarespace.com/unity
Varnishtest scripts are in src/tests. To run the tests:

```
make test
```

## Installation

The vmod depends on having the following installed:

* Varnish - https://github.com/varnish/Varnish-Cache
* libmaxminddb - https://github.com/maxmind/libmaxminddb

http://maxmind.github.io/MaxMind-DB provides an indepth look at the MaxMind GeoIP database.

You can get a copy of the city database from here: https://dev.maxmind.com/geoip/geoip2/geolite2/

### Step 1 - install varnish-cache
```
cd /usr/local/src
git clone -b 4.1 https://github.com/varnish/Varnish-Cache.git
cd Varnish-Cache
./autogen.sh
./configure --prefix=/usr --libdir=/usr/lib64 # <- change this to lib if your on a 32 bit system
make
sudo make install
```

**NOTE:** I received the following after running make:

``You need rst2man installed to make dist``

I was able to get past this by installing python-docutils with:
```yum install python-docutils```

I then re-ran everything from ./autogen.sh onward.
Varnish 4.1 publishes a package config file, so make sure you set libdir correctly or you will have to specify the PKG_CONFIG_PATH in step 3

### Step 2 - install libmaxmind
```
cd ..
git clone --recursive https://github.com/maxmind/libmaxminddb.git
cd libmaxminddb
git submodule update
./bootstrap
./configure --prefix=/usr
make 
sudo make install
cd ..
```

### Step 3 - build the mddb vmod
```
git clone git@github.com:nytm/varnish-mmdb-vmod.git
cd varnish-mmdb-vmod
./autogen.sh
./configure --prefix=/usr --with-maxminddbfile=/mnt/mmdb/GeoIP2-City.mmdb VMODDIR=/usr/lib64/varnish/vmods 
make
sudo make install
```

**NOTE** I added support for a flag in autoconf:  **--with-maxminddbfile** so that you can decide, when you build the module, where you're data file will live. If you don't specify a value the default will be used **/mnt/mmdb/GeoIP2-City.mmdb** See src/vmod_geo.h

**NOTE** Varnish 4.1 installes a package config. If varnish installs the varnish.pc file in the wrong directory, you will need to specify in the configure command e.g. PKG_CONFIG_PATH=/usr/lib/pkgconfig


```
#define MAX_CITY_DB "/mnt/mmdb/GeoLite2-City.mmdb"
```

