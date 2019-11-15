/*
 * @Description: http请求
 * @Author: your name
 * @Date: 2019-10-22 23:05:47
 * @LastEditTime: 2019-10-22 23:19:14
 * @LastEditors: Please set LastEditors
 */

#include <stdio.h>
#include <string.h>
#include <curl.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#define CURL_DEBUG 1
#define GET        1
#define POST       2

// 回调函数指针
typedef size_t (*Func)(void *, size_t, size_t, void *);

// HTTP请求信息
struct RequestsInfo{
    char type;     // 请求类型
    char *url;     // 请求URL
    char *params;  // 请求参数
    Func fun;      // 接收到数据时回调函数指针
};

class CurlUtil {

public:

    /**
    * @description: 全局初始化
    * @param {type} 
    * @return: 
    */
    static void globalInit() {
        curl_global_init(CURL_GLOBAL_ALL);// 只能用一次,如果这个函数在curl_easy_init函数调用时还没调用，由libcurl库自动调用
    }

    /**
     * @description: 全局释放
     * @param {type} 
     * @return: 
     */
    static void globalCleanup() {
        curl_global_cleanup();
    }

/**
 * @brief recvBuff
 */
char recvBuff[256];

/**
 * @brief recv_fun
 * @param ptr    下载回来的数据
 * @param size   数据长度
 * @param nmemb  数据片数量
 * @param userp  用户指针, 用户通过这个指针传输自己的数据
 * @return
 * 接收到数据后处理
 */
size_t recv_fun(void *ptr, size_t size, size_t nmemb, void *userp)
{
    if(ptr != '\0') {
        char *p = (char *)ptr;
        printf("recv:%s\n", p);
    }else{
        printf("recv:NULL\n");
    }
    return size * nmemb;
}

/**
 * @description: 设置参数
 * @param {type} 句柄 请求头 请求信息
 * @return: 
 */ 
void setParams(CURL* curlHandle, struct curl_slist* headers, struct RequestsInfo *info) {
    char temp[128];
    memset(temp, 0, sizeof(temp));

    if(NULL == curlHandle || NULL == info) {
        return;
    }

    if(POST == info->type) {
        // 设置问非0表示本次操作为post
        curl_easy_setopt(curlHandle, CURLOPT_POST, 1);
        // post参数
        if(info->params != NULL) {
            curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, info->params);
            // 发送的参数数据长度,，否则服务器不知道接受多少数据
            snprintf(temp, sizeof(temp), "Content-Length: %ld", sizeof(info->params));
            headers = curl_slist_append(headers, temp);
        }
    }

    // 修改请求头,为空时不提交.请求消息头用于告诉服务器如何处理请求；响应消息头则告诉浏览器如何处理接收到的数据
    // 浏览器的身份标识字符串
    snprintf(temp, sizeof(temp), "User-Agent: %s", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/72.0.3626.109 Safari/537.36");
    headers = curl_slist_append(headers, temp);

    /**
    * 可接受的响应内容类型（Content-Types）
    * 1.\*\/\*
    * 2.text/plainclass LibcurlUril
    * 3.application/json
    */
    // 允许接收任何类型的数据
    snprintf(temp, sizeof(temp), "Accept: %s", "*/*");
    headers = curl_slist_append(headers, temp);

    // 可接受的字符集
    snprintf(temp, sizeof(temp), "Accept-Charset: %s", "utf-8");
    headers = curl_slist_append(headers, temp);

    // 不要缓冲数据
    snprintf(temp, sizeof(temp), "Pragma: %s", "no-cache");
    headers = curl_slist_append(headers, temp);

    // 设置请求的url地址
    curl_easy_setopt(curlHandle, CURLOPT_URL, info->url);
    // 设置HTTP头
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, headers);
    // 设置发送超时时间
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT_MS, WAIT_TIMEOUT_MSECS);
    // 设置连接等待时间
    curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT_MS, WAIT_CONNECTTIMEOUT_MSECS);
    // 关闭域名解析超时,把超时时间设置为0，也就是DNS解析不设置超时时间（防止多线程修改全局变量出错）
    curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
    // 跟踪爬取重定向页面
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
    // 在完成交互以后强迫断开连接，不能重用
    curl_easy_setopt(curlHandle, CURLOPT_FORBID_REUSE, 1L);
    // 下载数据不包括HTTP头部
    // curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);
    // 设置接收数据的处理函数和存#include <pthread.h>

    if(info->fun != NULL) {
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, NULL);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, info->fun);
    }
    // 读取数据传递给远程主机
    // curl_easy_setopt(curlHandle, CURLOPT_READDATA, sendBuff);
    // curl_easy_setopt(curlHandle, CURLOPT_READFUNCTION, send_fun);
    // 调试输出
    #ifdef CURL_DEBUG
        curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 1L);
    #endif
}


/**
 * @brief http_easy
 * @param info   请求信息
 * @return
 * 同步
 */
