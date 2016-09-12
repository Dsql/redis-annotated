/* adlist.h - A generic doubly linked list implementation
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ADLIST_H__
#define __ADLIST_H__

/** --Add by wgy
 *  上面两行 code 为 宏定义，目的是为了进行条件编译。
 *  一般情况下，源程序中所有的行都参加编译。但是有时希望对其中一部分内容只在满足一定条件才进行编译，
 *  也就是对一部分内容指定编译的条件，这就是“条件编译”。
 *     条件编译命令最常见的形式为:
 *           #ifdef 标识符
 *           程序段1
 *           #else
 *           程序段2
 *           #endif
 *     它的作用是： 当标识符已将被定义过(一般使用#define定义)，则对程序段1进行编译，否则编译程序段2
 *     将“ifdef”改为“ifndef”。
 *     它的作用是：若标识符未被定义则编译程序段1，否则编译程序段2。这种形式与第一种形式的作用相反。
 *     
 *  例如，我们有一个数据类型，在Windows平台中，应该使用long类型表示，而在其他平台应该使用float表示，这样往往需要对源程序作必要的修改，这就降低了程序的通用性。
 *  可以用以下的条件编译：
 *      #ifdef WINDOWS      
 *      #define MYTYPE long 
 *      #else               
 *      #define MYTYPE float
 *      #endif              
 *  如果在Windows上编译程序，则可以在程序的开始加上 #define WINDOWS
 *      这样则编译下面的命令行： #define MYTYPE long
 *  如果在这组条件编译命令之前曾出现以下命令行：  #define WINDOWS 0
 *      则预编译后程序中的MYTYPE都用float代替。因0表示假，非0为真
 *  这样，源程序可以不必作任何修改就可以用于不同类型的计算机系统。
 *
 *  通常，标准库用用一个下划线，编译器内部用两个下划线
 */


/* Node, List, and Iterator are the only data structures used currently. */
/* Iterator 迭代器  --Add by wgy，通俗来说就是循环，每一次叫做一次迭代
 *                    迭代器（iterator）有时又称游标（cursor）是一个很典型的设计模式，它总是用同一种逻辑来遍历集合（数组、链表等），
 *                    无需关心集合的内部实现
 */
 
/*
 * 双端链表节点
 * 双向链表  --Add by wgy
 */
typedef struct listNode {

    // 前置节点 (节点的前一节点 --Add by wgy)
    struct listNode *prev;

    // 后置节点 (节点的前一节点 --Add by wgy)
    struct listNode *next;

    // 节点的值
    void *value;

} listNode;

/*  C中定义一个结构体类型用typedef后，在声明变量的时候就可：listNode listXX;（最后那个listNode）(如果没有typedef就必须用struct listNode listXX;来声明)
 *  这里的listNode（最后那个listNode）实际上就是struct listNode的别名。listNode==struct listNode，另外第一个listNode不是必须的（ 但省略后就只能使用listNode listXX;来定义变量）
 */

/*
 * 双端链表迭代器
 */
typedef struct listIter {

    // 当前迭代到的节点
    listNode *next;

    // 迭代的方向
    int direction;

} listIter;

/*
 * 双端链表结构
 */
typedef struct list {

    // 表头节点
    listNode *head;

    // 表尾节点
    listNode *tail;

    // 节点值复制函数
    void *(*dup)(void *ptr);

    // 节点值释放函数
    void (*free)(void *ptr);

    // 节点值对比函数
    int (*match)(void *ptr, void *key);

    // 链表所包含的节点数量
    unsigned long len;

} list;

/* Functions implemented as macros */
// 返回给定链表所包含的节点数量
// T = O(1)
#define listLength(l) ((l)->len)
// 返回给定链表的表头节点
// T = O(1)
#define listFirst(l) ((l)->head)
// 返回给定链表的表尾节点
// T = O(1)
#define listLast(l) ((l)->tail)
// 返回给定节点的前置节点
// T = O(1)
#define listPrevNode(n) ((n)->prev)
// 返回给定节点的后置节点
// T = O(1)
#define listNextNode(n) ((n)->next)
// 返回给定节点的值
// T = O(1)
#define listNodeValue(n) ((n)->value)

// 将链表 l 的值复制函数设置为 m
// T = O(1)
#define listSetDupMethod(l,m) ((l)->dup = (m))
// 将链表 l 的值释放函数设置为 m
// T = O(1)
#define listSetFreeMethod(l,m) ((l)->free = (m))
// 将链表的对比函数设置为 m
// T = O(1)
#define listSetMatchMethod(l,m) ((l)->match = (m))

// 返回给定链表的值复制函数
// T = O(1)
#define listGetDupMethod(l) ((l)->dup)
// 返回给定链表的值释放函数
// T = O(1)
#define listGetFree(l) ((l)->free)
// 返回给定链表的值对比函数
// T = O(1)
#define listGetMatchMethod(l) ((l)->match)

/* Prototypes */
list *listCreate(void);
void listRelease(list *list);
list *listAddNodeHead(list *list, void *value);
list *listAddNodeTail(list *list, void *value);
list *listInsertNode(list *list, listNode *old_node, void *value, int after);
void listDelNode(list *list, listNode *node);
listIter *listGetIterator(list *list, int direction);
listNode *listNext(listIter *iter);
void listReleaseIterator(listIter *iter);
list *listDup(list *orig);
listNode *listSearchKey(list *list, void *key);
listNode *listIndex(list *list, long index);
void listRewind(list *list, listIter *li);
void listRewindTail(list *list, listIter *li);
void listRotate(list *list);

/* Directions for iterators
 *
 * 迭代器进行迭代的方向
 */
// 从表头向表尾进行迭代
#define AL_START_HEAD 0
// 从表尾到表头进行迭代
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */
