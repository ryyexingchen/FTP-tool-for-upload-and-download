#include "ftp.h"

typedef struct {  
    char* filename; 
	char* filepath;	
    int   offset;  
    int   size;  
    int   index;  
} DownloadArgs; 

pthread_mutex_t download_lock;

/*=================================
name:thread_download
param:arg
return:NULL
function：下载的线程函数
===================================*/

void* thread_download(void* arg) {  
    DownloadArgs* args = (DownloadArgs*)arg;
	char 	send_buf[BUF_SIZE];
	char 	recv_buf[BUF_SIZE];
	
	int control_socket = connect_server();//连接服务器
	
	login(control_socket);//登录
	
	pthread_mutex_lock(&download_lock);
	
	int data_port = get_data_port(control_socket);//获取数据端口

	// 连接服务器新开的数据端口
	struct sockaddr_in server;
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
	
    if (data_socket < 0) {
        perror("socket");
        return NULL;
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(data_port);
    inet_aton(ip, &server.sin_addr);
	
    if (connect(data_socket, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(data_socket);
        return NULL;
    }
	
	//设置TypeI
	send(control_socket, "TYPE I\r\n", strlen("TYPE I\r\n"), 0);
    recv(control_socket, recv_buf, BUF_SIZE, 0);
    printf("%s\n", recv_buf);
    memset(recv_buf, 0, BUF_SIZE);
	
	//文件start的偏移
	sprintf(send_buf, "REST %d\r\n", args->offset);
	write(control_socket, send_buf, strlen(send_buf));
	if (read(control_socket, recv_buf, sizeof(recv_buf)) < 0) {
        perror("read");
        close(data_socket);
        return NULL;
    }
    printf("%s", recv_buf);
    memset(recv_buf, 0, BUF_SIZE);
	
	//开始下载
	sprintf(send_buf, "RETR %s\r\n", args->filepath);
	write(control_socket, send_buf, strlen(send_buf));
    if (read(control_socket, recv_buf, sizeof(recv_buf)) < 0) {
        perror("read");
        close(data_socket);
        return NULL;
    }
    printf("%s", recv_buf);
    memset(recv_buf, 0, BUF_SIZE);
	
	pthread_mutex_unlock(&download_lock);
	
	char path[MAX_PATH_NUM];
	sprintf(path , "%s/%d_%s.temp", TEMP_PATH, args->index, args->filename);
	FILE *fp = fopen(path,"wb+");
    if(fp!=NULL){
        int recv_already = 0;
        int recv_len = 0;
        char mem[BUF_SIZE] = {0};

        while((recv_len=recv(data_socket,mem,BUF_SIZE,0))>0){
            if(recv_already+recv_len < args->size){
                fwrite(mem,1,recv_len,fp);
                fflush(fp);
                memset(mem,0,sizeof(mem));
                recv_already+=recv_len;
            }else{
                fwrite(mem,1,args->size - recv_already,fp);
                close(data_socket);
                break;
            }
        }
        if(recv_len==-1){
            printf("error:transport error.\n");
            return NULL;
        }
        fclose(fp);
    }

	quit(control_socket);//退出服务器连接
    return NULL;  
}

/*=================================
name:file_download
param:
return:正常退出为0，异常退出为-1
function：多线程下载函数
===================================*/

int file_download() {
    int  file_handle;
    ssize_t bytes_received;
    struct sockaddr_in server;
	
	char filename[MAX_FILENAME_LENGTH];//文件名
	char file_size_str[BUF_SIZE];//文件大小
	char server_file_path[MAX_FILENAME_LENGTH];//服务器下文件的相对路径
	char client_file_path[MAX_FILENAME_LENGTH];//客户端文件的相对路径
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	
	while(1) {
        // 提示用户输入文件名
        printf("Please enter the filename you want to download: \n");
        scanf("%255s", filename); // 使用scanf读取文件名，限制长度防止溢出

        // 检查文件名是否以 '/' 开头，如果不是，则添加相对路径
        if (filename[0] != '/') {
			sprintf(server_file_path, "./%s/%s", DOWNLOAD_DICTIONARY, filename);
			break;
		} else {
			printf("Please use relative path.\n");
		}
    }
	sprintf(client_file_path, "./%s", filename);
    
	int control_socket = connect_server();//连接服务器
	login(control_socket);//登录
		
	//测试是否支持续传
	send(control_socket, "REST 100\r\n", strlen("REST 100\r\n"), 0);
   
    if(recv(control_socket,recv_buf,BUF_SIZE,0) < 0){
        printf("server can't use multi-thread download.\n");
        return -1;
    }else{
        printf("----------server can use multi-thread download.----------\n");
    }
	memset(recv_buf,0,BUF_SIZE);
	
	//获取文件大小
	sprintf(cmd, "SIZE %s\r\n", server_file_path);
	send(control_socket, cmd, strlen(cmd), 0);
    recv(control_socket, recv_buf, BUF_SIZE, 0);
	printf("%s\n", recv_buf);
	strcpy(file_size_str, recv_buf);//获取文件大小
	memset(recv_buf,0,BUF_SIZE);

	//退出链接
	quit(control_socket);
	
	int file_size = atoi(file_size_str + 4);//获取文件大小
	printf("-----------------the file size is:%d------------------\n", file_size);
	
	//使用多线程下载文件
	struct stat st;
	
	if(stat(TEMP_PATH, &st) != 0) {
        // 如果目录不存在，创建目录
        sprintf(cmd, "mkdir %s", TEMP_PATH);
        system(cmd);
    }
	
	int thread_num = THREAD_NUM;
	pthread_t threads[thread_num];
	int offset = file_size / thread_num;
	
	pthread_mutex_init(&download_lock, NULL);
	
	DownloadArgs args[THREAD_NUM];
	//创建线程
	 for (int i = 0; i < thread_num; i++) {
        args[i].filename = filename;
		args[i].filepath = server_file_path;
        args[i].offset = i * offset;
        args[i].size = (i == thread_num - 1) ? (file_size - i * offset) : offset;
        args[i].index = i;
        pthread_create(&threads[i], NULL, thread_download, (void*)&args[i]);
    }
	
	
	printf("**********************start download**********************\n");
    for (int i = 0; i < thread_num; i++) {
        pthread_join(threads[i], NULL);
    }
	
	//合并文件
    printf("***********************start merge files************************\n");
    FILE *fp;
    char final_fname[MAX_PATH_NUM]; 
	sprintf(final_fname, "./%s", filename);// 替换为实际的文件名
    if ((fp = fopen(final_fname, "wb+")) == NULL) {
        printf("error:open file error.\n");
        return -1;
    }

    for (int i = 0; i < thread_num; i++) {
        char fname[MAX_FILEPATH_LENGTH];
        snprintf(fname, sizeof(fname), "%s/%d_%s.temp", TEMP_PATH, i, filename);
        FILE *fp_part = fopen(fname, "rb");
        if (fp_part == NULL) {
            printf("error:open file error.\n");
            return -1;
        }

        char buffer[BUF_SIZE] = {0};
        int len;
        while ((len = fread(buffer, 1, BUF_SIZE, fp_part)) > 0) {
            fwrite(buffer, 1, len, fp);
            fflush(fp);
            memset(buffer, 0, sizeof(buffer));
        }
        fclose(fp_part);
        remove(fname);
    }
    fclose(fp);
	pthread_mutex_destroy(&download_lock);
	
	if(stat(TEMP_PATH, &st) == 0) {
        // 如果目录存在，删除目录
        sprintf(cmd, "rmdir %s", TEMP_PATH);
        system(cmd);
    }
	printf("File '%s' has been downloaded successfully.\n", filename);
    return 0;
}