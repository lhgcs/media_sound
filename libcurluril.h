#ifndef LIBCURLURIL_H
#define LIBCURLURIL_H

#include <stdio.h>
#include <string.h>
#include <curl.h>
#include <unistd.h>
#include <pthread.h>
#include <curl/curl.h>

#define CURL_DEBUG 1
#define GET        1
#define POST       2

//回调函数指针
typedef size_t (*Func)(void *, size_t, size_t, void *);

//HTTP请求信息
struct RequestsInfo{
    char type;     //请求类型
    char *url;     //请求URL
    char *params;  //请求参数
    Func fun;      //接收到数据时回调函数指针
};



/**
 * @brief global_init
 * curl全局初始化
 */
void global_init();

/**
 * @brief global_free
 * curl全局释放
 */
void global_free();


/**
 * @brief recv_fun
 * @param ptr    下载回来的数据
 * @param size   数据长度
 * @param nmemb  数据片数量
 * @param userp  用户指针, 用户通过这个指针传输自己的数据
 * @return
 * 接收到数据后处理
 */
size_t recv_fun(void *ptr, size_t size, size_t nmemb, void *userp);


/**
 * @brief set_requests_params
 * @param curlHandle  句柄
 * @param headers     请求头
 * @param info        请求信息
 * 设置参数
 */
void set_requests_params(CURL* curlHandle, struct curl_slist* headers, struct RequestsInfo *info);


/**
 * @brief http_easy
 * @param info   请求信息
 * @return
 * 同步
 */
int http_easy(struct RequestsInfo *info);


/**
 * @brief http_multi
 * @param info   请求信息(数组)
 * @param len    数组长度
 * @return
 * 异步
 */
int http_multi(struct RequestsInfo info[], int len);

/**
 * @brief pull_one_url
 * @param _info   请求信息
 * @return
 * 线程任务
 */

void *pull_one_url(void *_info);

/**
 * @brief start_thread
 * @param info   请求信息
 * @return
 * 线程请求
 */
int start_thread(struct RequestsInfo *info);




#endif // LIBCURLURIL_H
