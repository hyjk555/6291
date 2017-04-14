
#include "msg.h"

int _handle_client_json_req(ClientTheadInfo *client_info)
{
 //   return api_process(client_info);
}


void main()
{
		char ret_buf[RET_BUF_LEN];
		char code[CODE_LEN]="\0";
		char sid[SID_LEN]="\0";
		char fw_sid[33]="\0";
		char type[32]="\0";
		char pid[33]="\0";
		char tag[256]="\0";
		char tmp_buf[256]="\0";

		int i,j,k;
		char *web_str=NULL;
		int ret=0;

		char uci_option_str[UCI_BUF_LEN]="\0";
		ctx=uci_alloc_context();
		
		strcpy(uci_option_str,"system.@system[0].sid");			//name
		uci_get_option_value(uci_option_str,fw_sid);
		memset(uci_option_str,'\0',UCI_BUF_LEN);

		
		printf("Content-type:text/plain\r\n\r\n");

		if((web_str=GetStringFromWeb())==NULL)
		{
			sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"Can not get any parameters.\"}");
			printf("%s",ret_buf);
			fflush(stdout);
			uci_free_context(ctx);
			return ;

		}
		processString(web_str,SID,sid);		
		
		processString(web_str,CODE,code);		
		
		processString(web_str,PID,pid);	

		processString(web_str,"tag",tag); 

		p_debug("tag=

		if(!strcmp(sid,fw_sid)){//�ǹ���Ա
			//remove
			if(!strcmp(pid,"")){
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":4,\"errorMessage\":\"pid is empty\"}");
				goto exit;
			}	
			//if(strstr(vid,',')!=NULL)
			{
			
				sprintf(buf,"{\"%s\":{\"pid\":%s,\"tag\":%s}}",ALBUM_SET,pid,tag);
				#if 0
				//char *str=strstr(vid,',');
	//			for(i=0,j=0,k=0;i<strlen(vid);i++)
				for(i=0,j=0,k=0;i<strlen(pid);i++)
					{
						if(pid[i]==',')
						{ 
							j++;
							//strncpy(tmp_vid,vid+i,(i+1));
							sprintf(tmp_buf,"\"%s\",",tmp_vid);
							strcat(buf,tmp_buf);
							memset(tmp_vid,0,strlen(tmp_vid));
							k=0;
						}
						else {
							tmp_vid[k]=pid[i];
							k++;
						}
				}

				sprintf(tmp_buf,"\"%s\"",tmp_vid);
				strcat(buf,tmp_buf);
				strcat(buf,"]}}");
				#endif
			}
			//else{
			//	sprintf(buf,"{\"%s\":{\"%s\":\"%s\"}}",TASK_REMOVE,VID,);
			//}

			p_debug("buf=====%s",buf);
		
			
			ret = notify_server();
			if(ret <= 0){//ͨѶ����
				sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":13,\"errorMessage\":\"Communication Error with dm_letv\"}");
			}else//���յ���Ϣ
				sprintf(ret_buf,"%s",buf);
		}else{
			sprintf(ret_buf,"%s","{\"status\":0,\"data\":{},\"errorCode\":1,\"errorMessage\":\"Not Admin\"}");
		} 
exit:
		printf("%s",ret_buf);
		fflush(stdout);
		free(web_str);
		uci_free_context(ctx);
		return ;
}
