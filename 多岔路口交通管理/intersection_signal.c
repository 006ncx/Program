#include <stdio.h>
#include <stdlib.h>

#define N 13                        /* 路线数 */

/* 13 条可通行路线：进口道 -> 出口道 */
static const char *RT[N] = {
    "A>B", "A>C", "A>D", "B>A", "B>C", "B>D", "D>A",
    "D>B", "D>C", "E>A", "E>B", "E>C", "E>D"
};

/* 冲突矩阵：1 = 两条路线不能同时通行 */
static const int C[N][N] = {
    {0,0,0,0,1,1,1,1,0,1,1,0,0},   /* 1  A>B */
    {0,0,0,0,1,1,1,1,1,1,1,1,0},   /* 2  A>C */
    {0,0,0,0,0,1,0,0,0,1,1,1,1},   /* 3  A>D */
    {0,0,0,0,0,0,1,0,0,1,0,0,0},   /* 4  B>A */
    {1,1,0,0,0,0,0,1,1,0,1,1,0},   /* 5  B>C */
    {1,1,1,0,0,0,1,0,0,0,1,1,1},   /* 6  B>D */
    {1,1,0,1,0,1,0,0,0,1,1,1,0},   /* 7  D>A */
    {1,1,0,0,1,0,0,0,0,0,1,1,0},   /* 8  D>B */
    {0,1,0,0,1,0,0,0,0,0,0,1,0},   /* 9  D>C */
    {1,1,1,1,0,0,1,0,0,0,0,0,0},   /* 10 E>A */
    {1,1,1,0,1,1,1,1,0,0,0,0,0},   /* 11 E>B */
    {0,1,1,0,1,1,1,1,1,0,0,0,0},   /* 12 E>C */
    {0,0,1,0,0,1,0,0,0,0,0,0,0}    /* 13 E>D */
};

static int color[N];                /* 每个顶点（路线）的颜色 = 相位号 */

/* 回溯法：能否用 k 种颜色给全部顶点着色（相邻顶点不能同色） */
static int paint(int v, int k)
{
    int c, u;
    if (v == N) return 1;                       /* 13 个顶点全部着色成功 */
    for (c = 0; c < k; c++) {
        for (u = 0; u < v; u++)
            if (C[v][u] && color[u] == c) break;
        if (u < v) continue;                    /* 与已着色的邻点撞色，换一种颜色 */
        color[v] = c;
        if (paint(v + 1, k)) return 1;
        color[v] = -1;
    }
    return 0;
}

int main()
{
    int i, j, k, chi = 0, best[N];

    system("chcp 65001 > nul");      /* 让控制台按 UTF-8 编码显示中文（Windows 命令） */

    for (k = 1; k <= N; k++) {                  /* 求色数 chi(G) 及一个最优着色 */
        for (i = 0; i < N; i++) color[i] = -1;
        if (paint(0, k)) {
            chi = k;
            for (i = 0; i < N; i++) best[i] = color[i];
            break;
        }
    }

    printf("\n色数 chi(G) = %d  ->  最少相位数 = %d\n\n分相方案：\n", chi, chi);
    for (k = 0; k < chi; k++) {
        int cnt = 0;
        printf("  第 %d 相位：", k + 1);
        for (i = 0; i < N; i++)
            if (best[i] == k) { printf(" %s", RT[i]); cnt++; }
        printf("   （%d 条路线）\n", cnt);
    }

    for (i = 0; i < N; i++)                     /* 检验 1：同一相位内两两不冲突 */
        for (j = i + 1; j < N; j++)
            if (best[i] == best[j] && C[i][j])
                printf("错误：%s 与 %s 冲突却分在同一相位！\n", RT[i], RT[j]);
    for (k = 1; k < chi; k++) {                 /* 检验 2：更少的相位无解 */
        for (i = 0; i < N; i++) color[i] = -1;
        if (paint(0, k)) printf("错误：存在只用 %d 个相位的方案！\n", k);
    }
    printf("\n检验通过：每个相位内部两两不冲突，13 条路线全部覆盖，"
           "且少于 %d 个相位无解。\n", chi);
    return 0;
}
