#ifndef _ROUTER_SET
#define _ROUTER_SET
#include "hd_wifi.h"
#include "router_defs.h"

enum client_sta{
	CLIENT_ON,
	CLIENT_OFF
};

#ifdef OPENWRT_X1000
typedef struct _wifilist
{
	char ssid[64];
	char encryption[32];
	char key[32];
}wifilist_t, *pwifilist_t;


enum wifi_mod
{
	M2G=2,
	M5G=5
};

int get_wifi_mode();
wifilist_t *get_wifi_list(int wifimode,int *number);

#endif


int set_wifi_settings(hd_wifi_info *m_wifi_info);
int get_wifi_settings(hd_wifi_info *m_wifi_info);
int set_remote_ap(hd_remoteap_info *m_remote_info);
int set_to_repeart(hd_remoteap_info *m_remote_info);
int get_repeart_status(hd_remoteap_info *m_remote_info);
int get_remote_ap(hd_remoteap_info *m_remote_info);
int _get_wired_con_mode(wired_con_mode_array *m_wired_con_array);
int _set_wired_con_mode(wired_con_mode_array *m_wired_con_array);
int _get_client_status();
int _set_client_status(int status);
int _set_forget_wifi_info(char *pbssid);

#endif

