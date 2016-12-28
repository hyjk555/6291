

/*
 * =============================================================================
 *
 *       Filename:  api_process.c
 *
 *    Description:  longsys sever module.
 *
 *        Version:  1.0
 *        Created:  2014/10/29 14:51:25
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Oliver (), 515296288jf@163.com
 *   Organization:  
 *
 * =============================================================================
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mntent.h>
#include <sys/vfs.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>     /*Unix 标准函数定义*/
#include <sys/types.h>  
#include <locale.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>
#include "file_json.h"
#include "config.h"
#include "get_service_list.h"
#include "session.h"
#include "router_task.h"
#include "task/category_task.h"
#include "task/scan_task.h"


int dm_get_version(struct conn *c)
{
	int res = 0;
	JObj* response_json=JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	strcpy(c->ver,DM_FW_VERSION);
	JObj *response_data_array = JSON_NEW_ARRAY();
	JObj* ver_info = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(ver_info, "ver",JSON_NEW_OBJECT(c->ver,string));
	JSON_ARRAY_ADD_OBJECT (response_data_array,ver_info);
	JSON_ADD_OBJECT(response_json, "data", response_data_array);
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	return 0;
}
extern struct hd_dnode *router_dn;
int dm_login(struct conn *c)
{
	int res = 0;
	time_t timep;
	struct tm *p;
	JObj* response_json = JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	JObj *response_data_array = JSON_NEW_ARRAY();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	time(&timep);
	p = localtime(&timep);
	timep = mktime(p);
	DMCLOG_D("time()->localtime()->mktime():%d",timep);
	c->cur_time = timep;
	DMCLOG_D("device_name = %s",c->deviceName);
	res = dm_usr_login(c);
	if(res < 0)
	{
		c->error = ERROR_LOGIN;
		goto EXIT;
	}
	// 1:login
	JObj* session_info = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(session_info, "session",JSON_NEW_OBJECT(c->session,string));
	JSON_ARRAY_ADD_OBJECT (response_data_array,session_info);
	
	// 2:get version
	strcpy(c->ver,DM_FW_VERSION);
	JObj* ver_info = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(ver_info, "ver",JSON_NEW_OBJECT(c->ver,string));
	JSON_ARRAY_ADD_OBJECT (response_data_array,ver_info);

	// 3:get status changed
	c->statusFlag = dm_get_status_changed();
	if(c->statusFlag < 0)
	{
		c->error = ERROR_GET_STATUS_CHANGED;
		goto EXIT;
	}
	JObj* status_info = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(status_info, "statusFlag",JSON_NEW_OBJECT(c->statusFlag,int));
	JSON_ARRAY_ADD_OBJECT (response_data_array,status_info);

	// 4:get function list
	c->statusFlag = filetype_power_wifi_remoteap_access;
	JObj* func_list = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(func_list, "funcListFlag",JSON_NEW_OBJECT(c->statusFlag,int));
	JSON_ARRAY_ADD_OBJECT (response_data_array,func_list);
EXIT:
	JSON_ADD_OBJECT(response_json, "data", response_data_array);
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	return 0;
}

int dm_logout(struct conn *c)
{
	ENTER_FUNC();
	int res = 0;
	JObj* response_json = JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	res = dm_usr_logout(c);
	if(res  < 0)
	{
		c->error = ERROR_LOGOUT;
	}
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}


