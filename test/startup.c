#include <stdio.h>

int my_main(void);

int main(int argc, char** argv)
{
    int ret = my_main();

    printf("%d\n", ret);

    return 0;
}
