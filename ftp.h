#ifndef _FTP_H
#define _FTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>


#define RWXALL (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IXOTH)

#define PORT                21
#define SERVER_IP			"192.168.43.237"
#define BUF_SIZE            65536
#define CMD_LEN             64
#define IP_LEN              32
#define UPLOAD_DICTIONARY   "files"
#define DOWNLOAD_DICTIONARY "files"
#define TEMP_PATH			"./temp"
#define USERNAME            "ftpuser"
#define PASSWORD            "123"

#define MAX_FILENAME_LENGTH 256
#define MAX_FILEPATH_LENGTH 1024
#define THREAD_NUM			4
#define MAX_MEM_BUF 		40960
#define MAX_PATH_NUM		1024

extern char                    	*ip;
extern char 					buf[BUF_SIZE];
extern char		     	    	cmd[CMD_LEN];

int compute_data_port(char *port_buf);
int connect_server();
void login(int socket_fd);
int get_data_port(int socket_fd);
void quit(int socket_fd);
int file_upload();
void* thread_download(void* arg);
int file_download();
#endif