int dm_get_service_info(struct conn *c)
{
	int ret = 0;
	int i = 0;
	JObj* response_json=JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	service_list_t p_service_list;
	memset(&p_service_list,0,sizeof(service_list_t));
	ret = get_service_list(&p_service_list);
	DMCLOG_D("count = %d",p_service_list.count);
	if(ret < 0)
	{
		c->error = ERROR_GET_SERVICE_LIST;
	}
	JObj *response_data_array = JSON_NEW_ARRAY();
	for(i = 0;i < p_service_list.count;i++)
	{
		JObj *service_info = JSON_NEW_EMPTY_OBJECT();
		JSON_ADD_OBJECT(service_info, "name", JSON_NEW_OBJECT(p_service_list.service_info[i].name,string));
		JSON_ADD_OBJECT(service_info, "port", JSON_NEW_OBJECT(p_service_list.service_info[i].port,int));
		JSON_ARRAY_ADD_OBJECT (response_data_array,service_info);
	}
	JSON_ADD_OBJECT(response_json, "data", response_data_array);
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	ret = file_json_to_string(c,response_json);
	if(ret < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	return 0;
}

int _dm_del_client_info(struct conn *c)
{
	ENTER_FUNC();
	int res = 0;
	JObj* response_json=JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	#if 0
	extern struct usr_dnode *usr_dn;
	EnterCriticalSection(&c->ctx->mutex);
	display_usr_dnode(usr_dn);
	DMCLOG_D("client_ip = %s",c->client_ip);
	res = del_usr_from_list_for_descovery(&usr_dn,c->client_ip);
	LeaveCriticalSection(&c->ctx->mutex);
	if(res < 0)
	{
		DMCLOG_D("del client info error");
		c->error = ERROR_DEL_CLIENT_INFO;
		goto EXIT;
	}
	#else
	/*DMCLOG_D("c->ip = %s",c->client_ip);
	EnterCriticalSection(&c->ctx->mutex);
	res = handle_db_del_usr_for_ip(c->client_ip);
	LeaveCriticalSection(&c->ctx->mutex);
	if(res < 0)
	{
		c->error = DM_ERROR_DB_USER_TABLE;
		goto EXIT;
	}*/
	#endif
	extern struct hd_dnode *router_dn;
	EnterCriticalSection(&c->ctx->mutex);
	display_hd_dnode(router_dn);
	res = del_dev_from_list_for_ip(&router_dn,c->client_ip);
	LeaveCriticalSection(&c->ctx->mutex);
	if(res < 0)
	{
		DMCLOG_D("del client info error");
		c->error = ERROR_DEL_CLIENT_INFO;
		goto EXIT;
	}
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}
int dm_disk_scanning(struct conn *c)
{
	int res = 0;
	JObj* response_json=JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	notify_disk_scan_status(0);
	all_disk_t mAll_disk_t;
	int i = 0;
	memset(&mAll_disk_t,0,sizeof(all_disk_t));
	res = dm_get_storage(&mAll_disk_t);
	if(res != SQLITE_OK)
	{
		goto EXIT;
	}
 	DMCLOG_D("drive_count = %d",mAll_disk_t.count);
	if((res = dm_create_db_file(DATABASE,&mAll_disk_t)) != RET_SUCCESS)
	{
		DMCLOG_D("create db file error");
		goto EXIT;
	}

	for(i = 0;i < mAll_disk_t.count;i++)
	{
		if( mAll_disk_t.disk[i].is_scanning == 1)
		{
			//DMCLOG_D("path = %s,file_table_name = %s",mAll_disk_t.disk[i].path,mAll_disk_t.disk[i].file_table_name);
			if(create_scan_task(&mAll_disk_t.disk[i]) != RET_SUCCESS)
		    {
		        DMCLOG_D("start_scan_task failed");
		        goto EXIT;
		    }
		}
	}
	//notify_disk_scan_status(1);
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}

int dm_router_get_option(struct conn *c)
{
	int i = 0;
	int ret = 0;
	JObj* response_json=JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	/*JObj *response_data_array = JSON_NEW_ARRAY();
	for(i = 0; i < TAGHANDLE_NUM; i++)
	{
		JObj *cmd_info = JSON_NEW_EMPTY_OBJECT();
		JSON_ADD_OBJECT(cmd_info, "cmdID", JSON_NEW_OBJECT(1,int));
		JSON_ARRAY_ADD_OBJECT (response_data_array,cmd_info);
	}
	JSON_ADD_OBJECT(response_json, "data", response_data_array);*/
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	ret = file_json_to_string(c,response_json);
	if(ret < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	return 0;
}

int dm_router_get_status_changed(struct conn *c)
{
	ENTER_FUNC();
	int res = 0;
	JObj* response_json = JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	c->statusFlag = dm_get_status_changed();
	if(c->statusFlag < 0)
	{
		c->error = ERROR_GET_STATUS_CHANGED;
	}else{
		JObj *response_data_array = JSON_NEW_ARRAY();
		JObj* status_info = JSON_NEW_EMPTY_OBJECT();
		JSON_ADD_OBJECT(status_info, "statusFlag",JSON_NEW_OBJECT(c->statusFlag,int));
		JSON_ARRAY_ADD_OBJECT (response_data_array,status_info);
		JSON_ADD_OBJECT(response_json, "data", response_data_array);
	}
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}

int dm_router_get_func_list(struct conn *c)
{
	ENTER_FUNC();
	int res = 0;
	JObj* response_json = JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	c->statusFlag = filetype_power_wifi_remoteap_access;

	JObj *response_data_array = JSON_NEW_ARRAY();
	JObj* status_info = JSON_NEW_EMPTY_OBJECT();
	JSON_ADD_OBJECT(status_info, "statusFlag",JSON_NEW_OBJECT(c->statusFlag,int));
	JSON_ARRAY_ADD_OBJECT (response_data_array,status_info);
	JSON_ADD_OBJECT(response_json, "data", response_data_array);
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}

int dm_router_set_status_changed_listener(struct conn *c)
{
	ENTER_FUNC();
	int res = 0;
	JObj* response_json = JSON_NEW_EMPTY_OBJECT();
	JObj* header_json=JSON_NEW_EMPTY_OBJECT();
	if(c->error != 0)
	{
		DMCLOG_D("json cmd is error");
		goto EXIT;
	}
	if(c->session&&!*c->session)
	{
		DMCLOG_D("session is error");
		goto EXIT;
	}
	DMCLOG_D("c->cient_port = %d",c->client_port);
	EnterCriticalSection(&c->ctx->mutex);
	res = add_dev_to_list(&router_dn,c->session,c->client_port,c->statusFlag);
	LeaveCriticalSection(&c->ctx->mutex);
	if(res < 0)
	{
		c->error = ERROR_SET_STATUS_CHANGED;
	}
	display_hd_dnode(router_dn);
EXIT:
	JSON_ADD_OBJECT(header_json, "cmd", JSON_NEW_OBJECT(c->cmd,int));
	JSON_ADD_OBJECT(header_json, "seq", JSON_NEW_OBJECT(c->seq,int));
	JSON_ADD_OBJECT(header_json, "error", JSON_NEW_OBJECT(c->error,int));
	JSON_ADD_OBJECT(response_json, "header", header_json);
	res = file_json_to_string(c,response_json);
	if(res < 0)
	{
		JSON_PUT_OBJECT(response_json);
		return -1;
	}
	JSON_PUT_OBJECT(response_json);
	EXIT_FUNC();
	return 0;
}
/*int api_process(ClientTheadInfo *p_client_info)
{ 
	uint8_t i = 0;
	uint8_t switch_flag = 0;
	int res_sz;
	int ret = -1;
	char *cmd_buf = "input cmd is not finished!";
	for(i = 0; i < TAGHANDLE_NUM; i++)
	{
		if(p_client_info->cmd == all_tag_handle[i].tag)
		{
	       	 ret = all_tag_handle[i].tagfun(p_client_info);
		     switch_flag = 1;
		}
	}
	if(switch_flag == 0)
	{
		res_sz = strlen(cmd_buf);
		p_client_info->retstr = (char *)malloc(res_sz + 1);
		if(p_client_info->retstr == NULL)
		{
			return -1;
		}
	    strcpy(p_client_info->retstr,cmd_buf);
	}
	return 0;
}*/

int Parser_GetVersion(struct conn *c)
{
	
}
int Parser_Login(struct conn *c)
{
	int res = 0;
	JObj *data_json = JSON_GET_OBJECT(c->r_json,"data");
	if(data_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *para_json = JSON_GET_ARRAY_MEMBER_BY_ID(data_json,0);
	if(para_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *username_json = JSON_GET_OBJECT(para_json,"username");
	JObj *password_json = JSON_GET_OBJECT(para_json,"password");
	JObj *deviceType_json = JSON_GET_OBJECT(para_json,"deviceType");
	JObj *deviceUuid_json = JSON_GET_OBJECT(para_json,"deviceUuid");
	JObj *deviceName_json = JSON_GET_OBJECT(para_json,"deviceName");
	DMCLOG_D("header.cmd = %d",c->cmd);
	if(username_json == NULL||password_json == NULL||deviceType_json == NULL||deviceUuid_json == NULL||deviceName_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	strcpy(c->username,JSON_GET_OBJECT_VALUE(username_json,string));
	strcpy(c->password,JSON_GET_OBJECT_VALUE(password_json,string));
	c->deviceTpye = JSON_GET_OBJECT_VALUE(deviceType_json,int);
	char *deviceUuid = JSON_GET_OBJECT_VALUE(deviceUuid_json,string);
	if(deviceUuid == NULL)
	{
		DMCLOG_D("malloc error");
		c->error = SERVER_OUT_MEMORY;
		res = -1;
	}
	c->deviceUuid = (char *)malloc(strlen(deviceUuid) + 1);
	strcpy(c->deviceUuid,deviceUuid);
	char *deviceName = JSON_GET_OBJECT_VALUE(deviceName_json,string);
	if(deviceName == NULL)
	{
		DMCLOG_D("malloc error");
		c->error = SERVER_OUT_MEMORY;
		res = -1;
	}
	c->deviceName = (char *)malloc(strlen(deviceName) + 1);
	strcpy(c->deviceName,deviceName);
EXIT:
	if(c->r_json != NULL)
		JSON_PUT_OBJECT(c->r_json);
}
int Parser_Logout(struct conn *c)
{
	JObj *data_json = JSON_GET_OBJECT(c->r_json,"data");
	if(data_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *para_json = JSON_GET_ARRAY_MEMBER_BY_ID(data_json,0);
	if(para_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *session_json = JSON_GET_OBJECT(para_json,"session");
	DMCLOG_D("header.cmd = %d",c->cmd);
	if(session_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	char *session = JSON_GET_OBJECT_VALUE(session_json,string);
	if(session == NULL)
	{
		DMCLOG_D("session is NULL");
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}else if(strlen(session) < 16)
	{
		DMCLOG_D("session is normal");
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	memset(c->session,0,sizeof(c->session));
	strcpy(c->session,session);
EXIT:
	if(c->r_json != NULL)
			JSON_PUT_OBJECT(c->r_json);
}
int Parser_GetServiceInfo(struct conn *c)
{
	
}
int Parser_DelClientInfo(struct conn *c)
{
	DMCLOG_D("c->cmd = %d",c->cmd);
	JObj *data_json = JSON_GET_OBJECT(c->r_json,"data");
	if(data_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *para_json = JSON_GET_ARRAY_MEMBER_BY_ID(data_json,0);
	if(para_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *ip_json = JSON_GET_OBJECT(para_json,"ip");
	if(ip_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	strcpy(c->client_ip, JSON_GET_OBJECT_VALUE(ip_json,string));
	DMCLOG_D("cleint ip = %s",c->client_ip);
EXIT:
	if(c->r_json != NULL)
			JSON_PUT_OBJECT(c->r_json);

}
int Parser_DiskScanning(struct conn *c)
{
	return 0;
}
int Parser_RouterGetOption(struct conn *c)
{
	return 0;
}
int Parser_RouterGetStatusChanged(struct conn *c)
{
	return 0;
}
int Parser_RouterGetFuncList(struct conn *c)
{
	return 0;
}
int Parser_RouterSetStatusListen(struct conn *c)
{
	DMCLOG_D("c->cmd = %d",c->cmd);
	JObj *data_json = JSON_GET_OBJECT(c->r_json,"data");
	if(data_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *para_json = JSON_GET_ARRAY_MEMBER_BY_ID(data_json,0);
	if(para_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	JObj *port_json = JSON_GET_OBJECT(para_json,"port");
	JObj *statusFlag_json = JSON_GET_OBJECT(para_json,"statusFlag");
	if(port_json == NULL||statusFlag_json == NULL)
	{
		c->error = INVALIDE_COMMAND;
		goto EXIT;
	}
	c->client_port = JSON_GET_OBJECT_VALUE(port_json,int);
	c->statusFlag = JSON_GET_OBJECT_VALUE(statusFlag_json,int);
	DMCLOG_D("port = %d",c->client_port);
EXIT:
	if(c->r_json != NULL)
			JSON_PUT_OBJECT(c->r_json);

}
	


