#include "unity.h"
#include <stdlib.h>
#include <string.h>
#include <maxminddb.h>
#include <float.h>
#include "vmod_geo.h"
static MMDB_s mmdb_handle;
MMDB_s *
get_handle(void) {
    return &mmdb_handle;
}

void setUp(void)
{

}

void tearDown(void)
{

}

void test_OpenMMDB(void)
{
    int mmdb_baddb = open_mmdb(get_handle());
    TEST_ASSERT_EQUAL_INT(0,mmdb_baddb);
}

void test_Sweden()
{
  char *ip = "188.178.203.190";
  char *expected = "iso-Copenhagen--DK";
  char *value = geo_lookup_weather(get_handle(), ip, 1);
  TEST_ASSERT_EQUAL_STRING(expected, value);
  TEST_ASSERT_EQUAL(value[18], '\0');
  free(value);
}

void test_BadIP(void)
{
    char * ip = "127.0.0.1";
    char * value = geo_lookup_weather(get_handle(), ip,1);
    char * expected = "New YorkNYUS";
    TEST_ASSERT_EQUAL_STRING(expected,value);
    free(value);
}

void test_CaliforniaIP(void)
{
    char * ip = "199.254.0.98";
    char * value = geo_lookup_weather(get_handle(), ip,1);
    char * expected = "iso-Beverly HillsCAUS";
    TEST_ASSERT_EQUAL_STRING(expected,value);
    free(value);
}

void test_CaliforniaLatlon(void)
{
  char *ip = "199.254.0.98";
  char *value = geo_lookup_latlon(get_handle(), ip);
  printf("latlon: %s\n", value);
  char *expected = "34.060700:-118.403200";
  TEST_ASSERT_EQUAL_STRING(expected,value);
  free(value);
}

void test_ParisFranceIP(void)
{
    char * ip = "88.190.229.170";
    char * value = geo_lookup_weather(get_handle(), ip,1);
    char * expected = "iso-Paris--FR";
    TEST_ASSERT_EQUAL_STRING(expected,value);
    free(value);
}

