#ifndef LINKLISTUTIL_H
#define LINKLISTUTIL_H

/*
 * 单向链表
 * 2019/04/12
 */

//链表结构体
struct LinkData{
    //文件地址
    char data[100];
    struct LinkData *next;
};


/**
 * @brief create_node
 * @param data
 * @return
 * 创建节点
 */
struct LinkData * create_node(char *data);

/**
 * @brief delete_node
 * @param node
 * 删除节点
 */
void delete_node(struct LinkData *node);

/**
 * @brief get_node_num
 * @param list  链表
 * @return
 * 获取链表节点数目
 */
int get_node_num(struct LinkData *list);

/**
 * @brief add
 * @param list 链表
 * @param node 节点
 * @return
 * 链表增加节点
 */
struct LinkData * add(struct LinkData *list,struct LinkData *node);

/**
 * @brief del
 * @param list 链表
 * 链表删除节点
 */
void del(struct LinkData *list);

/**
 * @brief del_all
 * @param list 链表
 * 链表删除所有节点
 */
void del_all(struct LinkData *list);


#endif // LINKLISTUTIL_H
