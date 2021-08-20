#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "forest.h"
#include "menu.h"
#include "main.h"

int main(int argc, char const *argv[]) {
    srand(time(NULL));

    int length, width, n;
    do {
        printf("Which size should the forest be? (LxW, e.g. 10x10) ");
        n = scanf("%dx%d", &length, &width);
        if (n < 2) {
            char tmp[100];
            scanf("%s", tmp);

            printf("Please enter a valid size.\n");
        } else if (length <= 0 || width <= 0)
            printf("Please enter a positive size.\n");
    } while (n < 2 || length <= 0 || width <= 0);

    struct Forest forest = create_forest(length, width);
    menu(forest);
    return 0;
}
