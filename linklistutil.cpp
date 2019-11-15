#include "linklistutil.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * @brief create_node
 * @param data
 * @return
 * 创建节点
 */
struct LinkData * create_node(char *data) {
   struct LinkData *temp = (struct LinkData *)malloc(sizeof(struct LinkData));
   strcpy(temp->data,data);
   temp->next = NULL;
   return temp;
}

/**
 * @brief delete_node
 * @param node
 * 删除节点
 */
void delete_node(struct LinkData *node) {
   if(node != NULL) {
       free(node);
       node = NULL;
   }
}

/**
 * @brief get_node_num
 * @param list  链表
 * @return
 * 获取链表节点数目
 */
int get_node_num(struct LinkData *list) {
    int cnt=0;
    struct LinkData *p;
    p = list;
    while(p != NULL) {
        cnt++;
        p=p->next;
    }
    printf("node number:%d\n",cnt);
    return cnt;
}

/**
 * @brief add
 * @param list 链表
 * @param node 节点
 * @return
 * 链表增加节点
 */
struct LinkData * add(struct LinkData *list,struct LinkData *node) {
   struct LinkData *temp;

   if(NULL == node) {
       return list;
   }
   if(NULL == list) {
       return node;
   } else {
       node->next=NULL;
       temp = list;
       while (temp->next != NULL) {
           temp = temp->next;
       }
       temp->next = node;
   }
   return list;
}

/**
 * @brief del
 * @param list 链表
 * 链表删除节点
 */
void del(struct LinkData *list) {
   struct LinkData *temp = list->next;
   if(NULL == list) {
       return;
   }
   free(list);
   list = temp;
}

/**
 * @brief del_all
 * @param list 链表
 * 链表删除所有节点
 */
void del_all(struct LinkData *list) {
   struct LinkData *temp;

   while(list != NULL) {
       temp = list;
       list = list->next;
       delete_node(temp);
   }
   list = NULL;
}
