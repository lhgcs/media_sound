/*
 * socket
 * 连接服务器
 * 2017-06-01
 */

#include "linuxsocket.h"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <QDebug>

#include <arpa/inet.h>
#include <sys/time.h>

//#define IP_ADDR  "ytxyy.veiding.com"
//#define SERVPORT 54995

#define IP_ADDR  "mini.veiding.com"
#define SERVPORT 54990

#define LOGD printf
#define LOGI printf
#define LOGE printf

int LinuxSocket::sockfd=-1;
char *LinuxSocket::sendBuff[SEND_BUFF_SIZE]={0};  //发送队列
char *LinuxSocket::repeatBuff[RECV_BUFF_SIZE]={0};//已接收到的数据


char *LinuxSocket::packetConnect=NULL;
char *LinuxSocket::packetConnect_ACK=NULL;

char *LinuxSocket::packetHeart=NULL;
char *LinuxSocket::packetHeart_ACK=NULL;

char *LinuxSocket::packetPutoutCmd_ACK=NULL;
char *LinuxSocket::packetPutoutResult_ACK=NULL;
/*线程一*/
void* LinuxSocket::thread1(void *)//param
{
    int sockfd;                   //socket描述符
    struct hostent *host;
    struct sockaddr_in serv_addr;

    /**
        hostent->h_name
        表示的是主机的规范名。例如www.google.com的规范名其实是www.l.google.com。
        hostent->h_aliases
        表示的是主机的别名.www.google.com就是google他自己的别名。有的时候，有的主机可能有好几个别名，这些，其实都是为了易于用户记忆而为自己的网站多取的名字。
        hostent->h_addrtype
        表示的是主机ip地址的类型，到底是ipv4(AF_INET)，还是pv6(AF_INET6)
        hostent->h_length
        表示的是主机ip地址的长度
        hostent->h_addr_lisst
        表示的是主机的ip地址，注意，这个是以网络字节序存储的。千万不要直接用printf带%s参数来打这个东西，会有问题的哇。所以到真正需要打印出这个IP的话，需要调用inet_ntop()。
        const char *inet_ntop(int af, const void *src, char *dst, socklen_t cnt) ：
        这个函数，是将类型为af的网络地址结构src，转换成主机序的字符串形式，存放在长度为cnt的字符串中。返回指向dst的一个指针。如果函数调用错误，返回值是NULL
    */

    /*
        inet_pton函数和inet_ntop函数
        这两个函数是随IPv6出现的新函数，对于IPv4地址和IPv6地址都适用。函数名中的p和n非别代表表达（presentation）和数值（numeric）。地址的表达格式通常是ASCII字符串，数值格式则是存放到套接字地址结构中的二进制值
        int inet_pton(int family, const char *strptr, void *addrptr);
        返回：若成功则为1,若输入不是有效的表达格式则为0,若出错则为-1
        const char *inet_ntop(int family, const void *addrptr, char *strptr, size_t len);
        返回：若成功则为指向结果的指针， 若出错则为NULL

        这两个函数的family参数既可以是AF_INET，也可以是AF_INET6。如果以不被支持的地址族作为family的参数，这两个函数就都返回一个错误，并将errno置为EAFNOSUPPORT。
        第一个函数尝试转换由strptr指针所指的字符串，并通过addrptr指针存放二进制结果。若成功则返回1,否则如果对所指定的family而言输入的字符串不是有效的表达式，那么值为0。
        inet_ntop进行相反的转换，从数值格式（addrptr）转换到表达格式（strptr）。len参数是目标存储单元的大小，以免该函数溢出其调用者的缓冲区。
    */

    /*
        1.把ip地址转化为用于网络传输的二进制数值
        int inet_aton(const char *cp, struct in_addr *inp);
        2.将网络传输的二进制数值转化为成点分十进制的ip地址
        char *inet_ntoa(struct in_addr in);

        struct in_addr addr1,addr2;
        ulong l1,l2;
        l1= inet_addr("192.168.0.74");
        l2 = inet_addr("211.100.21.179");
        memcpy(&addr1, &l1, 4);
        memcpy(&addr2, &l2, 4);
        printf("%s : %s\n", inet_ntoa(addr1), inet_ntoa(addr2)); //注意这一句的运行结果
        printf("%s\n", inet_ntoa(addr1));
        printf("%s\n", inet_ntoa(addr2));
    */

    //将点分十进制的ip地址转化为用于网络传输的数值格式
    char *ip_addr;
    if(inet_pton(AF_INET,IP_ADDR,&ip_addr)<0){
        printf("inet_pton failed\n");
        exit(1);
    }else{
        printf("ip_addr:%s",ip_addr);
    }

//    if (gethostname(IP_ADDR, strlen(IP_ADDR)) == -1) {
//        perror("gethostname:");
//        return NULL;
//        //exit(1);
//    }

//    int flags = fcntl(sockfd, F_GETFL, 0);        //获取文件的flags值。
//    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);   //设置成非阻塞模式；//同时在接收和发送数据时，需要使用MSG_DONTWAIT标志
//    fcntl(sockfd,F_SETFL,flags&~O_NONBLOCK);      //设置成阻塞模式；同时在接收和发送数据时，需要使用阻塞标志0

    memset(sendBuff,0,SEND_BUFF_SIZE);
    memset(repeatBuff,0,RECV_BUFF_SIZE);

    while(1){
        /*地址解析函数*/
        if((host=gethostbyname(IP_ADDR)) == NULL){//用域名或主机名获取IP地址(将一个服务器的域名转化成该服务器的IP地址),返回对应于给定主机名的包含主机名字和地址信息的hostent结构指针
            printf("gethostbyname failed\n");
            perror("gethostbyname");
            sleep(5);
            continue;
            //exit(1);
        }else{
            /* 将主机的规范名打出来 */
            printf("official hostname:%s\n",host->h_name);
            /* 主机可能有多个别名，将所有别名分别打出来 */
            char **pptr;
            char   str[32];
            for(pptr = host->h_aliases; *pptr != NULL; pptr++){
                printf(" alias:%s\n",*pptr);
            }
            /* 根据地址类型，将地址打出来 */
            switch(host->h_addrtype){
                case AF_INET:
                case AF_INET6:
                    pptr=host->h_addr_list;
                    /* 将刚才得到的所有地址都打出来。其中调用了inet_ntop()函数 */
                    for(; *pptr!=NULL; pptr++){
                        printf(" address:%s\n",inet_ntop(host->h_addrtype, *pptr, str, sizeof(str)));
                    }
                    printf(" first address: %s\n",inet_ntop(host->h_addrtype, host->h_addr, str, sizeof(str)));
                break;
                default:printf("unknown address type\n");break;
            }
        }

        /*创建 socket*/
        if((sockfd=socket(AF_INET,SOCK_STREAM,0))== -1){//IPv4协议,字节流套接字
            perror("socket");
            sockfd=-1;
            sleep(3);
            continue;
        }else{
            qDebug("socket ok!\n");
        }
        /*设置 sockaddr_in 结构体中相关参数*/
        serv_addr.sin_family=AF_INET;                          //地址族
        serv_addr.sin_port=htons(SERVPORT);                    //端口号
        serv_addr.sin_addr=*((struct in_addr *)host->h_addr);  //IP地址
        bzero(&(serv_addr.sin_zero),8);                        //填充0以保持与 struct sockaddr 同样大小
        /*调用 connect 函数主动发起对服务器端的连接*/
        int res=connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr));//服务器端地址,地址长度
        if(res == 0){//立即创建了socket链接
           qDebug("connect ok\n");
        }else{
            switch(errno){
                case EISCONN:
                    res = 0;
                    break;
                case EINPROGRESS://errno == EINPROGRESS表示正在建立链接
                case EALREADY:
                    //res = socket_wait_writable(sockfd, 30000);
                    res=-1;
                    break;
                default:
                    res = -EFAULT;
                    break;
            }
            if(res < 0){
                perror("connect");
                LOGE("connect [%s:%d] failed, reval=%d", IP_ADDR, SERVPORT, res);
                close_socket();
                continue;
            }
        }

        int recvbytes;
        char recvBuff[128];
        memset(recvBuff,0,sizeof(recvBuff));
        /*将调用 socket 函数的描述符作为文件描述符*/
        fd_set readfd;               //定义要监视的集合,监听文件描述符是否可读，不监听可以传入 NULL
        fd_set *writefds=NULL;       //监听文件描述符是否可写 ，不监听可以传入 NULL
        fd_set *exceptfds=NULL;      //监听文件描述符是否有异常，不监听可以传入 NULL
        struct timeval time;         //设置监视的时间  0：从不等待  NULL：永远等待

        int heartCnt=0;
        int timeoutCnt=0;

        if(packetConnect != NULL){
            add_to_send_list(packetConnect);
            send_data_all(sockfd);
        }

        while(1){
            //由于select()会修改字符集，因此如果select()调用是在一个循环中，则描述符集必须被重新赋值
            FD_ZERO(&readfd);        //清除一个文件描述符集
            FD_SET(sockfd,&readfd);  //将一个文件描述符加入文件描述符集中

            //timeout的值必须在每次select()之前重新赋值，因为操作系统会修改此值
            //在非必须的情况下，尽量使用不超时的select()，即将utimeout参数设置为NULL
            time.tv_sec=0;
            time.tv_usec=1000000;

            /*调用 select 函数*/
            //最高的文件描述符加1,读文件描述符集合,写文件描述符集合,异常处理文件描述符集合
            int res=select(sockfd+1,&readfd,writefds,exceptfds,&time);
            if(res > 0) {                                  //准备好的文件描述符
                if(FD_ISSET(sockfd,&readfd)>0) {           //用于测试指定的文件描述符是否在该集合中
                    //将检测到_socket_fd读事件或写时间，并不能说明connect成功
                    int error = 0;
                    socklen_t len = sizeof(error);
                    if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0){//建立简介失败close(_socket_fd)
                        continue;
                    }
                    if(error != 0) {//建立链接失败close(_socket_fd)
                        continue;
                    }else{          //建立链接成功
                    }

                    recvbytes=recv(sockfd,recvBuff,sizeof(recvBuff),0);
                    if(recvbytes == -1){
                        qDebug("recv error");
                    }else if(recvbytes == 0){//当返回值为0时，表示对端已经关闭了这个链接，我们应该自己关闭这个链接
                        FD_CLR(sockfd,&readfd);
                        close_socket();
                        break;
                    }else{
                        timeoutCnt=0;
                        qDebug()<<"recv:"<<recvBuff;
                        if(recvbytes >= 5){
                            if (strcmp("heart", recvBuff) == 0){
                                send(sockfd,"1",1,0);
                            }else{
                                parse_json(sockfd,recvBuff);
                            }
                        }
                        memset(recvBuff,0,sizeof(recvBuff));
                    }
                }
            }else if(res == 0){
                //qDebug()<<"time out";
                if(timeoutCnt++ > 30){
                    timeoutCnt=0;
                    FD_CLR(sockfd,&readfd);
                    close_socket();
                    break;
                }else{
                    continue;
                }
            }else{
                LOGE("error in socket_wait 1:%s\n", strerror(errno));
                LOGE("select error, process will eixt\n");
                FD_CLR(sockfd,&readfd);
                close_socket();
                break;
                //exit(0);
            }




            if(heartCnt++ >10){
                heartCnt=0;
                if(packetHeart != NULL){
                    add_to_send_list(packetHeart);
                }else{
                    qDebug()<<"heart null";
                }
            }

            send_data_all(sockfd);


        }

    }
}
//                                memcpy(recvBuff,s,strlen(s));//strcpy只能拷贝字符串了，它遇到'\0'就结束拷贝

