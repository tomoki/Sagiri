#include <stdio.h>

int main(int argc, char** argv)
{
    printf(
        "\t.text\n"
        "\t.globl my_main\n"
        "my_main:\n"
        "\tpushq %%rbp\n"
        "\tmovq %%rsp, %%rbp\n"
        "\tmovl $0, %%eax\n"
        "\tpopq %%rbp\n"
        "\tret\n");
    return 0;
}