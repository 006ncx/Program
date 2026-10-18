/* 候补车票系统：链式队列，先候补先出票
   1 新增候补订单  2 余票发放  3 查看队列  0 退出 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#endif

typedef struct Order {
    char orderNo[32];
    char name[40];
    char idCard[24];
    int  tickets;
    struct Order *next;
} Order;

typedef struct {
    Order *front;                       /* 队头：最先出票 */
    Order *rear;                        /* 队尾：最后入队 */
    int    count;
} Queue;

/* 初始化队列 */
static void initQueue(Queue *q)
{
    q->front = q->rear = NULL;
    q->count = 0;
}

/* 判断队列是否为空 */
static int isEmpty(Queue *q)
{
    return q->front == NULL;
}

/* 入队：新订单加到队尾 */
static void enqueue(Queue *q, Order *o)
{
    o->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = o;
    } else {
        q->rear->next = o;
        q->rear = o;
    }
    q->count++;
}

/* 出队：取走队头结点并返回 */
static Order *dequeue(Queue *q)
{
    Order *o;

    if (isEmpty(q)) return NULL;
    o = q->front;
    q->front = o->next;
    if (q->front == NULL) q->rear = NULL;   /* 变空时队尾必须置空 */
    o->next = NULL;
    q->count--;
    return o;
}

/* 释放队列中的全部结点 */
static void clearQueue(Queue *q)
{
    Order *o;

    while ((o = q->front) != NULL) {
        q->front = o->next;
        free(o);
    }
    q->rear = NULL;
    q->count = 0;
}

/* 统计全部订单所需票数之和 */
static int totalTickets(Queue *q)
{
    int sum = 0;
    Order *o;

    for (o = q->front; o != NULL; o = o->next) sum += o->tickets;
    return sum;
}

/* 打印一条订单信息 */
static void printOrder(const Order *o, const char *tag)
{
    printf("%s订单号 %s  姓名 %s  身份证号 %s  需 %d 张\n",
           tag, o->orderNo, o->name, o->idCard, o->tickets);
}

/* 校验身份证号格式 */
static int validId(const char *s)
{
    int i, n = (int)strlen(s);

    if (n != 15 && n != 18) return 0;
    for (i = 0; i < n; i++) {
        if (i == 17 && (s[i] == 'X' || s[i] == 'x')) continue;
        if (!isdigit((unsigned char)s[i])) return 0;
    }
    return 1;
}

/* 读取一行非空文本 */
static void readLine(const char *prompt, char *buf, size_t n)
{
    for (;;) {
        printf("%s", prompt);
        if (fgets(buf, (int)n, stdin) == NULL) { buf[0] = '\0'; return; }
        buf[strcspn(buf, "\r\n")] = '\0';
        if (buf[0] != '\0') return;
        printf("  输入不能为空，请重新输入。\n");
    }
}

/* 读取指定范围内的整数 */
static int readInt(const char *prompt, int min, int max)
{
    char line[32];
    int v;

    for (;;) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin) == NULL) return min;
        if (sscanf(line, "%d", &v) == 1 && v >= min && v <= max) return v;
        printf("  请输入 %d~%d 之间的整数。\n", min, max);
    }
}

/* 功能 1：新增候补订单，加入队尾 */
static void addOrder(Queue *q)
{
    Order *o = (Order *)malloc(sizeof(Order));

    if (o == NULL) { printf("内存不足，新增失败。\n"); return; }
    memset(o, 0, sizeof(Order));

    readLine("订单号：", o->orderNo, sizeof(o->orderNo));
    readLine("姓名：", o->name, sizeof(o->name));
    if (o->orderNo[0] == '\0' || o->name[0] == '\0') { free(o); return; }

    for (;;) {
        readLine("身份证号：", o->idCard, sizeof(o->idCard));
        if (o->idCard[0] == '\0') { free(o); return; }
        if (validId(o->idCard)) break;
        printf("  格式不对：15 位或 18 位数字，18 位末位可为 X。\n");
    }

    o->tickets = readInt("所需票数(1~5)：", 1, 5);
    enqueue(q, o);
    printf("已入队，排在第 %d 位。\n", q->count);
}

/* 功能 2：余票发放，从队头依次出票 */
static void releaseTickets(Queue *q)
{
    int k, remain, done = 0;
    Order *o;

    if (isEmpty(q)) { printf("候补队列为空，无需发放。\n"); return; }

    k = readInt("本次释放余票 k：", 1, 100000);
    remain = k;

    while (remain > 0 && !isEmpty(q)) {
        if (q->front->tickets > remain) {       /* 余票不够，不拆单 */
            printf("队头 %s 需 %d 张，仅剩 %d 张，本次无法出票。\n",
                   q->front->orderNo, q->front->tickets, remain);
            break;
        }
        o = dequeue(q);                 /* 能满足才出队 */
        remain -= o->tickets;
        done++;
        printOrder(o, "[出票成功] ");
        free(o);
    }

    printf("本次释放 %d 张，出票 %d 单，剩余 %d 张。\n", k, done, remain);
    if (!isEmpty(q))
        printf("队列还有 %d 单、%d 张票在等待。\n", q->count, totalTickets(q));
}

/* 功能 3：显示当前候补队列 */
static void showQueue(Queue *q)
{
    int i = 1;
    Order *o;

    if (isEmpty(q)) { printf("队列为空。\n"); return; }

    printf("当前候补队列（队头 -> 队尾）：\n");
    for (o = q->front; o != NULL; o = o->next)
        printf("  %d. %s  %s  %s  需 %d 张\n",
               i++, o->orderNo, o->name, o->idCard, o->tickets);
    printf("共 %d 单，%d 张票。\n", q->count, totalTickets(q));
}

/* 主函数：显示菜单并循环处理 1/2/3/0 */
int main(void)
{
    Queue q;
    char line[32];
    int running = 1;

#ifdef _WIN32
    SetConsoleOutputCP(65001);
#endif
    initQueue(&q);
    printf("候补车票系统：按提交时间排队，有票时从队头依次出票\n");

    while (running) {
        printf("\n---------------------------\n");
        printf("1 新增候补订单   2 余票发放\n");
        printf("3 查看队列       0 退出\n");
        printf("当前 %d 单，共 %d 张票\n选择：", q.count, totalTickets(&q));

        if (fgets(line, sizeof(line), stdin) == NULL) break;
        switch (line[0]) {
        case '1': addOrder(&q); break;
        case '2': releaseTickets(&q); break;
        case '3': showQueue(&q); break;
        case '0': running = 0; break;
        default:  printf("无效选择，请输入 1、2、3 或 0。\n");
        }
    }

    clearQueue(&q);
    printf("已退出。\n");
    return 0;
}