/*发送消息给服务器端*/
void LinuxSocket::add_to_send_list(char buf[]){
    //将flag设置为0，默认是阻塞。
    if(buff_contain_data(sendBuff,SEND_BUFF_SIZE,buf) == -1){
        buff_add(sendBuff,SEND_BUFF_SIZE,buf);
    }
}

void LinuxSocket::send_data_all(int fd){
    if(fd > 0){
        for(unsigned int i=0;i<SEND_BUFF_SIZE;i++){//sizeof(sendBuff[i]);
            if(sendBuff[i] != NULL){
                int buffLen=strlen(sendBuff[i]);
                qDebug()<<"send:"<<sendBuff[i];
                int len=send(fd,sendBuff[i],buffLen,0);//设置为异步方式，就可以一边读，一边写
                if(len<0){
                    //printf("消息'%s'发送失败！错误代码是%d，错误信息是'%s'\n",buf, errno, strerror(errno));
                    perror("send");
                    qDebug("send error");
                }else if(len==0){
                    qDebug("send 0");
                }else if(len == buffLen){
                    //qDebug("send ok");
                }
            }else{
                break;
            }
        }
    }
}

LinuxSocket::LinuxSocket(){

    QMap<QString,QString> map;
    map.insert("mac_id",App::MACID);
    map.insert("topic","on");
    packetConnect=JsonHandler::package_json(map);

    map.clear();
    map.insert("msg","on");
    packetConnect_ACK=JsonHandler::package_json(map);

    map.clear();
    map.insert("mac_id",App::MACID);
    map.insert("topic","heart");
    packetHeart=JsonHandler::package_json(map);

    map.clear();
    //map.insert("mac_id",App::MACID);
    map.insert("msg","heart");
    packetHeart_ACK=JsonHandler::package_json(map);

    map.clear();
    map.insert("msg","ordersuccess");
    packetPutoutCmd_ACK=JsonHandler::package_json(map);

    map.clear();
    map.insert("msg","receivesuccess");
    packetPutoutResult_ACK=JsonHandler::package_json(map);

qDebug()<<packetConnect
<<packetConnect_ACK
<<packetHeart
<<packetHeart_ACK
<<packetPutoutCmd_ACK
<<packetPutoutResult_ACK;

    pthread_t id1;
    pthread_attr_t attr;
    /*初始化线程*/
    pthread_attr_init(&attr);
    /*设置线程绑定属性*/
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    /*设置线程分离属性*/
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    /*创建线程*/
    int ret=pthread_create(&id1,&attr,thread1,NULL);//创建线程成功时，函数返回0
    if(ret!=0){
        printf ("Create pthread error!\n");
        qDebug("Create pthread error!\n");
        exit (1);
    }else{
        qDebug("Create pthread ok!\n");
    }
    /*等待线程结束*/
    pthread_join(id1,NULL);
    close(sockfd);
}

void LinuxSocket::close_socket(){
    if(sockfd > 0){
        shutdown(sockfd,SHUT_RDWR);
        close(sockfd);
        sockfd=-1;
        sleep(3);
    }
}


// strncasecmp(buf, "Heart", 5))   {//比较参数s1 和s2字符串前n个字符，比较时会自动忽略大小写的差异


