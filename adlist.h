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
    /* 指针可以指向一个结构体，定义的形式一般为：struct 结构体名 *变量名;
     * 
     * 结构体和结构体变量是两个不同的概念：结构体是一种数据类型，是一种创建变量的模板，编译器不会为它分配内存空间，就像 int、float、char 这些关键字本身不占用内存一样；
     * 结构体变量才包含实实在在的数据，才需要内存来存储。不可能去取一个结构体名的地址，也不能将它赋值给其他变量
     *
     * 通过结构体指针可以获取结构体成员，一般形式为：
     * (*pointer).memberName  // .的优先级高于*，(*pointer)两边的括号不能少
     * 或者：
     * pointer->memberName
     * 上面的两种写法是等效的，我们通常采用后面的写法，这样更加直观。
     */
    // 后置节点 (节点的前一节点 --Add by wgy)
    struct listNode *next;

    // 节点的值
    void *value;
    /* void 类型指针，无类型指针，可以指向任何类型的数据，即可以被赋予任何类型的指针，如 
     * void *p1;  
     * int *p2;    float b = 11.22;          char *c = "abc";
     * p1 = p2;    p1 = &b;                  p = c;         
     */
    //数组名在表达式中会被转换为数组指针，而结构体变量名不会，无论在任何表达式中它表示的都是整个集合本身，要想取得结构体变量的地址，必须在前面加&
} listNode;

//  结构体也是一种数据类型，它由程序员自己定义，可以包含多个其他类型的数据(可以包含多个基本类型的数据，也可以包含其他的结构体)。 
//  结构体内的成员不能初始化
//  结构体是一种自定义的数据类型，是创建变量的模板，不占用内存空间；结构体变量才包含了实实在在的数据，需要内存空间来存储。

/*  C中定义一个结构体类型用typedef后，在声明变量的时候就可：listNode listXX;（此listNode是最后那个listNode）
 *  (如果没有typedef就必须用struct listNode listXX;来声明)
 *  这里的listNode（最后那个listNode）实际上就是struct listNode的别名。listNode==struct listNode，
 *  另外第一个listNode不是必须的（ 但省略后就只能使用listNode listXX;来定义变量）
 */
/* 结构体变量名代表的是整个集合本身，作为函数参数时传递的整个集合，也就是所有成员，
 * 而不是像数组一样被编译器转换成一个指针。如果结构体成员较多，尤其是成员为数组时，传送的时间和空间开销会很大，影响程序的运行效率。
 * 所以最好的办法就是使用结构体指针，这时由实参传向形参的只是一个地址，非常快速
 */
/*
 * 双端链表迭代器
 * list迭代器，只能单向
 */
typedef struct listIter {

    // 当前迭代到的节点(当前迭代位置的下一个节点)
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

    // 链表所包含的节点数量(列表常读)
    unsigned long len;

} list;

// 宏定义的一些基本操作
/* Functions implemented as macros */
// 返回给定链表所包含的节点数量
// T = O(1)
#define listLength(l) ((l)->len)
/*
 *带参宏定义的一般形式为：
 * #define 宏名(形参列表) 字符串
 * 在字符串中含有各个形参。对带参数的宏，在调用中，不仅要宏展开，而且要用实参去代换形参。
 * e.g.     #define M(y) y*y+3*y  //宏定义
 *          // Code                    
 *          k=M(5);  //宏调用             
 *      在宏调用时，用实参5去代替形参y，经预处理宏展开后的语句为k=5*5+3*5。
 *  1) 带参宏定义中，形参之间可以出现空格，但是宏名和形参列表之间不能有空格出现。例如把：
 *  2) 在带参宏定义中，不会为形式参数分配内存，因此不必指明数据类型。而在宏调用中，实参包含了具体的数据，要用它们去代换形参，因此必须指明数据类型。
 *  3) 在宏定义中，字符串内的形参通常要用括号括起来以避免出错。这是由于替换只作符号替换而不作其它处理。
 *      如 #define SQ(y) y*y ， sq = SQ(a+1) 替换后为 sq=a+1*a+1; 与期望的 sq=(a+1)*(a+1); 不一样
 *      因此宏定义要写为 #define SQ(y) (y)*(y)
 *      如果code中 sq = 200 / SQ(a+1); 就算形参已扩起来 #define SQ(y) (y)*(y) ，但替换后
 *      出来的结果为 sq=200/(a+1)*(a+1); 等于200与期望的结果为2不一致。
 *      由此可见，对于带参宏定义不仅要在参数两侧加括号，还应该在整个字符串外加括号。
 *      如 #define SQ(y) ((y)*(y))
 *
 *   带参数的宏和函数很相似，但有本质上的区别：宏展开仅仅是字符串的替换，不会对表达式进行计算；
 *   宏在编译之前就被处理掉了，它没有机会参与编译，也不会占用内存。
 *   而函数是一段可以重复使用的代码，会被编译，会给它分配内存，每次调用函数，就是执行这块内存中的代码。
 */


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
/* 定义了方法的原型 */
list *listCreate(void);     //创建list列表
void listRelease(list *list); //列表的释放
list *listAddNodeHead(list *list, void *value); //添加列表头节点
list *listAddNodeTail(list *list, void *value); //添加列表尾节点
list *listInsertNode(list *list, listNode *old_node, void *value, int after); //在某位置上插入节点
void listDelNode(list *list, listNode *node);  //在列表上删除给定的节点
listIter *listGetIterator(list *list, int direction); //获取列表给定方向上的迭代器
listNode *listNext(listIter *iter);                   //花去迭代器内的下一个节点
void listReleaseIterator(listIter *iter);             //释放列表迭代器
list *listDup(list *orig);                            //列表的赋值
listNode *listSearchKey(list *list, void *key);       //关键字搜索具体节点
listNode *listIndex(list *list, long index);          //下标索引具体的节点
void listRewind(list *list, listIter *li);            //重置迭代器为方向从头开始
void listRewindTail(list *list, listIter *li);        //重置迭代器为方向从尾开始
void listRotate(list *list);                          //类表的旋转操作

/* Directions for iterators
 *
 * 迭代器进行迭代的方向
 */
// 从表头向表尾进行迭代
#define AL_START_HEAD 0
// 从表尾到表头进行迭代
#define AL_START_TAIL 1

#endif /* __ADLIST_H__ */
