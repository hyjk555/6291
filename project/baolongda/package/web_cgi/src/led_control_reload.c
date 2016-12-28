#include <errno.h>
#include <stdlib.h>             
#include <ctype.h>
#include <time.h>           
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>      
#include <setjmp.h>

#include <netinet/in.h>
#include <linux/wireless.h>
#include <sys/ioctl.h>
#include <sys/types.h>

#include <sys/mman.h>
#include <sys/types.h>        
#include <sys/socket.h>      
#include <sys/stat.h>         


#define rtl_encryp_control "/proc/rtl_encryp_control"

#define power_gotoload_ctrl  4

#define power_reset_ctrl  3



int main(int argc, char** argv)
{

		int fh=NULL;
		fh=open(rtl_encryp_control, O_RDWR);
		if(fh== NULL)
		{
			printf("fh null .\n");
			goto error;
		}
//		printf("tt=%s\n",argv[1]);
		if ( !strcmp(argv[1], "reload") )
		{
			if(ioctl(fh, power_gotoload_ctrl, 0) < 0)
			{
				printf("upload_bin_end error \n\n");
			}

		}
		if ( !strcmp(argv[1], "reset") )
		{
			if(ioctl(fh, power_reset_ctrl, 0) < 0)
			{
				printf("upload_bin_end error \n\n");
			}
		}
		close(fh);
		return 0;
error:
			return -1;

}

