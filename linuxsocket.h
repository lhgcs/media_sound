#ifndef LINUXSOCKET_H
#define LINUXSOCKET_H

#include "jsonhandler.h"
#include "model/httprequest.h"
#include "serial/serialputout.h"

#include <QList>

#define SEND_BUFF_SIZE 10
#define RECV_BUFF_SIZE 10

class LinuxSocket
{
public:
    LinuxSocket();
    static void send_data_all(int fd);
    static void add_to_send_list(char buf[]);
    static void* thread1(void *);
    static void close_socket();
private:
    static int sockfd;

    /*
     TCP可靠性
     1.超时重传,发出数据后启动定时器,5秒内没收到回复OK,再发
     2.数据头和数据校准,收到的数据不对不回复OK
     3.收到重复数据,比较是否已收到
    */
    //指针数组,等于(char *)sendBuff[32]
    static char *sendBuff[SEND_BUFF_SIZE];  //发送队列
    static char *repeatBuff[RECV_BUFF_SIZE];//已接收到的数据

    //连接
    static char *packetConnect;
    static char *packetConnect_ACK;

    //心跳
    static char *packetHeart;
    static char *packetHeart_ACK;

    static char *packetPutoutCmd_ACK;
    static char *packetPutoutResult_ACK;

    //左移
    static void buff_del(char **buff,int buffLen){
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                return;
            }
        }
        char *p=buff[0];
        for(int i=1;i<buffLen;i++){
            buff[i-1]=buff[i];
        }
        free(p);
        buff[buffLen-1]=NULL;
    }
    //追加   //存储收到的数据
    static void buff_add(char *buff[],int buffLen,char *str){

        qDebug()<<"add to buff befor";
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                //qDebug()<<"null";
            }else{
                qDebug()<<buff[i];
            }
        }
        buff_del(buff,buffLen);

        char *p=(char *)malloc(sizeof(char)*strlen(str)+1);
        memcpy(p,str,strlen(str));
        p[strlen(str)]='\0';

        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                buff[i]=p;//////////////////////////////
                break;
            }
        }

        qDebug()<<"add to buff after";
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                //qDebug()<<"null";
            }else{
                qDebug()<<buff[i];
            }
        }


    }
    //删除指定字符串
    static void buff_del_data(char **buff,int buffLen,char *str){
        qDebug()<<"delete ";
        int k=0;
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                break;
            }
            if(buff[i] != str){
                buff[k]=buff[i];
                k++;
            }else{
                free(buff[i]);
            }
        }
        buff[k]=NULL;
    }
    //删除包含指定字符串的字符串
    static void buff_del_data_contain(char **buff,int buffLen,char *str){
        qDebug()<<"delete contain";
        int k=0;
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                break;
            }
            if(strstr(buff[i],str) == NULL){
                buff[k]=buff[i];
                k++;
            }else{
                free(buff[i]);
            }
        }
        buff[k]=NULL;
    }
    //删除指定字符串(只删除第一条)
    static void buff_del_data_first(char **buff,int buffLen,char *str){
        qDebug()<<"delete first";
        int index=buff_contain_data(buff,buffLen,str);
        if(index >= 0){
            int k=index;
            int flag=0;
            for(int i=index;i<buffLen;i++){
                if(flag == 0){
                    if(buff[i] != NULL){
                        if(strcmp(str,buff[i]) != 0){
                            buff[k]=buff[i];
                            k++;
                        }
                    }else{
                        free(buff[i]);
                        continue;
                    }
                }else{
                    buff[k]=buff[i];
                    k++;
                }
            }
            buff[k]=NULL;
        }
    }
    static int buff_contain_data(char **buff,int buffLen,char *str){
        qDebug()<<"check";
        int k=-1;
        for(int i=0;i<buffLen;i++){
            if(buff[i] == NULL){
                break;
            }else{
                qDebug()<<i<<"buff:"<<buff[i]<<str;
                if(strcmp(buff[i],str) == 0){//buff[i] == str
                    qDebug()<<"===========";
                    k=i;
                    break;
                }
            }
        }
        return k;
    }

    /**
      解析数据
     * @brief parse_json
     * @param fd
     * @param str
     */
    static void parse_json(const int &fd,char *str){
        if(str == NULL){
            qDebug()<<"NULL";
            return;
        }
        //收到应答,删除发送队列中的第一条数据
        if(strcmp(str,packetHeart_ACK) == 0){        //心跳回复
            qDebug()<<"get heart ack";
            buff_del_data_first(sendBuff,SEND_BUFF_SIZE,packetHeart);
        }else if(strcmp(str,packetConnect_ACK) == 0){//连接回复
            qDebug()<<"get connect ack";
            buff_del_data_first(sendBuff,SEND_BUFF_SIZE,packetConnect);
        }else{
            QJsonDocument json=JsonHandler::get_json_format(str);
            if(!json.isEmpty()){
                //"{“order”:”8153300908281845”,“topic”:”good”,“box”:”A1”}"
                QString order=JsonHandler::get_json_object_value_str(json,"order","NULL");
                if(order != NULL && order != "NULL"){
                    //是否已收到这条指令

                    int flag=buff_contain_data(repeatBuff,RECV_BUFF_SIZE,str);
                    if(flag < 0){//第一次收到数据
                        qDebug()<<"get data first";
                        buff_add(repeatBuff,RECV_BUFF_SIZE,str);

                        QString topic=JsonHandler::get_json_object_value_str(json,"topic","NULL");
                        QByteArray topicbyte=topic.toLatin1();
                        char *topicStr=topicbyte.data();
                        //出货命令
                        if(strcmp(topicStr,"good") == 0){
                            QString box=JsonHandler::get_json_object_value_str(json,"box","NULL");
                            if(box != NULL && box != ""){
                                box=box.right(box.length()-1);
                                if(box != NULL && box != ""){
                                    int res=HttpRequest::getInstance()->add_to_list(PUT_OUT_NO,box.toInt(),HttpRequest::NONE,order);
                                    if(res == 0){//加入队列成功
                                        //发送应答
                                        QMap<QString,QString> map;
                                        map.insert("mac_id",App::MACID);
                                        map.insert("order",order);
                                        map.insert("topic","order");

                                        char *buff=JsonHandler::package_json(map);
                                        add_to_send_list(buff);
                                        free(buff);
                                    }else{
                                        putout_failed(order);
                                    }
                                }
                            }
                        }
                        QString msg=JsonHandler::get_json_object_value_str(json,"msg","NULL");
                        if(msg != NULL && msg != "NULL"){
                            QByteArray msgbyte=msg.toLatin1();
                            char *msgStr=msgbyte.data();
                             if(msg.compare("ordersuccess") == 0){ //真正出货
                                qDebug()<<"start put out";
                                QByteArray byte=order.toLatin1();
                                char *str=byte.data();
                                buff_del_data_contain(sendBuff,SEND_BUFF_SIZE,str);
                                int res=HttpRequest::getInstance()->put_out(order);
                                if(res != 0){
                                    putout_failed(order);
                                }
                            }
                            //响应结果
                            else if(0 == strcmp(msgStr,"orderchecked")    ||
                                    0 == strcmp(msgStr,"receivesuccess")  ||
                                    0 == strcmp(msgStr,"receivechecked")){
                                QByteArray byte=order.toLatin1();
                                char *str=byte.data();
                                buff_del_data_contain(sendBuff,SEND_BUFF_SIZE,str);
                            }
                            // {"order":"8153300908281845","msg":"orderchecked"}
                            // {"order":"8153390089426197","msg":"ordersuccess"}
                        }
                    }else{//多次收到
                        qDebug()<<"get data again";
                    }

                }else{
                    qDebug()<<"order not null";
                    QString topic=JsonHandler::get_json_object_value_str(json,"topic","NULL");
                    //测试电机
                    if(topic == "singledoor"){
                        QString box=JsonHandler::get_json_object_value_str(json,"box","NULL");
                        if(box != NULL && box != ""){
                            int b=box.toInt();
                            SerialPutOut::getInstance()->open_reset(b);
                        }
                    }else if(topic == "alldoor"){
                        SerialPutOut::getInstance()->open_reset(1);
                    }else if(topic == "restart"){
                        system("sync && reboot");
                    }
                }
            }else{
                qDebug()<<"json error";
            }
        }
        send_data_all(sockfd);
    }
public:
    /**
      出货成功
     * @brief putout_ok
     * @param fd
     * @param order
     */
    static void putout_ok(QString order){
        QMap<QString,QString> map;
        map.insert("mac_id",App::MACID);
        map.insert("order",order);
        map.insert("topic","goodsuccess");

        char *buff=JsonHandler::package_json(map);
        add_to_send_list(buff);//发送应答
        send_data_all(sockfd);
        free(buff);
    }
    /**
      出货失败
     * @brief putout_failed
     * @param fd
     * @param order
     */
    static void putout_failed(QString order){
        QMap<QString,QString> map;
        map.insert("mac_id",App::MACID);
        map.insert("order",order);
        map.insert("topic","goodfail");

        char *buff=JsonHandler::package_json(map);
        add_to_send_list(buff);//发送应答
        send_data_all(sockfd);
        free(buff);
    }

};



#endif // LINUXSOCKET_H
