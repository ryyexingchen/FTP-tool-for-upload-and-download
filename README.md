# FTP-tool-for-upload-and-download
2024年暑期实习项目
1、实训项目说明
我实现了综合运用了Linux的文件子系统、进程子系统和网络子系统，用C语言开发了一个FTP多线程上传下载的工具。其中，上传功能是通过单线程实现的，下载功能是通过多线程实现的。
2、FTP工具功能说明
	（1）上传：利用FTP协议，将用户目录下用户指定的文件上传到FTP服务器中指定的文件夹中。（FTP服务器根目录中的files文件夹中）
（2）下载：利用FTP协议，通过多线程的形式（线程数可调，在ftp.h中调节THREAD_NUM的值即可，默认为4），将FTP服务器指定文件夹（files）下的指定文件加载到用户的目录中。
（3）控制界面：给予用户提示信息，通过用户输入“upload”、“download”或“end”控制程序执行上传功能、下载功能或退出程序。
3、FTP工具使用注意事项
	（1）程序在与FTP服务器进行交互时会打印很多服务器返回的提示信息，这些是为了便于理解程序逻辑而保留的，实际使用过程中可以直接注释掉这些提示信息。（具体做法是将recv和write函数后面的“printf(“%s”, buf);”语句注释掉）
（2）运行程序之前需要自行配置FTP服务器（vsftpd），具体配置方法可以参照后面的介绍。
（3）运行程序之前，需要将头文件ftp.h中的IP地址换成连接飞腾派时使用的IP地址。
（4）运行程序之前，需要将头文件ftp.h中的FTP用户名（USERNAME）和密码（PASSWORD）替换成自己的用户名和密码。
（5）在工程编译时，请把所有.c和.h文件以及makefile放在同一个目录下，随后使用“make”命令完成编译，编译后的可执行文件名为ftp。
4、FTP服务器环境配置
（1）环境下载
	在linux系统下输入“sudo apt-get install vsftpd”，安装FTP服务器。
（2）环境配置
	在/ect/vsftpd.conf文件中进行环境配置。
	①输入“sudo nano vsftpd.conf”或“sudo vim vsftpd.conf”打开环境配置文件
	②取消部分注释，包括：
		write_enable=YES
		local_umask=022
		chroot_local_user=YES
	③新增配置，即在文档中添加下面的配置：
		local_root=\*这里填服务器的根目录*\（我的是/home/cxy2212518/ftpdir）
		allow_writeable_chroot=YES
	④保存并退出配置文件。
（3）创建ftp用户
	利用“sudo useradd ftpuser”来创建用户（用户名为ftpuser），利用“sudo passwd ftpuser”来为该用户设置密码（我设置的密码为123）
（4）配置用于上传和下载的文件夹
	打开服务器的根目录，在根目录下创建一个文件夹（我起名为files），并利用“chmod 777 files”赋予其最高权限。
这样做的原因是，刚才设置的根目录的权限为755，如下图所示。也就是说，其他用户没有写入该文件夹的权限，因此ftp用户在上传时无法直接将文件上传到服务器根目录下（因为没有写权限）
 
所以解决办法就是在根目录下创建一个其他用户有写权限的文件夹，即上面提到的files文件夹，如下图所示。这样ftp用户就可以将文件上传到该文件夹中。
 
另外还要注意，如果修改了files文件夹的名字，那么在头文件ftp.h中的UPLOAD_DICTIONARY和DOWNLOAD_DICTIONARY也要进行修改。
（5）开启FTP服务器
	输入“sudo systemctl start vsftpd”开启服务器，利用“sudo systemctl status vsftpd”可以查看当前服务器的状态，利用“sudo systemctl restart vsftpd”可以重启服务器。
（6）测试FTP服务器状态
	①输入“sudo netstat -antup | grep ftp”指令可以查看FTP服务器连接状态，返回信息如下所示：
tcp 15 0 192.168.43.237:585681 192.168.43.237:21 CLOSE_WAIT  2802/ ./ftp_upload
tcp6 0 0 :::21                :::*                    LISTEN  2857 /vsftpd
	②利用ftp工具连接FTP服务器
利用“sudo apt-get install ftp”安装ftp工具，安装成功后，可以用“ftp localhost 21”连接FTP服务器，输入用户名密码登录服务器，随后就可以输入指令与FTP服务器进行交互，例如直接在根目录下输入ls就可以看到根目录下的文件。注意，服务器中的根目录“/”的实际目录为之前设置的“local_root”路径。