int http_easy(struct RequestsInfo *info)
{
    // 句柄
    CURL* curlHandle = NULL;
    CURLcode res = CURLE_OK;
    struct curl_slist* headers = NULL;

    if(NULL == info || NULL == info->url) {
        return -1;
    }

    // 初始化
    curlHandle = curl_easy_init();
    if (curlHandle) {
        set_requests_params(curlHandle, headers, info);

        // 执行单条请求res ==
        memset(recvBuff, 0, sizeof(recvBuff));
        res = curl_easy_perform(curlHandle);
        if (CURLE_OK == res) {
            printf("OK\n");
        }else{
            printf("error:%s\n",curl_easy_strerror(res));
            switch (res) {
                case CURLE_UNSUPPORTED_PROTOCOL:printf("不支持的协议，由URL的头部指定\n");break;
                case CURLE_COULDNT_CONNECT:     printf("不能连接到remote 主机或者代理\n");break;
                case CURLE_REMOTE_ACCESS_DENIED:printf("访问被拒绝\n");break;
                case CURLE_HTTP_RETURNED_ERROR: printf("Http返回错误\n");break;
                case CURLE_READ_ERROR:printf("\n");break;
                default:break;
            }
        }

        if(headers != NULL) {
            curl_slist_free_all(headers);
        }

        if(curlHandle != NULL) {
            curl_easy_cleanup(curlHandle);
        }
    }
    return res;
}


/**
 * @brief http_multi
 * @param info   请求信息(数组)
 * @param len    数组长度
 * @return
 * 异步
 */
int http_multi(struct RequestsInfo info[], int len) {
    if(NULL == info || len <= 0) {
        return -1;
    }

    CURLcode res = CURLE_OK;
    struct curl_slist* headers = NULL;

    CURLM *multiHandle = curl_multi_init();
    if (NULL == multiHandle) {
        return -1;
    }

    int i = 0;
    for(i=0; i<len; i++) {
        CURL* easyHandle = curl_easy_init();
        if (NULL == easyHandle || NULL == info[i].url) {
            continue;
        }
        set_requests_params(easyHandle, headers, &info[i]);
        // 加入multi栈
        curl_multi_add_handle(multiHandle, easyHandle);
    }

    int retcode;
    int still_running;
    int numfds = 0;

    // 并发,发送数据并用still_running监测数据是否发送完成，如果未发送完成则持续发送
    curl_multi_perform(multiHandle, &still_running);
    do {
        retcode = curl_multi_wait(multiHandle, NULL, 0, WAIT_TIMEOUT_MSECS, &numfds);
        if (retcode != CURLM_OK) {
            printf("ERROR----curl_multi_wait  errorcode[%d]\n",retcode);
            break;
        }

        retcode = curl_multi_perform(multiHandle, &still_running);
        if (retcode != CURLM_OK) {
            printf("ERROR----curl_multi_perform  errorcode[%d]\n", retcode);
            break;
        }
        usleep(10000);// 10ms
    } while (still_running);

    CURLMsg* msg = NULL;
    int msgs_in_queue = 0;
    // 获取每一个执行完成的操作信息,读取发送返回结果信息
    while (msg = curl_multi_info_read(multiHandle, &msgs_in_queue)) {

        if (msg->msg == CURLMSG_DONE) {// 是否读取成功
            // 检测数据发送结果
            if (msg->data.result != CURLE_OK) {
                // 检测HTTP状态
                fprintf(stderr, "CURL error code: %d\n", msg->data.result);
                continue;
            }

            CURL *easyHandle = msg->easy_handle;
            CURLcode retcode;
            // 从curl句柄里获得附加信息
            // 状态码
            long response_code = -1;
            retcode = curl_easy_getinfo(easyHandle, CURLINFO_RESPONSE_CODE, &response_code);
            if(retcode != CURLE_OK) {
                continue;
            }
            printf("code:%ld\n",response_code);

            // URL
            char* url = NULL;
            retcode = curl_easy_getinfo(easyHandle, CURLINFO_EFFECTIVE_URL, &url);
            if(retcode != CURLE_OK) {
                continue;
            }
            printf("url:%s\n",url);

            // 耗时
            int totalTime = 0;
            retcode = curl_easy_getinfo(easyHandle,CURLINFO_TOTAL_TIME, &totalTime);
            if(retcode != CURLE_OK) {
                continue;
            }
            printf("time:%d\n",totalTime);

            if(easyHandle) {
                // 移除
                curl_multi_remove_handle(multiHandle, easyHandle);
                // 清除
                curl_easy_cleanup(easyHandle);
            }
        }
        usleep(10000);// 10ms
    }
    // 清除
    curl_multi_cleanup(multiHandle);
    return res;
}

// /**
//  * @brief pull_one_url
//  * @param _info   请求信息
//  * @return
//  * 线程任务
//  */
// void *pull_one_url(void *_info)
// {
//     struct RequestsInfo *info = (struct RequestsInfo *)_info;

//     if (info != NULL) {
//         http_easy(info);
//     }
//     return NULL;
// }

// /**
//  * @brief start_thread
//  * @param info   请求信息
//  * @return
//  * 线程请求
//  */
// int start_thread(struct RequestsInfo *info)
// {   static int i=0;
//     i++;
//     printf("%d\n",i);
//     int err;
//     pthread_t tid;
//     err = pthread_create(&tid, NULL, pull_one_url, (void *)info);
//     if(err != 0) {
//         printf("%s错误出现在第%d行",__FUNCTION__,__LINE__);
//         printf("%s:%s\n",__func__,strerror(err));
//     }else{
//         // 获取本线程id
//         printf("thread ID is %lu\n",pthread_self());
//     }
//     return 0;
// }


};
