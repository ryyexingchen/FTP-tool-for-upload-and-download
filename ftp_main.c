#include "ftp.h"

char                    *ip;
char 					buf[BUF_SIZE];
char		     	    cmd[CMD_LEN];

/*=================================
name:Usage
param:
return:
function：用于显示提示信息
===================================*/

static void Usage(){
	printf("The instuctions you can input as follows:\n");
	printf("upload -- for file upload\n");
	printf("download -- for file download\n");
	printf("end -- to exit the program\n");
}

int main(int argc, char **argv)
{
    ip = malloc(IP_LEN);
	char ins[CMD_LEN];

    /*Get the server IP*/
	strncpy(ip, SERVER_IP, IP_LEN);
	
	/*main loop*/
	Usage();
	while(1)
	{
		printf("Please input your instuction:\n");
		scanf("%255s", ins);
		if(!strcmp(ins,"upload"))
		{
			/*upload files*/
			if(file_upload() == -1)
			{	
				return -1;
			}
		}
		else if(!strcmp(ins,"download"))
		{
			/*download files*/
			if(file_download() == -1)
			{		
				return -1;
			}
		}
		else if(!strcmp(ins,"end"))
		{
			break;
		}
		else
		{
			printf("The instruction is illegal.\n");
		}
	}
	printf("The program has ended seccessfully.\n");
    return 0;
}

