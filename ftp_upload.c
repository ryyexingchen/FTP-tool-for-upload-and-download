#include "ftp.h"

/*=================================
name:file_upload
param:
return:正常退出为0，异常退出为-1
function：文件上传函数
===================================*/

int file_upload(){
	char filename[MAX_FILENAME_LENGTH];
	char server_file_path[MAX_FILENAME_LENGTH];
	char client_file_path[MAX_FILENAME_LENGTH];
	
	while(1) {
        // 提示用户输入文件名
        printf("Please enter the filename you want to upload: \n");
        scanf("%255s", filename); // 使用scanf读取文件名，限制长度防止溢出

        // 检查文件名是否以 '/' 开头，如果不是，则添加相对路径
        if (filename[0] != '/') {
			sprintf(server_file_path, "./%s/%s", UPLOAD_DICTIONARY, filename);
			break;
		} else {
			printf("Please use relative path.\n");
		}
    }
	sprintf(client_file_path, "./%s", filename);
	
	/*Connect with the server*/
    int socket_fd = connect_server();
	/*login*/
    login(socket_fd);
	/*get data port*/
	struct sockaddr_in data_addr;
    int data_port = get_data_port(socket_fd);
	
	int data_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(data_fd < 0)
    {
		printf("Fail to create a socket to transmit the file data :%s\n", strerror(errno));
		return -1;
    }
	
	data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(data_port);
    inet_aton(ip, &data_addr.sin_addr);
	
	if(connect(data_fd, (struct sockaddr*)&data_addr, sizeof(data_addr)) < 0)
    {
	    printf("Fail to connect with the data port %s :%s", ip, strerror(errno));
	    return -1;
    }
	
	//set typeA
    send(socket_fd, "TYPE A\r\n", strlen("TYPE A\r\n"), 0);
    recv(socket_fd, buf, BUF_SIZE, 0);
    printf("%s\n", buf);
    memset(buf, 0, BUF_SIZE);
	
	 // 构建STOR命令
    sprintf(buf, "STOR %s\r\n", server_file_path);
	send(socket_fd, buf, strlen(buf), 0);
	
    int remote_fd = open(client_file_path, O_RDONLY);
    if(remote_fd < 0)
    {
	    printf("Fail to open the local file :%s\n", strerror(errno));
	    return -1;
    }
    if(read(remote_fd, buf, sizeof(buf)) < 0)
    {
	    printf("Fail to read the local file :%s\n", strerror(errno));
	    return -1;
    }

    send(data_fd, buf, sizeof(buf), 0);
	close(data_fd);
	quit(socket_fd);

	printf("File has been uploaded seccessfully.\n");
	return 0;
}