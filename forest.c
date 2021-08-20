#include <stdio.h>
#include <stdlib.h>

#include "forest.h"
#include "stdbool.h"

struct Forest create_forest(int length, int width) {
    struct Forest forest;
    forest.length = length;
    forest.width = width;

    forest.cells = (struct Cell ***) malloc(sizeof(struct Cell **) * length);

    for (int y = 0; y < length; ++y) {
        forest.cells[y] = (struct Cell **) malloc(sizeof(struct Cell *) * width);

        for (int x = 0; x < width; ++x) {
            struct Cell *cell = (struct Cell *) malloc(sizeof(struct Cell));
            cell->previous = NULL;
            cell->type = GROUND_TYPE;
            cell->degree = cell->type.degree;
            cell->state = 0;
            forest.cells[y][x] = cell;
        }
    }

    return forest;
}

void input_forest(struct Forest forest) {
    printf("Enter the forest, for example by copy-pasting a previous one (use _ for spaces):\n");

    for (int y = 0; y < forest.length; ++y) {
        for (int x = 0; x < forest.width; ++x) {
            char type;
            do {
                scanf("%c", &type);
            } while (type == '\n' || type == ' ');

            for (int i = 0; i < TYPE_COUNT; ++i) {
                if (type == TYPES[i].symbol || (type == '_' && TYPES[i].symbol == ' ')) {
                    type = '\0';
                    struct Cell *next = (struct Cell *) malloc(sizeof(struct Cell));
                    next->previous = forest.cells[y][x];
                    next->type = TYPES[i];
                    next->degree = TYPES[i].degree;
                    next->state = 0;
                    forest.cells[y][x] = next;
                    break;
                }
            }

            if (type != '\0') {
                printf("You entered an invalid type, try again.\n");
                --x;
            }
        }
    }
}

void randomize_forest(struct Forest forest) {
    int proportions[] = { 6, 2, 4, 2, 1, 2, 0, 0 };

    int total = 0;
    for (int i = 0; i < TYPE_COUNT; ++i)
        total += proportions[i];

    for (int y = 0; y < forest.length; ++y) {
        for (int x = 0; x < forest.width; ++x) {
            int r = rand() % total;

            int type = 0;
            while (r >= proportions[type]) {
                r -= proportions[type];
                ++type;
            }

            struct Cell *next = (struct Cell *) malloc(sizeof(struct Cell));
            next->previous = forest.cells[y][x];
            next->type = TYPES[type];
            next->degree = TYPES[type].degree;
            next->state = 0;
            forest.cells[y][x] = next;
        }
    }
}

void free_forest(struct Forest forest) {
    // TODO
}

bool compare_forests(struct Forest forest1, struct Forest forest2) {
    if (forest1.length != forest2.length || forest1.width != forest2.width)
        return false;

    for (int y = 0; y < forest1.length; ++y)
        for (int x = 1; x < forest1.width; ++x)
            if (forest1.cells[y][x]->type.symbol != forest2.cells[y][x]->type.symbol || forest1.cells[y][x]->degree != forest2.cells[y][x]->degree || forest1.cells[y][x]->state != forest2.cells[y][x]->state)
                return false;

    return true;
}

void display_forest(struct Forest forest) {
    for (int y = 0; y < forest.length; ++y) {
        if (forest.cells[y][0]->state)
            printf("\033[37;41m%c\033[0m", forest.cells[y][0]->type.symbol);
        else printf("%c", forest.cells[y][0]->type.symbol);

        for (int x = 1; x < forest.width; ++x) {
            if (forest.cells[y][x]->state) {
                if (forest.cells[y][x - 1]->state)
                    printf("\033[37;41m %c\033[0m", forest.cells[y][x]->type.symbol);
                else printf(" \033[37;41m%c\033[0m", forest.cells[y][x]->type.symbol);
            } else printf(" %c", forest.cells[y][x]->type.symbol);
        }
        printf("\n");
    }
}

int get_surrounding_state(struct Forest forest, int x, int y) {
    for (int i = -1; i <= 1; ++i) {
        if (y + i < 0 || y + i >= forest.length) continue;

        for (int j = -1; j <= 1; ++j) {
            if (x + j < 0 || x + j >= forest.width || (i == 0 && j == 0)) continue;

            if (forest.cells[y + i][x + j]->state)
                return 1;
        }
    }

    return 0;
}

void iterate_cell(struct Forest forest, int x, int y, struct Forest next) {
    next.cells[y][x]->previous = forest.cells[y][x];
    if (forest.cells[y][x]->state) {
        if (forest.cells[y][x]->degree > 2) {
            next.cells[y][x]->type = forest.cells[y][x]->type;
            next.cells[y][x]->degree = forest.cells[y][x]->degree - 1;
            next.cells[y][x]->state = 1;
        } else if (forest.cells[y][x]->type.symbol != ASHES_SYMBOL && forest.cells[y][x]->type.symbol != EXTINGUISHED_ASHES_SYMBOL) {
            next.cells[y][x]->type = ASHES_TYPE;
            next.cells[y][x]->degree = ASHES_DEGREE;
            next.cells[y][x]->state = 1;
        } else {
            next.cells[y][x]->type = EXTINGUISHED_ASHES_TYPE;
            next.cells[y][x]->degree = EXTINGUISHED_ASHES_DEGREE;
            next.cells[y][x]->state = 0;
        }
    } else {
        switch(forest.cells[y][x]->type.symbol) {
        case TREE_SYMBOL:
        case ROCK_SYMBOL:
        case GRASS_SYMBOL:
        case LEAF_SYMBOL:
        case ASHES_SYMBOL:
            next.cells[y][x]->type = forest.cells[y][x]->type;
            next.cells[y][x]->degree = forest.cells[y][x]->degree;
            next.cells[y][x]->state = get_surrounding_state(forest, x, y);
            break;
        case GROUND_SYMBOL:
        case WATER_SYMBOL:
        case EXTINGUISHED_ASHES_SYMBOL:
        default:
            next.cells[y][x]->type = forest.cells[y][x]->type;
            next.cells[y][x]->degree = forest.cells[y][x]->degree;
            next.cells[y][x]->state = forest.cells[y][x]->state;
        }
    }
}

struct Forest iterate_forest(struct Forest forest) {
    struct Forest next;
    next.length = forest.length;
    next.width = forest.width;

    next.cells = (struct Cell ***) malloc(sizeof(struct Cell **) * forest.length);

    for (int y = 0; y < forest.length; ++y) {
        next.cells[y] = (struct Cell **) malloc(sizeof(struct Cell *) * forest.width);

        for (int x = 0; x < forest.width; ++x) {
            next.cells[y][x] = (struct Cell *) malloc(sizeof(struct Cell));
            iterate_cell(forest, x, y, next);
        }
    }

    return next;
}

bool rollback_forest(struct Forest forest) {
    for (int y = 0; y < forest.length; ++y) {
        for (int x = 0; x < forest.width; ++x) {
            if (forest.cells[y][x]->previous == NULL)
                return false;
        }
    }

    for (int y = 0; y < forest.length; ++y) {
        for (int x = 0; x < forest.width; ++x) {
            struct Cell *cell = forest.cells[y][x];
            forest.cells[y][x] = cell->previous;
            free(cell);
        }
    }

    return true;
}
