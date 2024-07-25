#include "ftp.h"

/*=================================
name：compute_data_port
param：port_buf
return：data_port
function：计算当前数据端口
===================================*/

int compute_data_port(char *port_buf)
{
	int		port_low = 0, port_high = 0, count = 0;
	char		div[] = {'(', ',', ')'};
	char		*num = NULL;

	for(num = strtok(port_buf, div); num; num = strtok(NULL, div)){
		count++;
		if(count == 6)
			port_high = atoi(num);
		if(count == 7)
			port_low = atoi(num);
	}
	
	return (port_high << 8) + port_low;
}

/*=================================
name：connect_server
param：
return：socket_fd
function：连接服务器并返回socket_fd
===================================*/

int connect_server()
{
    /*Create a socket_fd: port, ip, and the address family protype have been writen in*/
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd < 0)
    {
        printf("Fail to create a socket to connect with server :%s\n", strerror(errno));
        return -1;
    }
	
    /* Connect with the server*/
	struct sockaddr_in server_addr;
	
    server_addr.sin_port = htons(PORT);
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);

    if(connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Fail to connect with the server %s :%s\n", ip, strerror(errno));
        return -1;
    }
    else
    {
    	recv(socket_fd, buf, BUF_SIZE, 0);
    	printf("%s\n", buf);
    	memset(buf, 0, BUF_SIZE);
    }
	printf("Server has been connected seccessfully.\n\n");
	return socket_fd;
}

/*=================================
name：login
param：socket_fd
return：
function：登录
===================================*/

void login(int socket_fd){
	//user
	sprintf(buf,"USER %s\r\n",USERNAME);
	send(socket_fd, buf, strlen(buf), 0);
    recv(socket_fd, buf, BUF_SIZE, 0);
    printf("%s\n", buf);
    memset(buf, 0, BUF_SIZE);
	
	//password
	sprintf(buf,"PASS %s\r\n",PASSWORD);
    send(socket_fd, buf, strlen(buf), 0);
    recv(socket_fd, buf, BUF_SIZE, 0);
    printf("%s\n", buf);
    memset(buf, 0, BUF_SIZE);
	
	printf("User %s has been login.\n\n", USERNAME);
}

/*=================================
name：get_data_port
param：socket_fd
return：data_port
function：开启被动模式，获取数据端口并返回
===================================*/

int get_data_port(int socket_fd)
{
	//set passive mode
	send(socket_fd, "PASV\r\n", strlen("PASV\r\n"), 0);
    recv(socket_fd, buf, BUF_SIZE, 0);
    printf("%s\n", buf);
	
	char port_buf[BUF_SIZE];
	//Compute data port
    strncpy(port_buf, buf, BUF_SIZE);
    int data_port = compute_data_port(port_buf);
    //free(port_buf);
    memset(buf, 0, sizeof(buf));
	
	printf("Data port %d has been opened.\n\n", data_port);
	return data_port;
}

/*=================================
name：quit
param：socket_fd
return：
function：退出服务器链接
===================================*/

void quit(int socket_fd)
{
	send(socket_fd, "QUIT\r\n", strlen("QUIT\r\n"), 0);
    recv(socket_fd, buf, BUF_SIZE, 0);
    printf("%s\n", buf);
    memset(buf, 0, BUF_SIZE);
}