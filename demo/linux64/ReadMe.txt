一，环境准备：
需要将msgdRelease/lib/linux64中的libmsgd.so  libos.so libtrpc.so放到系统/usr/lib/等可找到的系统目录下；
也可使用export LD_LIBRARY_PATH=$(目录路径)/lib/linux64  来增加库文件搜索路径。

二，运行
在命令行执行：cd $(目录路径)/demo/linux64

1，服务端程序
./msgd_server -h查看参数意义
./msgd_server -p 7897   监听7897端口运行服务程序

2，客户端程序
运行命令行：./msgd_client svrip svrport  localGroup localUser dstGroup dstUser persistTimeSec
其参数意义如下：
svrip是指msgd_server运行的主机IP
svrport是指msgd_server运行的主机监听的端口
localGroup本端在服务器注册的用户组
localUser 本端在服务器注册的用户名
dstGroup发送数据目的接收端的用户组
dstUser 发送数据目的接收端的用户名
persistTimeSec目标接收用户不在线时，服务器缓存时间秒
./msgd_client 127.0.0.1 7897 auto test auto dest 20
