server端使用gcc进行编译
1.目录下执行make操作即可获得可执行文件server
2.执行./server即可运行server，通过-port 与 -root 参数来指定监听端口与起始路径
3.执行make clean可以清除无关文件

注：port默认为21，root默认为/tmp