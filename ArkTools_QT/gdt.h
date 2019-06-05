#ifndef GDT_H
#define GDT_H

#endif // GDT_H
typedef struct _GDTENTRY
{
    unsigned long long       LimitLow : 16;
    unsigned long long       BaseLow : 16;
    unsigned long long      BaseMid : 8;
    unsigned long long      Type : 4;
    unsigned long long      S : 1;
    unsigned long long      Dpl : 2;
    unsigned long long      P : 1;
    unsigned long long      LimitHi : 4;
    unsigned long long      AVL : 1;
    unsigned long long      Reserved_0 : 1;
    unsigned long long      Default_Big : 1;
    unsigned long long      G : 1;
    unsigned long long      BaseHi : 8;

}GDTENTRY, *PGDTENTRY;

char g_Atrr[16][32] = {

    "只读",
    "只读,已访问",
    "可读可写",
    "可读可写,已访问",
    "只读,向下扩展",
    "只读,向下扩展,已访问",
    "可读可写,向下扩展",
    "可读可写,向下扩展,已访问",
    "可执行","可执行,已访问",
    "可读可执行",
    "可读可执行,已访问",
    "可执行,一致代码段",
    "可执行,一致代码段,已访问",
    "可读可执行,一致代码段",
    "可读可执行,一致代码段,已访问"
};

char g_SystemAtrr[16][32] =
{
    "Reserved",
    "16位TSS(有效的)",
    "LDT",
    "16位TSS(Busy)",
    "16位调用门",
    "任务门",
    "16位中断门",
    "16位陷阱门",
    "Reserved",
    "32位TSS(有效的)",
    "Reserved",
    "32位TSS(Busy)",
    "32位调用门",
    "Reserved",
    "32位中断门",
    "32位陷阱门"
};