void test_LookupCity(void)
{
    const char *lookup_path[] = {"city", "names", "en", NULL};
    char *ip = "199.254.0.98";
    char *actual = geo_lookup(get_handle(), ip, lookup_path);
    char *expected = "Beverly Hills";
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void test_LookupState(void)
{
    const char *lookup_path[] = {"subdivisions", "0", "iso_code", NULL};
    char *ip = "199.254.0.98";
    char *actual = geo_lookup(get_handle(), ip, lookup_path);
    char *expected = "CA";
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void test_LookupCountry(void)
{
    const char *lookup_path[] = {"country", "iso_code", NULL};
    char *ip = "199.254.0.98";
    char *actual = geo_lookup(get_handle(), ip, lookup_path);
    char *expected = "US";
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}

void test_LookupLocation(void)
{
    char *ip = "199.254.0.98";
    char *actual = geo_lookup_location(get_handle(), ip);
    char *expected = "{\"city\":\"Beverly Hills\",\"state\":\"CA\",\"country\":\"US\",\"lat\":34.060700,\"lon\":-118.403200,\"ip\":\"199.254.0.98\",\"response_code\":200}";

    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}


// test to make sure we always get back the first value no matter where the NYT_W2 cookie sits in a large string

void test_GetWeatherCode(void)
{
    const char * cookiestra = "NYT-Edition=edition|US; localytics.js=1; RMID=007f01002c15543d3ed50004; madison_user_id=3tO7Obk_Sg6uI4lzodnFTQ; madison_guest=false; s_pers=%20s_vsn_nytimesglobal_1%3D7844460248015%7C1730135643028%3B; __gads=ID=b7772508b463357f:T=1414525015:S=ALNI_MYUerI95dT7BeEuYt-8nbA3yaoQ-Q; wordpress_test_cookie=WP+Cookie+check; wordpress_logged_in_=scott.blumenthal%7C1414785730%7Cdd4286dfc0d60c4f7e605a51dd4614c5; fbm_9869919170=base_domain=.nytimes.com; nyt-recommend=1; __utma=69104142.778352587.1413318396.1415723158.1415983661.32; __utmc=69104142; __utmz=69104142.1415212413.29.12.utmcsr=localhost.nytimes.com:3001|utmccn=(referral)|utmcmd=referral|utmcct=/portal; nyt-d=101.000000000NAI00000C9Iio1f6ous0z1Iq/0eVpii0qApjv1eUY4r0M4meE1/FImH0AUY4r0C1sCa0f6mf/0zB0SA1w0mSI0M6tmN0vCp0g0aVnyU0K2Wnv0eCoir0kVnyU032W9v0UUWC70I4HeU0D77np1hStHe1rTcD1@c194bf3e/378764d3; _dycmc=17; _dyuss_undefined=8; _dyus_undefined=4%7C0%7C0%7C4%7C0%7C0.17.1398875133463.1416256906833.1022457.0%7C320%7C47%7C10%7C114%7C1%7C0%7C0%7C0%7C0%7C0%7C0%7C1%7C0%7C3%7C4%7C0%7C0%7C4%7C4%7C7%7C0%7C0%7C0; _ga=GA1.2.778352587.1413318396; welcomeviewed=1; nyt-m=896A6446A903792D23C341A88595492C&e=i.1420088400&t=i.10&v=i.0&n=i.2&g=i.0&rc=i.0&er=i.1415720418&vr=l.4.64.240.307.192&pr=l.4.152.799.1466.948&vp=i.739&gf=l.10.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&ft=i.0&fv=i.0&rl=l.1.-1&cav=i.6&imu=i.1&igu=i.1&prt=i.5&kid=i.1&ica=i.0&iue=i.1&ier=i.0&iub=i.0&ifv=i.0&igd=i.0&iga=i.0&imv=i.0&igf=i.0&iru=i.0&ird=i.0&ira=i.0&iir=i.1&l=l.15.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&gl=l.2.-1.-1&gb=l.3.0.5.1416286800; _dysvar_8765260=63%253A@%253Afeeds.@.64%253A@%253Apolitics.@.65%253A@%253Ainteractive; _dyuss_8765260=89; _dycst=tg.m.frv1.ms.ltos.c.; _dycnoabc=1417552237877; _dyus_8765260=694%7C0%7C0%7C6%7C0%7C0.0.1398091900829.1415140239176.17048338.0%7C307%7C45%7C10%7C114%7C9%7C9%7C0%7C0%7C5%7C4%7C0%7C18%7C12%7C0%7C1%7C10%7C4%7C18%7C25%7C52%7C14%7C7%7C62; circgroup=store; _cb_ls=1; __utma=28590334.778352587.1413318396.1416594906.1417620206.2; __utmb=28590334.6.8.1417620228850; __utmc=28590334; __utmz=28590334.1416594906.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); _chartbeat2=tH6YdLrhnyDMUI6M.1397144713343.1417621166239.0000000000000001; rsi_segs=H07707_11003|H07707_10415|H07707_11063|D08734_70059|H07707_10207|D08734_70105|D08734_70509|D08734_70751|D08734_72008|D08734_72078|H07707_10044|H07707_10577|H07707_10638|H07707_11173|H07707_11174|H07707_11207|H07707_11175|H07707_11176|H07707_11178|H07707_11183|H07707_11187|H07707_11194|H07707_11195|H07707_11196|H07707_0; ab1=yes; ab2=yes; ab3=yes; _dyuss_8765283=6; _dycst=tg.m.frv3.ms.ltos.c.; _dycnoabc=1417621169989; NYT-mab=%7B%221%22%3A%22RI%3A5%22%7D; WT_FPC=id=8ad5d99b-3fc9-4d66-820c-24c260f59c62:lv=1417621210781:ss=1417619144492; _dy_geo=US.NA; krux_segs=n58825q2q%7Cooo3l4e96%7Cohph8wqcu%7Cm8yi6fn0f%7Couxjsml96%7Coeh287nzm%7Copjtfyu92%7Cn7szfo3xl%7Corj54ice4%7Com4sj6shu%7Cmimpufxui%7Cn7seltb6y%7Cn7szfoqt6%7Cn7szfomx3%7Cosewaclj3%7Colkt7d9x1%7Cnr9v9llm0%7Coxtp6iwgj%7Cm9ewee5pq%7Clmviw2vot%7Cn7szfp6hd%7Cn7szflyme%7Cn7szfp1ky%7Cm9pwujheh%7Cn7szfl4pr%7Cm8yye40vf%7Cobz8gonvw%7Cosihzsglx%7Cn7szfq00g%7Cosepjej6x%7Cogpp6a8ph%7Cn7szfnicn%7Coo7ak7npn%7Cmithviff8%7Coj2aw1ioh%7Cm9pwg0j70%7Corqp3nlu7%7Cmf94g1ng5%7Cosfb5x7ni%7Cn7szfqu4p%7Cn7szfpd5c%7Cn7szfo8qn%7Cosiv2aaqb%7Cn7szfpxrm%7Cos7bpagdd%7Copz7ovkmb%7Cose3hh4uj%7Coktlxay2x%7Cllf28yvry%7Cnxjo4r9x2%7Cn7szfpiuz%7Cn7szfn5z4%7Cor63eu8tq%7Cnxq8cvp69%7Consdjs5d5%7Cn55py3t5z%7Cn7szfozqo%7Cn4l4i077e%7Cn7szfpsa8%7Cmg1nsx1fv%7Cni23h8v7i%7Coe2qe4v6h%7Cnts9co5pn%7Cosmd7fhhy%7Cnv0z088vj%7Cm5m11hxs9%7Cnjr5hi4dg%7Cn7szfotw4%7Cn3qirawfw%7Cn7szfovgr%7Cor63qghhk%7Cn7szfpg8d%7Cn7szfq3ce%7Coj2f8sugv%7Col8a2b12r%7Com4xgijd5%7Cosia5rtou%7Cn7szfqg9h%7Cn7szfp3w0%7Cmith0bdg1%7Cm8yxv39ew%7Cmk1bczyjt%7Coi15lc4ig%7Corrl8dtnr%7Cnvjujpjr5%7Cn7szfohm8%7Cnvjps5n77%7Com37mgsm0%7Cm9pwj9zwc%7Colij9j24i%7Cosinwlerj%7Corfj4mfif%7Clmviretav%7Cmitha2f0p%7Cosd9mpckx%7Cosfjeop7w%7Cn7szfnfqa%7Copz7s44rg%7Copvlqddbx%7Cn7szfm50i%7Cn7szfqqcb%7Cmu2yn384m%7Cn7szfqkrg%7Coygqn8xqq%7Cose9dhiwy%7Cmiticd6en%7Coygp13c0e%7Cosiap3fde%7Cn7szfmnx5%7Cosbq85ja6%7Cor6nmdiqi%7Consa2ur7h%7Cm5qw3j39o%7Cn7szfoflt%7Cmk1apcr0l%7Cn7szfopag%7Cmimp808l7%7Cop628novu%7Cnnebseogg%7Conzln8357%7Cor63hay7s%7Clmvity5kb%7Cocuqd5reh%7Cot62o7t28%7Cnuzmoezkg%7Cn7szfmfna%7Cn7szfnpkk%7Cookj64uiq%7Cn7szfp9zx%7Cn7szfokt4%7Cm9h4kcgmq%7Cosh3bzwwo%7Cn7szfpn20%7Cmkdw07f0t%7Coypsztoyt%7Cn0jujk1xo%7Cn7szfodpt%7Cn7szfmbfg%7Cm8yjzdn6h%7Cn7szfnjri%7Cnv0695lgs%7Cnjrz9tpye%7Cmimr5pw0n%7Conrl71gcz%7Coonzp0li4%7Cosbscev80%7Cor7a8d230%7Cmil0ydr2y%7Conrjxld9v%7Cmgy7i5i8n%7Cor63arfps%7Col44qk36m%7Comvfswdjz%7Coe92g4cti%7Cow6bxaj2q%7Coe5xlqm5g%7Cosbun07ae%7Colokhjnjo%7Cor624qgz2%7Cm8yynb42j; nyt-a=2c61af26c5e5dd3243e5dacee20368db; _chartbeat4=t=BO41WDCIZ6YKNL9NxGUYXYZ2Whf&E=0&x=0&c=8.05&y=5051&w=429; _dyus_8765283=850%7C0%7C0%7C0%7C0%7C0.0.1397144713754.1417621169050.20476455.0%7C336%7C49%7C11%7C114%7C6%7C0%7C0%7C0%7C0%7C0%7C0%7C6%7C0%7C0%7C0%7C0%7C2%7C6%7C0%7C6%7C62%7C10%7C2; _dyrc=; adxcl=t*3e882=547fea4f:1415163821|t*3fe28=54813bcf:1415283269|t*3f9e8=54813bcf:1415287349|t*400ca=54828d4f:1415373334|t*40156=5485304f:1415584888|l*3bfc4=6013964f:1|t*40711=54a2314f:1417460577|t*3d7de=54ab6bcf:1415144032|lcig=55c42d3f:1|l*31068=566cfb4f:1|t*3f9cc=54c9be4f:1416238182|l*352b7=55c42d3f:1; adxcs=s*3eb33=0:1|s*3eb34=0:1|s*3fc77=0:1|s*3e7a3=0:1|s*3ee80=0:1|stiffany-sur=0:3|s*3f2f6=0:1|s*400ca=0:1|s*3f65b=0:1|s*401a6=0:1|s*3a3d4=0:1|si=0:1|s*3ef58=0:2|s*3f1eb=0:1|s*3f232=0:1; NYT-S=488tnv36oe3LC4x8aE6wiJBUN9UMCN5cRph1fqDAt4FAo8/Tmo1TI8r4dnlfH.PgAlyITOtMElPUh/1BHDL5FwxTgU/uxaXj5EheKVKOJ3FDzatRBLydWjyKvJJp.6isWKAOv4Pg3StLRJXdiICg4xQbQEigbFF5r9EA3S3wXCb8hRCPVdhsyeEbQQSz4dVrWCDjRfwsa3OHQnYGGpMlOwgep2harHhSvXpFdwHys/y/cCQx0aGXKsAHLKZUTSaBQLiew/8jh4FIfztkIt4yfhCeFnnlktqWFTbppAT5nE4HRWkZmSBpm9u5NXoAIORqPBPZ1aIBl0RbQfzBXnU2qc3cV.Ynx4rkFI; NYT-wpAB=0002|1&0010|2&0011|3&0012|1; NYT_W2=New%20YorkNYUS; abc=ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP";
    printf("Cookie is this long: %lu\n\n", strlen(cookiestra));
    const char* cookiename = "NYT_W2";
    const char* expected = "New%20YorkNYUS";
    char* actual = get_weather_code_from_cookie(cookiestra, cookiename);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);

    const char* cookiestrb = "NYT-Edition=edition|US; localytics.js=1; RMID=007f01002c15543d3ed50004; madison_user_id=3tO7Obk_Sg6uI4lzodnFTQ; madison_guest=false; s_pers=%20s_vsn_nytimesglobal_1%3D7844460248015%7C1730135643028%3B; __gads=ID=b7772508b463357f:T=1414525015:S=ALNI_MYUerI95dT7BeEuYt-8nbA3yaoQ-Q; wordpress_test_cookie=WP+Cookie+check; wordpress_logged_in_=scott.blumenthal%7C1414785730%7Cdd4286dfc0d60c4f7e605a51dd4614c5; fbm_9869919170=base_domain=.nytimes.com; nyt-recommend=1; __utma=69104142.778352587.1413318396.1415723158.1415983661.32; __utmc=69104142; __utmz=69104142.1415212413.29.12.utmcsr=localhost.nytimes.com:3001|utmccn=(referral)|utmcmd=referral|utmcct=/portal; nyt-d=101.000000000NAI00000C9Iio1f6ous0z1Iq/0eVpii0qApjv1eUY4r0M4meE1/FImH0AUY4r0C1sCa0f6mf/0zB0SA1w0mSI0M6tmN0vCp0g0aVnyU0K2Wnv0eCoir0kVnyU032W9v0UUWC70I4HeU0D77np1hStHe1rTcD1@c194bf3e/378764d3; _dycmc=17; _dyuss_undefined=8; _dyus_undefined=4%7C0%7C0%7C4%7C0%7C0.17.1398875133463.1416256906833.1022457.0%7C320%7C47%7C10%7C114%7C1%7C0%7C0%7C0%7C0%7C0%7C0%7C1%7C0%7C3%7C4%7C0%7C0%7C4%7C4%7C7%7C0%7C0%7C0; _ga=GA1.2.778352587.1413318396; welcomeviewed=1; nyt-m=896A6446A903792D23C341A88595492C&e=i.1420088400&t=i.10&v=i.0&n=i.2&g=i.0&rc=i.0&er=i.1415720418&vr=l.4.64.240.307.192&pr=l.4.152.799.1466.948&vp=i.739&gf=l.10.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&ft=i.0&fv=i.0&rl=l.1.-1&cav=i.6&imu=i.1&igu=i.1&prt=i.5&kid=i.1&ica=i.0&iue=i.1&ier=i.0&iub=i.0&ifv=i.0&igd=i.0&iga=i.0&imv=i.0&igf=i.0&iru=i.0&ird=i.0&ira=i.0&iir=i.1&l=l.15.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&gl=l.2.-1.-1&gb=l.3.0.5.1416286800; _dysvar_8765260=63%253A@%253Afeeds.@.64%253A@%253Apolitics.@.65%253A@%253Ainteractive; _dyuss_8765260=89; _dycst=tg.m.frv1.ms.ltos.c.; _dycnoabc=1417552237877; _dyus_8765260=694%7C0%7C0%7C6%7C0%7C0.0.1398091900829.1415140239176.17048338.0%7C307%7C45%7C10%7C114%7C9%7C9%7C0%7C0%7C5%7C4%7C0%7C18%7C12%7C0%7C1%7C10%7C4%7C18%7C25%7C52%7C14%7C7%7C62; circgroup=store; _cb_ls=1; __utma=28590334.778352587.1413318396.1416594906.1417620206.2; __utmb=28590334.6.8.1417620228850; __utmc=28590334; __utmz=28590334.1416594906.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); _chartbeat2=tH6YdLrhnyDMUI6M.1397144713343.1417621166239.0000000000000001; rsi_segs=H07707_11003|H07707_10415|H07707_11063|D08734_70059|H07707_10207|D08734_70105|D08734_70509|D08734_70751|D08734_72008|D08734_72078|H07707_10044|H07707_10577|H07707_10638|H07707_11173|H07707_11174|H07707_11207|H07707_11175|H07707_11176|H07707_11178|H07707_11183|H07707_11187|H07707_11194|H07707_11195|H07707_11196|H07707_0; ab1=yes; ab2=yes; ab3=yes; _dyuss_8765283=6; _dycst=tg.m.frv3.ms.ltos.c.; _dycnoabc=1417621169989; NYT-mab=%7B%221%22%3A%22RI%3A5%22%7D; WT_FPC=id=8ad5d99b-3fc9-4d66-820c-24c260f59c62:lv=1417621210781:ss=1417619144492; _dy_geo=US.NA; krux_segs=n58825q2q%7Cooo3l4e96%7Cohph8wqcu%7Cm8yi6fn0f%7Couxjsml96%7Coeh287nzm%7Copjtfyu92%7Cn7szfo3xl%7Corj54ice4%7Com4sj6shu%7Cmimpufxui%7Cn7seltb6y%7Cn7szfoqt6%7Cn7szfomx3%7Cosewaclj3%7Colkt7d9x1%7Cnr9v9llm0%7Coxtp6iwgj%7Cm9ewee5pq%7Clmviw2vot%7Cn7szfp6hd%7Cn7szflyme%7Cn7szfp1ky%7Cm9pwujheh%7Cn7szfl4pr%7Cm8yye40vf%7Cobz8gonvw%7Cosihzsglx%7Cn7szfq00g%7Cosepjej6x%7Cogpp6a8ph%7Cn7szfnicn%7Coo7ak7npn%7Cmithviff8%7Coj2aw1ioh%7Cm9pwg0j70%7Corqp3nlu7%7Cmf94g1ng5%7Cosfb5x7ni%7Cn7szfqu4p%7Cn7szfpd5c%7Cn7szfo8qn%7Cosiv2aaqb%7Cn7szfpxrm%7Cos7bpagdd%7Copz7ovkmb%7Cose3hh4uj%7Coktlxay2x%7Cllf28yvry%7Cnxjo4r9x2%7Cn7szfpiuz%7Cn7szfn5z4%7Cor63eu8tq%7Cnxq8cvp69%7Consdjs5d5%7Cn55py3t5z%7Cn7szfozqo%7Cn4l4i077e%7Cn7szfpsa8%7Cmg1nsx1fv%7Cni23h8v7i%7Coe2qe4v6h%7Cnts9co5pn%7Cosmd7fhhy%7Cnv0z088vj%7Cm5m11hxs9%7Cnjr5hi4dg%7Cn7szfotw4%7Cn3qirawfw%7Cn7szfovgr%7Cor63qghhk%7Cn7szfpg8d%7Cn7szfq3ce%7Coj2f8sugv%7Col8a2b12r%7Com4xgijd5%7Cosia5rtou%7Cn7szfqg9h%7Cn7szfp3w0%7Cmith0bdg1%7Cm8yxv39ew%7Cmk1bczyjt%7Coi15lc4ig%7Corrl8dtnr%7Cnvjujpjr5%7Cn7szfohm8%7Cnvjps5n77%7Com37mgsm0%7Cm9pwj9zwc%7Colij9j24i%7Cosinwlerj%7Corfj4mfif%7Clmviretav%7Cmitha2f0p%7Cosd9mpckx%7Cosfjeop7w%7Cn7szfnfqa%7Copz7s44rg%7Copvlqddbx%7Cn7szfm50i%7Cn7szfqqcb%7Cmu2yn384m%7Cn7szfqkrg%7Coygqn8xqq%7Cose9dhiwy%7Cmiticd6en%7Coygp13c0e%7Cosiap3fde%7Cn7szfmnx5%7Cosbq85ja6%7Cor6nmdiqi%7Consa2ur7h%7Cm5qw3j39o%7Cn7szfoflt%7Cmk1apcr0l%7Cn7szfopag%7Cmimp808l7%7Cop628novu%7Cnnebseogg%7Conzln8357%7Cor63hay7s%7Clmvity5kb%7Cocuqd5reh%7Cot62o7t28%7Cnuzmoezkg%7Cn7szfmfna%7Cn7szfnpkk%7Cookj64uiq%7Cn7szfp9zx%7Cn7szfokt4%7Cm9h4kcgmq%7Cosh3bzwwo%7Cn7szfpn20%7Cmkdw07f0t%7Coypsztoyt%7Cn0jujk1xo%7Cn7szfodpt%7Cn7szfmbfg%7Cm8yjzdn6h%7Cn7szfnjri%7Cnv0695lgs%7Cnjrz9tpye%7Cmimr5pw0n%7Conrl71gcz%7Coonzp0li4%7Cosbscev80%7Cor7a8d230%7Cmil0ydr2y%7Conrjxld9v%7Cmgy7i5i8n%7Cor63arfps%7Col44qk36m%7Comvfswdjz%7Coe92g4cti%7Cow6bxaj2q%7Coe5xlqm5g%7Cosbun07ae%7Colokhjnjo%7Cor624qgz2%7Cm8yynb42j; nyt-a=2c61af26c5e5dd3243e5dacee20368db; _chartbeat4=t=BO41WDCIZ6YKNL9NxGUYXYZ2Whf&E=0&x=0&c=8.05&y=5051&w=429; _dyus_8765283=850%7C0%7C0%7C0%7C0%7C0.0.1397144713754.1417621169050.20476455.0%7C336%7C49%7C11%7C114%7C6%7C0%7C0%7C0%7C0%7C0%7C0%7C6%7C0%7C0%7C0%7C0%7C2%7C6%7C0%7C6%7C62%7C10%7C2; _dyrc=; adxcl=t*3e882=547fea4f:1415163821|t*3fe28=54813bcf:1415283269|t*3f9e8=54813bcf:1415287349|t*400ca=54828d4f:1415373334|t*40156=5485304f:1415584888|l*3bfc4=6013964f:1|t*40711=54a2314f:1417460577|t*3d7de=54ab6bcf:1415144032|lcig=55c42d3f:1|l*31068=566cfb4f:1|t*3f9cc=54c9be4f:1416238182|l*352b7=55c42d3f:1; adxcs=s*3eb33=0:1|s*3eb34=0:1|s*3fc77=0:1|s*3e7a3=0:1|s*3ee80=0:1|stiffany-sur=0:3|s*3f2f6=0:1|s*400ca=0:1|s*3f65b=0:1|s*401a6=0:1|s*3a3d4=0:1|si=0:1|s*3ef58=0:2|s*3f1eb=0:1|s*3f232=0:1; NYT-S=488tnv36oe3LC4x8aE6wiJBUN9UMCN5cRph1fqDAt4FAo8/Tmo1TI8r4dnlfH.PgAlyITOtMElPUh/1BHDL5FwxTgU/uxaXj5EheKVKOJ3FDzatRBLydWjyKvJJp.6isWKAOv4Pg3StLRJXdiICg4xQbQEigbFF5r9EA3S3wXCb8hRCPVdhsyeEbQQSz4dVrWCDjRfwsa3OHQnYGGpMlOwgep2harHhSvXpFdwHys/y/cCQx0aGXKsAHLKZUTSaBQLiew/8jh4FIfztkIt4yfhCeFnnlktqWFTbppAT5nE4HRWkZmSBpm9u5NXoAIORqPBPZ1aIBl0RbQfzBXnU2qc3cV.Ynx4rkFI; NYT-wpAB=0002|1&0010|2&0011|3&0012|1; NYT_W2=New%20YorkNYUS|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP";
    actual = get_weather_code_from_cookie(cookiestrb, cookiename);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
    const char * cookiestrc = "NYT_W2=New%20YorkNYUS|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP; NYT-Edition=edition|US; localytics.js=1; RMID=007f01002c15543d3ed50004; madison_user_id=3tO7Obk_Sg6uI4lzodnFTQ; madison_guest=false; s_pers=%20s_vsn_nytimesglobal_1%3D7844460248015%7C1730135643028%3B; __gads=ID=b7772508b463357f:T=1414525015:S=ALNI_MYUerI95dT7BeEuYt-8nbA3yaoQ-Q; wordpress_test_cookie=WP+Cookie+check; wordpress_logged_in_=scott.blumenthal%7C1414785730%7Cdd4286dfc0d60c4f7e605a51dd4614c5; fbm_9869919170=base_domain=.nytimes.com; nyt-recommend=1; __utma=69104142.778352587.1413318396.1415723158.1415983661.32; __utmc=69104142; __utmz=69104142.1415212413.29.12.utmcsr=localhost.nytimes.com:3001|utmccn=(referral)|utmcmd=referral|utmcct=/portal; nyt-d=101.000000000NAI00000C9Iio1f6ous0z1Iq/0eVpii0qApjv1eUY4r0M4meE1/FImH0AUY4r0C1sCa0f6mf/0zB0SA1w0mSI0M6tmN0vCp0g0aVnyU0K2Wnv0eCoir0kVnyU032W9v0UUWC70I4HeU0D77np1hStHe1rTcD1@c194bf3e/378764d3; _dycmc=17; _dyuss_undefined=8; _dyus_undefined=4%7C0%7C0%7C4%7C0%7C0.17.1398875133463.1416256906833.1022457.0%7C320%7C47%7C10%7C114%7C1%7C0%7C0%7C0%7C0%7C0%7C0%7C1%7C0%7C3%7C4%7C0%7C0%7C4%7C4%7C7%7C0%7C0%7C0; _ga=GA1.2.778352587.1413318396; welcomeviewed=1; nyt-m=896A6446A903792D23C341A88595492C&e=i.1420088400&t=i.10&v=i.0&n=i.2&g=i.0&rc=i.0&er=i.1415720418&vr=l.4.64.240.307.192&pr=l.4.152.799.1466.948&vp=i.739&gf=l.10.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&ft=i.0&fv=i.0&rl=l.1.-1&cav=i.6&imu=i.1&igu=i.1&prt=i.5&kid=i.1&ica=i.0&iue=i.1&ier=i.0&iub=i.0&ifv=i.0&igd=i.0&iga=i.0&imv=i.0&igf=i.0&iru=i.0&ird=i.0&ira=i.0&iir=i.1&l=l.15.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&gl=l.2.-1.-1&gb=l.3.0.5.1416286800; _dysvar_8765260=63%253A@%253Afeeds.@.64%253A@%253Apolitics.@.65%253A@%253Ainteractive; _dyuss_8765260=89; _dycst=tg.m.frv1.ms.ltos.c.; _dycnoabc=1417552237877; _dyus_8765260=694%7C0%7C0%7C6%7C0%7C0.0.1398091900829.1415140239176.17048338.0%7C307%7C45%7C10%7C114%7C9%7C9%7C0%7C0%7C5%7C4%7C0%7C18%7C12%7C0%7C1%7C10%7C4%7C18%7C25%7C52%7C14%7C7%7C62; circgroup=store; _cb_ls=1; __utma=28590334.778352587.1413318396.1416594906.1417620206.2; __utmb=28590334.6.8.1417620228850; __utmc=28590334; __utmz=28590334.1416594906.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); _chartbeat2=tH6YdLrhnyDMUI6M.1397144713343.1417621166239.0000000000000001; rsi_segs=H07707_11003|H07707_10415|H07707_11063|D08734_70059|H07707_10207|D08734_70105|D08734_70509|D08734_70751|D08734_72008|D08734_72078|H07707_10044|H07707_10577|H07707_10638|H07707_11173|H07707_11174|H07707_11207|H07707_11175|H07707_11176|H07707_11178|H07707_11183|H07707_11187|H07707_11194|H07707_11195|H07707_11196|H07707_0; ab1=yes; ab2=yes; ab3=yes; _dyuss_8765283=6; _dycst=tg.m.frv3.ms.ltos.c.; _dycnoabc=1417621169989; NYT-mab=%7B%221%22%3A%22RI%3A5%22%7D; WT_FPC=id=8ad5d99b-3fc9-4d66-820c-24c260f59c62:lv=1417621210781:ss=1417619144492; _dy_geo=US.NA; krux_segs=n58825q2q%7Cooo3l4e96%7Cohph8wqcu%7Cm8yi6fn0f%7Couxjsml96%7Coeh287nzm%7Copjtfyu92%7Cn7szfo3xl%7Corj54ice4%7Com4sj6shu%7Cmimpufxui%7Cn7seltb6y%7Cn7szfoqt6%7Cn7szfomx3%7Cosewaclj3%7Colkt7d9x1%7Cnr9v9llm0%7Coxtp6iwgj%7Cm9ewee5pq%7Clmviw2vot%7Cn7szfp6hd%7Cn7szflyme%7Cn7szfp1ky%7Cm9pwujheh%7Cn7szfl4pr%7Cm8yye40vf%7Cobz8gonvw%7Cosihzsglx%7Cn7szfq00g%7Cosepjej6x%7Cogpp6a8ph%7Cn7szfnicn%7Coo7ak7npn%7Cmithviff8%7Coj2aw1ioh%7Cm9pwg0j70%7Corqp3nlu7%7Cmf94g1ng5%7Cosfb5x7ni%7Cn7szfqu4p%7Cn7szfpd5c%7Cn7szfo8qn%7Cosiv2aaqb%7Cn7szfpxrm%7Cos7bpagdd%7Copz7ovkmb%7Cose3hh4uj%7Coktlxay2x%7Cllf28yvry%7Cnxjo4r9x2%7Cn7szfpiuz%7Cn7szfn5z4%7Cor63eu8tq%7Cnxq8cvp69%7Consdjs5d5%7Cn55py3t5z%7Cn7szfozqo%7Cn4l4i077e%7Cn7szfpsa8%7Cmg1nsx1fv%7Cni23h8v7i%7Coe2qe4v6h%7Cnts9co5pn%7Cosmd7fhhy%7Cnv0z088vj%7Cm5m11hxs9%7Cnjr5hi4dg%7Cn7szfotw4%7Cn3qirawfw%7Cn7szfovgr%7Cor63qghhk%7Cn7szfpg8d%7Cn7szfq3ce%7Coj2f8sugv%7Col8a2b12r%7Com4xgijd5%7Cosia5rtou%7Cn7szfqg9h%7Cn7szfp3w0%7Cmith0bdg1%7Cm8yxv39ew%7Cmk1bczyjt%7Coi15lc4ig%7Corrl8dtnr%7Cnvjujpjr5%7Cn7szfohm8%7Cnvjps5n77%7Com37mgsm0%7Cm9pwj9zwc%7Colij9j24i%7Cosinwlerj%7Corfj4mfif%7Clmviretav%7Cmitha2f0p%7Cosd9mpckx%7Cosfjeop7w%7Cn7szfnfqa%7Copz7s44rg%7Copvlqddbx%7Cn7szfm50i%7Cn7szfqqcb%7Cmu2yn384m%7Cn7szfqkrg%7Coygqn8xqq%7Cose9dhiwy%7Cmiticd6en%7Coygp13c0e%7Cosiap3fde%7Cn7szfmnx5%7Cosbq85ja6%7Cor6nmdiqi%7Consa2ur7h%7Cm5qw3j39o%7Cn7szfoflt%7Cmk1apcr0l%7Cn7szfopag%7Cmimp808l7%7Cop628novu%7Cnnebseogg%7Conzln8357%7Cor63hay7s%7Clmvity5kb%7Cocuqd5reh%7Cot62o7t28%7Cnuzmoezkg%7Cn7szfmfna%7Cn7szfnpkk%7Cookj64uiq%7Cn7szfp9zx%7Cn7szfokt4%7Cm9h4kcgmq%7Cosh3bzwwo%7Cn7szfpn20%7Cmkdw07f0t%7Coypsztoyt%7Cn0jujk1xo%7Cn7szfodpt%7Cn7szfmbfg%7Cm8yjzdn6h%7Cn7szfnjri%7Cnv0695lgs%7Cnjrz9tpye%7Cmimr5pw0n%7Conrl71gcz%7Coonzp0li4%7Cosbscev80%7Cor7a8d230%7Cmil0ydr2y%7Conrjxld9v%7Cmgy7i5i8n%7Cor63arfps%7Col44qk36m%7Comvfswdjz%7Coe92g4cti%7Cow6bxaj2q%7Coe5xlqm5g%7Cosbun07ae%7Colokhjnjo%7Cor624qgz2%7Cm8yynb42j; nyt-a=2c61af26c5e5dd3243e5dacee20368db; _chartbeat4=t=BO41WDCIZ6YKNL9NxGUYXYZ2Whf&E=0&x=0&c=8.05&y=5051&w=429; _dyus_8765283=850%7C0%7C0%7C0%7C0%7C0.0.1397144713754.1417621169050.20476455.0%7C336%7C49%7C11%7C114%7C6%7C0%7C0%7C0%7C0%7C0%7C0%7C6%7C0%7C0%7C0%7C0%7C2%7C6%7C0%7C6%7C62%7C10%7C2; _dyrc=; adxcl=t*3e882=547fea4f:1415163821|t*3fe28=54813bcf:1415283269|t*3f9e8=54813bcf:1415287349|t*400ca=54828d4f:1415373334|t*40156=5485304f:1415584888|l*3bfc4=6013964f:1|t*40711=54a2314f:1417460577|t*3d7de=54ab6bcf:1415144032|lcig=55c42d3f:1|l*31068=566cfb4f:1|t*3f9cc=54c9be4f:1416238182|l*352b7=55c42d3f:1; adxcs=s*3eb33=0:1|s*3eb34=0:1|s*3fc77=0:1|s*3e7a3=0:1|s*3ee80=0:1|stiffany-sur=0:3|s*3f2f6=0:1|s*400ca=0:1|s*3f65b=0:1|s*401a6=0:1|s*3a3d4=0:1|si=0:1|s*3ef58=0:2|s*3f1eb=0:1|s*3f232=0:1; NYT-S=488tnv36oe3LC4x8aE6wiJBUN9UMCN5cRph1fqDAt4FAo8/Tmo1TI8r4dnlfH.PgAlyITOtMElPUh/1BHDL5FwxTgU/uxaXj5EheKVKOJ3FDzatRBLydWjyKvJJp.6isWKAOv4Pg3StLRJXdiICg4xQbQEigbFF5r9EA3S3wXCb8hRCPVdhsyeEbQQSz4dVrWCDjRfwsa3OHQnYGGpMlOwgep2harHhSvXpFdwHys/y/cCQx0aGXKsAHLKZUTSaBQLiew/8jh4FIfztkIt4yfhCeFnnlktqWFTbppAT5nE4HRWkZmSBpm9u5NXoAIORqPBPZ1aIBl0RbQfzBXnU2qc3cV.Ynx4rkFI; NYT-wpAB=0002|1&0010|2&0011|3&0012|1";

    actual = get_weather_code_from_cookie(cookiestrc, cookiename);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
    const char * cookiestrd = "NYT-Edition=edition|US; localytics.js=1; RMID=007f01002c15543d3ed50004; madison_user_id=3tO7Obk_Sg6uI4lzodnFTQ; madison_guest=false; s_pers=%20s_vsn_nytimesglobal_1%3D7844460248015%7C1730135643028%3B; __gads=ID=b7772508b463357f:T=1414525015:S=ALNI_MYUerI95dT7BeEuYt-8nbA3yaoQ-Q; wordpress_test_cookie=WP+Cookie+check; wordpress_logged_in_=scott.blumenthal%7C1414785730%7Cdd4286dfc0d60c4f7e605a51dd4614c5; fbm_9869919170=base_domain=.nytimes.com; nyt-recommend=1; __utma=69104142.778352587.1413318396.1415723158.1415983661.32; __utmc=69104142; __utmz=69104142.1415212413.29.12.utmcsr=localhost.nytimes.com:3001|utmccn=(referral)|utmcmd=referral|utmcct=/portal; nyt-d=101.000000000NAI00000C9Iio1f6ous0z1Iq/0eVpii0qApjv1eUY4r0M4meE1/FImH0AUY4r0C1sCa0f6mf/0zB0SA1w0mSI0M6tmN0vCp0g0aVnyU0K2Wnv0eCoir0kVnyU032W9v0UUWC70I4HeU0D77np1hStHe1rTcD1@c194bf3e/378764d3; _dycmc=17; _dyuss_undefined=8; _dyus_undefined=4%7C0%7C0%7C4%7C0%7C0.17.1398875133463.1416256906833.1022457.0%7C320%7C47%7C10%7C114%7C1%7C0%7C0%7C0%7C0%7C0%7C0%7C1%7C0%7C3%7C4%7C0%7C0%7C4%7C4%7C7%7C0%7C0%7C0; _ga=GA1.2.778352587.1413318396; welcomeviewed=1; nyt-m=896A6446A903792D23C341A88595492C&e=i.1420088400&t=i.10&v=i.0&n=i.2&g=i.0&rc=i.0&er=i.1415720418&vr=l.4.64.240.307.192&pr=l.4.152.799.1466.948&vp=i.739&gf=l.10.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&ft=i.0&fv=i.0&rl=l.1.-1&cav=i.6&imu=i.1&igu=i.1&prt=i.5&kid=i.1&ica=i.0&iue=i.1&ier=i.0&iub=i.0&ifv=i.0&igd=i.0&iga=i.0&imv=i.0&igf=i.0&iru=i.0&ird=i.0&ira=i.0&iir=i.1&l=l.15.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&gl=l.2.-1.-1&gb=l.3.0.5.1416286800; _dysvar_8765260=63%253A@%253Afeeds.@.64%253A@%253Apolitics.@.65%253A@%253Ainteractive; _dyuss_8765260=89; _dycst=tg.m.frv1.ms.ltos.c.; _dycnoabc=1417552237877; _dyus_8765260=694%7C0%7C0%7C6%7C0%7C0.0.1398091900829.1415140239176.17048338.0%7C307%7C45%7C10%7C114%7C9%7C9%7C0%7C0%7C5%7C4%7C0%7C18%7C12%7C0%7C1%7C10%7C4%7C18%7C25%7C52%7C14%7C7%7C62; circgroup=store; _cb_ls=1; __utma=28590334.778352587.1413318396.1416594906.1417620206.2; __utmb=28590334.6.8.1417620228850; NYT_W2=New%20YorkNYUS|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP; __utmc=28590334; __utmz=28590334.1416594906.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); _chartbeat2=tH6YdLrhnyDMUI6M.1397144713343.1417621166239.0000000000000001; rsi_segs=H07707_11003|H07707_10415|H07707_11063|D08734_70059|H07707_10207|D08734_70105|D08734_70509|D08734_70751|D08734_72008|D08734_72078|H07707_10044|H07707_10577|H07707_10638|H07707_11173|H07707_11174|H07707_11207|H07707_11175|H07707_11176|H07707_11178|H07707_11183|H07707_11187|H07707_11194|H07707_11195|H07707_11196|H07707_0; ab1=yes; ab2=yes; ab3=yes; _dyuss_8765283=6; _dycst=tg.m.frv3.ms.ltos.c.; _dycnoabc=1417621169989; NYT-mab=%7B%221%22%3A%22RI%3A5%22%7D; WT_FPC=id=8ad5d99b-3fc9-4d66-820c-24c260f59c62:lv=1417621210781:ss=1417619144492; _dy_geo=US.NA; krux_segs=n58825q2q%7Cooo3l4e96%7Cohph8wqcu%7Cm8yi6fn0f%7Couxjsml96%7Coeh287nzm%7Copjtfyu92%7Cn7szfo3xl%7Corj54ice4%7Com4sj6shu%7Cmimpufxui%7Cn7seltb6y%7Cn7szfoqt6%7Cn7szfomx3%7Cosewaclj3%7Colkt7d9x1%7Cnr9v9llm0%7Coxtp6iwgj%7Cm9ewee5pq%7Clmviw2vot%7Cn7szfp6hd%7Cn7szflyme%7Cn7szfp1ky%7Cm9pwujheh%7Cn7szfl4pr%7Cm8yye40vf%7Cobz8gonvw%7Cosihzsglx%7Cn7szfq00g%7Cosepjej6x%7Cogpp6a8ph%7Cn7szfnicn%7Coo7ak7npn%7Cmithviff8%7Coj2aw1ioh%7Cm9pwg0j70%7Corqp3nlu7%7Cmf94g1ng5%7Cosfb5x7ni%7Cn7szfqu4p%7Cn7szfpd5c%7Cn7szfo8qn%7Cosiv2aaqb%7Cn7szfpxrm%7Cos7bpagdd%7Copz7ovkmb%7Cose3hh4uj%7Coktlxay2x%7Cllf28yvry%7Cnxjo4r9x2%7Cn7szfpiuz%7Cn7szfn5z4%7Cor63eu8tq%7Cnxq8cvp69%7Consdjs5d5%7Cn55py3t5z%7Cn7szfozqo%7Cn4l4i077e%7Cn7szfpsa8%7Cmg1nsx1fv%7Cni23h8v7i%7Coe2qe4v6h%7Cnts9co5pn%7Cosmd7fhhy%7Cnv0z088vj%7Cm5m11hxs9%7Cnjr5hi4dg%7Cn7szfotw4%7Cn3qirawfw%7Cn7szfovgr%7Cor63qghhk%7Cn7szfpg8d%7Cn7szfq3ce%7Coj2f8sugv%7Col8a2b12r%7Com4xgijd5%7Cosia5rtou%7Cn7szfqg9h%7Cn7szfp3w0%7Cmith0bdg1%7Cm8yxv39ew%7Cmk1bczyjt%7Coi15lc4ig%7Corrl8dtnr%7Cnvjujpjr5%7Cn7szfohm8%7Cnvjps5n77%7Com37mgsm0%7Cm9pwj9zwc%7Colij9j24i%7Cosinwlerj%7Corfj4mfif%7Clmviretav%7Cmitha2f0p%7Cosd9mpckx%7Cosfjeop7w%7Cn7szfnfqa%7Copz7s44rg%7Copvlqddbx%7Cn7szfm50i%7Cn7szfqqcb%7Cmu2yn384m%7Cn7szfqkrg%7Coygqn8xqq%7Cose9dhiwy%7Cmiticd6en%7Coygp13c0e%7Cosiap3fde%7Cn7szfmnx5%7Cosbq85ja6%7Cor6nmdiqi%7Consa2ur7h%7Cm5qw3j39o%7Cn7szfoflt%7Cmk1apcr0l%7Cn7szfopag%7Cmimp808l7%7Cop628novu%7Cnnebseogg%7Conzln8357%7Cor63hay7s%7Clmvity5kb%7Cocuqd5reh%7Cot62o7t28%7Cnuzmoezkg%7Cn7szfmfna%7Cn7szfnpkk%7Cookj64uiq%7Cn7szfp9zx%7Cn7szfokt4%7Cm9h4kcgmq%7Cosh3bzwwo%7Cn7szfpn20%7Cmkdw07f0t%7Coypsztoyt%7Cn0jujk1xo%7Cn7szfodpt%7Cn7szfmbfg%7Cm8yjzdn6h%7Cn7szfnjri%7Cnv0695lgs%7Cnjrz9tpye%7Cmimr5pw0n%7Conrl71gcz%7Coonzp0li4%7Cosbscev80%7Cor7a8d230%7Cmil0ydr2y%7Conrjxld9v%7Cmgy7i5i8n%7Cor63arfps%7Col44qk36m%7Comvfswdjz%7Coe92g4cti%7Cow6bxaj2q%7Coe5xlqm5g%7Cosbun07ae%7Colokhjnjo%7Cor624qgz2%7Cm8yynb42j; nyt-a=2c61af26c5e5dd3243e5dacee20368db; _chartbeat4=t=BO41WDCIZ6YKNL9NxGUYXYZ2Whf&E=0&x=0&c=8.05&y=5051&w=429; _dyus_8765283=850%7C0%7C0%7C0%7C0%7C0.0.1397144713754.1417621169050.20476455.0%7C336%7C49%7C11%7C114%7C6%7C0%7C0%7C0%7C0%7C0%7C0%7C6%7C0%7C0%7C0%7C0%7C2%7C6%7C0%7C6%7C62%7C10%7C2; _dyrc=; adxcl=t*3e882=547fea4f:1415163821|t*3fe28=54813bcf:1415283269|t*3f9e8=54813bcf:1415287349|t*400ca=54828d4f:1415373334|t*40156=5485304f:1415584888|l*3bfc4=6013964f:1|t*40711=54a2314f:1417460577|t*3d7de=54ab6bcf:1415144032|lcig=55c42d3f:1|l*31068=566cfb4f:1|t*3f9cc=54c9be4f:1416238182|l*352b7=55c42d3f:1; adxcs=s*3eb33=0:1|s*3eb34=0:1|s*3fc77=0:1|s*3e7a3=0:1|s*3ee80=0:1|stiffany-sur=0:3|s*3f2f6=0:1|s*400ca=0:1|s*3f65b=0:1|s*401a6=0:1|s*3a3d4=0:1|si=0:1|s*3ef58=0:2|s*3f1eb=0:1|s*3f232=0:1; NYT-S=488tnv36oe3LC4x8aE6wiJBUN9UMCN5cRph1fqDAt4FAo8/Tmo1TI8r4dnlfH.PgAlyITOtMElPUh/1BHDL5FwxTgU/uxaXj5EheKVKOJ3FDzatRBLydWjyKvJJp.6isWKAOv4Pg3StLRJXdiICg4xQbQEigbFF5r9EA3S3wXCb8hRCPVdhsyeEbQQSz4dVrWCDjRfwsa3OHQnYGGpMlOwgep2harHhSvXpFdwHys/y/cCQx0aGXKsAHLKZUTSaBQLiew/8jh4FIfztkIt4yfhCeFnnlktqWFTbppAT5nE4HRWkZmSBpm9u5NXoAIORqPBPZ1aIBl0RbQfzBXnU2qc3cV.Ynx4rkFI; NYT-wpAB=0002|1&0010|2&0011|3&0012|1";

    actual = get_weather_code_from_cookie(cookiestrd, cookiename);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
}


void test_GetCookie(void)
{
  const char * cookiestra = "NYT-Edition=edition|US; localytics.js=1; RMID=007f01002c15543d3ed50004; madison_user_id=3tO7Obk_Sg6uI4lzodnFTQ; madison_guest=false; s_pers=%20s_vsn_nytimesglobal_1%3D7844460248015%7C1730135643028%3B; __gads=ID=b7772508b463357f:T=1414525015:S=ALNI_MYUerI95dT7BeEuYt-8nbA3yaoQ-Q; wordpress_test_cookie=WP+Cookie+check; wordpress_logged_in_=scott.blumenthal%7C1414785730%7Cdd4286dfc0d60c4f7e605a51dd4614c5; fbm_9869919170=base_domain=.nytimes.com; nyt-recommend=1; __utma=69104142.778352587.1413318396.1415723158.1415983661.32; __utmc=69104142; __utmz=69104142.1415212413.29.12.utmcsr=localhost.nytimes.com:3001|utmccn=(referral)|utmcmd=referral|utmcct=/portal; nyt-d=101.000000000NAI00000C9Iio1f6ous0z1Iq/0eVpii0qApjv1eUY4r0M4meE1/FImH0AUY4r0C1sCa0f6mf/0zB0SA1w0mSI0M6tmN0vCp0g0aVnyU0K2Wnv0eCoir0kVnyU032W9v0UUWC70I4HeU0D77np1hStHe1rTcD1@c194bf3e/378764d3; _dycmc=17; _dyuss_undefined=8; _dyus_undefined=4%7C0%7C0%7C4%7C0%7C0.17.1398875133463.1416256906833.1022457.0%7C320%7C47%7C10%7C114%7C1%7C0%7C0%7C0%7C0%7C0%7C0%7C1%7C0%7C3%7C4%7C0%7C0%7C4%7C4%7C7%7C0%7C0%7C0; _ga=GA1.2.778352587.1413318396; welcomeviewed=1; nyt-m=896A6446A903792D23C341A88595492C&e=i.1420088400&t=i.10&v=i.0&n=i.2&g=i.0&rc=i.0&er=i.1415720418&vr=l.4.64.240.307.192&pr=l.4.152.799.1466.948&vp=i.739&gf=l.10.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&ft=i.0&fv=i.0&rl=l.1.-1&cav=i.6&imu=i.1&igu=i.1&prt=i.5&kid=i.1&ica=i.0&iue=i.1&ier=i.0&iub=i.0&ifv=i.0&igd=i.0&iga=i.0&imv=i.0&igf=i.0&iru=i.0&ird=i.0&ira=i.0&iir=i.1&l=l.15.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1.-1&gl=l.2.-1.-1&gb=l.3.0.5.1416286800; _dysvar_8765260=63%253A@%253Afeeds.@.64%253A@%253Apolitics.@.65%253A@%253Ainteractive; _dyuss_8765260=89; _dycst=tg.m.frv1.ms.ltos.c.; _dycnoabc=1417552237877; _dyus_8765260=694%7C0%7C0%7C6%7C0%7C0.0.1398091900829.1415140239176.17048338.0%7C307%7C45%7C10%7C114%7C9%7C9%7C0%7C0%7C5%7C4%7C0%7C18%7C12%7C0%7C1%7C10%7C4%7C18%7C25%7C52%7C14%7C7%7C62; circgroup=store; _cb_ls=1; __utma=28590334.778352587.1413318396.1416594906.1417620206.2; __utmb=28590334.6.8.1417620228850; __utmc=28590334; __utmz=28590334.1416594906.1.1.utmcsr=(direct)|utmccn=(direct)|utmcmd=(none); _chartbeat2=tH6YdLrhnyDMUI6M.1397144713343.1417621166239.0000000000000001; rsi_segs=H07707_11003|H07707_10415|H07707_11063|D08734_70059|H07707_10207|D08734_70105|D08734_70509|D08734_70751|D08734_72008|D08734_72078|H07707_10044|H07707_10577|H07707_10638|H07707_11173|H07707_11174|H07707_11207|H07707_11175|H07707_11176|H07707_11178|H07707_11183|H07707_11187|H07707_11194|H07707_11195|H07707_11196|H07707_0; ab1=yes; ab2=yes; ab3=yes; _dyuss_8765283=6; _dycst=tg.m.frv3.ms.ltos.c.; _dycnoabc=1417621169989; NYT-mab=%7B%221%22%3A%22RI%3A5%22%7D; WT_FPC=id=8ad5d99b-3fc9-4d66-820c-24c260f59c62:lv=1417621210781:ss=1417619144492; _dy_geo=US.NA; krux_segs=n58825q2q%7Cooo3l4e96%7Cohph8wqcu%7Cm8yi6fn0f%7Couxjsml96%7Coeh287nzm%7Copjtfyu92%7Cn7szfo3xl%7Corj54ice4%7Com4sj6shu%7Cmimpufxui%7Cn7seltb6y%7Cn7szfoqt6%7Cn7szfomx3%7Cosewaclj3%7Colkt7d9x1%7Cnr9v9llm0%7Coxtp6iwgj%7Cm9ewee5pq%7Clmviw2vot%7Cn7szfp6hd%7Cn7szflyme%7Cn7szfp1ky%7Cm9pwujheh%7Cn7szfl4pr%7Cm8yye40vf%7Cobz8gonvw%7Cosihzsglx%7Cn7szfq00g%7Cosepjej6x%7Cogpp6a8ph%7Cn7szfnicn%7Coo7ak7npn%7Cmithviff8%7Coj2aw1ioh%7Cm9pwg0j70%7Corqp3nlu7%7Cmf94g1ng5%7Cosfb5x7ni%7Cn7szfqu4p%7Cn7szfpd5c%7Cn7szfo8qn%7Cosiv2aaqb%7Cn7szfpxrm%7Cos7bpagdd%7Copz7ovkmb%7Cose3hh4uj%7Coktlxay2x%7Cllf28yvry%7Cnxjo4r9x2%7Cn7szfpiuz%7Cn7szfn5z4%7Cor63eu8tq%7Cnxq8cvp69%7Consdjs5d5%7Cn55py3t5z%7Cn7szfozqo%7Cn4l4i077e%7Cn7szfpsa8%7Cmg1nsx1fv%7Cni23h8v7i%7Coe2qe4v6h%7Cnts9co5pn%7Cosmd7fhhy%7Cnv0z088vj%7Cm5m11hxs9%7Cnjr5hi4dg%7Cn7szfotw4%7Cn3qirawfw%7Cn7szfovgr%7Cor63qghhk%7Cn7szfpg8d%7Cn7szfq3ce%7Coj2f8sugv%7Col8a2b12r%7Com4xgijd5%7Cosia5rtou%7Cn7szfqg9h%7Cn7szfp3w0%7Cmith0bdg1%7Cm8yxv39ew%7Cmk1bczyjt%7Coi15lc4ig%7Corrl8dtnr%7Cnvjujpjr5%7Cn7szfohm8%7Cnvjps5n77%7Com37mgsm0%7Cm9pwj9zwc%7Colij9j24i%7Cosinwlerj%7Corfj4mfif%7Clmviretav%7Cmitha2f0p%7Cosd9mpckx%7Cosfjeop7w%7Cn7szfnfqa%7Copz7s44rg%7Copvlqddbx%7Cn7szfm50i%7Cn7szfqqcb%7Cmu2yn384m%7Cn7szfqkrg%7Coygqn8xqq%7Cose9dhiwy%7Cmiticd6en%7Coygp13c0e%7Cosiap3fde%7Cn7szfmnx5%7Cosbq85ja6%7Cor6nmdiqi%7Consa2ur7h%7Cm5qw3j39o%7Cn7szfoflt%7Cmk1apcr0l%7Cn7szfopag%7Cmimp808l7%7Cop628novu%7Cnnebseogg%7Conzln8357%7Cor63hay7s%7Clmvity5kb%7Cocuqd5reh%7Cot62o7t28%7Cnuzmoezkg%7Cn7szfmfna%7Cn7szfnpkk%7Cookj64uiq%7Cn7szfp9zx%7Cn7szfokt4%7Cm9h4kcgmq%7Cosh3bzwwo%7Cn7szfpn20%7Cmkdw07f0t%7Coypsztoyt%7Cn0jujk1xo%7Cn7szfodpt%7Cn7szfmbfg%7Cm8yjzdn6h%7Cn7szfnjri%7Cnv0695lgs%7Cnjrz9tpye%7Cmimr5pw0n%7Conrl71gcz%7Coonzp0li4%7Cosbscev80%7Cor7a8d230%7Cmil0ydr2y%7Conrjxld9v%7Cmgy7i5i8n%7Cor63arfps%7Col44qk36m%7Comvfswdjz%7Coe92g4cti%7Cow6bxaj2q%7Coe5xlqm5g%7Cosbun07ae%7Colokhjnjo%7Cor624qgz2%7Cm8yynb42j; nyt-a=2c61af26c5e5dd3243e5dacee20368db; _chartbeat4=t=BO41WDCIZ6YKNL9NxGUYXYZ2Whf&E=0&x=0&c=8.05&y=5051&w=429; _dyus_8765283=850%7C0%7C0%7C0%7C0%7C0.0.1397144713754.1417621169050.20476455.0%7C336%7C49%7C11%7C114%7C6%7C0%7C0%7C0%7C0%7C0%7C0%7C6%7C0%7C0%7C0%7C0%7C2%7C6%7C0%7C6%7C62%7C10%7C2; _dyrc=; adxcl=t*3e882=547fea4f:1415163821|t*3fe28=54813bcf:1415283269|t*3f9e8=54813bcf:1415287349|t*400ca=54828d4f:1415373334|t*40156=5485304f:1415584888|l*3bfc4=6013964f:1|t*40711=54a2314f:1417460577|t*3d7de=54ab6bcf:1415144032|lcig=55c42d3f:1|l*31068=566cfb4f:1|t*3f9cc=54c9be4f:1416238182|l*352b7=55c42d3f:1; adxcs=s*3eb33=0:1|s*3eb34=0:1|s*3fc77=0:1|s*3e7a3=0:1|s*3ee80=0:1|stiffany-sur=0:3|s*3f2f6=0:1|s*400ca=0:1|s*3f65b=0:1|s*401a6=0:1|s*3a3d4=0:1|si=0:1|s*3ef58=0:2|s*3f1eb=0:1|s*3f232=0:1; NYT-S=488tnv36oe3LC4x8aE6wiJBUN9UMCN5cRph1fqDAt4FAo8/Tmo1TI8r4dnlfH.PgAlyITOtMElPUh/1BHDL5FwxTgU/uxaXj5EheKVKOJ3FDzatRBLydWjyKvJJp.6isWKAOv4Pg3StLRJXdiICg4xQbQEigbFF5r9EA3S3wXCb8hRCPVdhsyeEbQQSz4dVrWCDjRfwsa3OHQnYGGpMlOwgep2harHhSvXpFdwHys/y/cCQx0aGXKsAHLKZUTSaBQLiew/8jh4FIfztkIt4yfhCeFnnlktqWFTbppAT5nE4HRWkZmSBpm9u5NXoAIORqPBPZ1aIBl0RbQfzBXnU2qc3cV.Ynx4rkFI; NYT-wpAB=0002|1&0010|2&0011|3&0012|1; NYT_W2=New%20YorkNYUS|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP; abc=123";
    printf("Cookie is this long: %lu\n\n", strlen(cookiestra));
    const char* cookiename = "NYT_W2";
    const char* expected = "New%20YorkNYUS|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP";
    char* actual = get_cookie(cookiestra, cookiename);
    TEST_ASSERT_EQUAL_STRING(expected, actual);
    free(actual);
    }


void test_GetEmptyCookie()
{
  const char* cookiestra = "This=name; NYT_W2=; Other=abc";
  const char* cookiename = "NYT_W2";
  const char* expected = NULL;
  char* actual = get_cookie(cookiestra, cookiename);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
  free(actual);
}


void test_GetEmptyCookieA()
{
  const char* cookiestra = "This=name; NYT_W2  =Some; Other=abc";
  const char* cookiename = "NYT_W2";
  const char* expected = "Some";
  char* actual = get_cookie(cookiestra, cookiename);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
  free(actual);
}

void test_GetEmptyCookieAb()
{
  const char* cookiestra = "This=name; NYT_W2NYT_W2  =Some; Other=abc";
  const char* cookiename = "NYT_W2";
  const char* expected = "Some";
  char* actual = get_cookie(cookiestra, cookiename);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
  free(actual);
}


void test_GetEmptyCookieB()
{
  const char* cookiestra = "This=name; NYT_W2  =Some  ; Other=abc";
  const char* cookiename = "NYT_W2";
  const char* expected = "Some";

  char* actual = get_cookie(cookiestra, cookiename);
  printf("I have:%s:\n", actual);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
  free(actual);
}

void test_GetEmptyCookieC()
{
  const char* cookiestra = "NYT_W2=New%20YorkNYUSÿ|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP||";
  const char* cookiename = "NYT_W2";
  const char* expected = "New%20YorkNYUSÿ|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP||";

  char* actual = get_cookie(cookiestra, cookiename);
  printf("I have:%s:\n", actual);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
  free(actual);
}

/*
void test_GetEmptyCookieD()
{
  char* cookiestra = "NYT_W2=New%20YorkNYUSÿ|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP||";
  const char* cookiename = "NYT_W2";
  const char* expected = "New%20YorkNYUSÿ";
  char* actual = get_weather_code_from_cookie(cookiestra, cookiename);
  printf("I have:%s:\n", actual);
  TEST_ASSERT_EQUAL_STRING(expected, actual);

}

void test_GetEmptyCookieE()
{
  char* cookiestra = "NYT_W2=IndianapolisINUSÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ|ChicagoILUS|London--UK|Los%20AngelesCAUS|San%20FranciscoCAUS|Tokyo--JP||";
  const char* cookiename = "NYT_W2";
  const char* expected="IndianapolisINUSÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿÿ";
  char* actual = get_weather_code_from_cookie(cookiestra, cookiename);
  printf("I have:%s:\n", actual);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}

void test_GetEmptyCookieF()
{
  char* cookiestra = "NYT_W2=IndianapolisINUS";
  const char* cookiename = "NYT_W2";
  const char* expected="IndianapolisINUS";
  char* actual = get_weather_code_from_cookie(cookiestra, cookiename);
  printf("I have:%s:\n", actual);
  TEST_ASSERT_EQUAL_STRING(expected, actual);
}


*/
void test_CloseMMDB() {
    close_mmdb(&mmdb_handle);
}
