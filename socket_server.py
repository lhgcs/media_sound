#服务端Linux系统下：处理命令并返回
import socket
import os

#声明类型，生成socket链接对象
server = socket.socket()

#监听接收端口元组(本地，端口)，绑定要监听的端口
server.bind(('0.0.0.0', 6200))

#1.监听
#2.“5”最大监听数，允许多少人在排队
server.listen(5)

print("waiting client")

#循环锁定访问客户端
while True:

    #1.等待客户端 #会返回链接的标记位conn，与连接的地址
    #2.客户端同过conn,addr进行通话
    #3.conn就是客户端连接过来而在服务器端为其生成的一个连接实例
    conn,addr = server.accept()

    #查看标记位与IP地址
    print(conn,addr, flush = True)


    print("client connect", flush = True)

    #循环处理客户端请求
    while True:

        #1.接收数据,1024字节
        #2.如果发不完会存在缓冲去，下次在发送。
        #3.缓冲区每次最多发32768字节
        #4.每个系统不同，超出数值会有限制。
        data = conn.recv(102400)

        #返回
        print(data)

        #Linux系统内可以判断是否为空。
        if not data:break

        # #执行命令，赋值给变量
        # res = os.popen(data).read()
        s = data.decode('utf-8')
        print(s)
        backStr = ("ifconfig").encode('utf-8')
        #返回一个值，返回res命令
        conn.send(backStr)

        #sendll就是循环send，用法发送大型文件。
        #conn.sendll(res)


#关闭链接
server.close()