#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "forest.h"
#include "pathfinding.h"
#include "menu.h"

void edit_cell(struct Forest forest) {
    int x, y, n;
    do {
        printf("Which cell do you want to edit? (X;Y, e.g. 2;1, type Q to cancel) ");
        n = scanf("%d;%d", &x, &y);
        if (n < 2) {
            char tmp[100];
            scanf("%s", tmp);

            if (tmp[0] == 'Q' || tmp[0] == 'q') return;

            printf("Please enter valid coordinates.\n");
        } else if (x <= 0 || y <= 0 || x > forest.width || y > forest.length)
            printf("Please enter coordinates inside the forest.\n");
    } while (n < 2 || x <= 0 || y <= 0 || x > forest.width || y > forest.length);

    printf("The current state of this cell is:\n");
    printf("Type: %c\n", forest.cells[y - 1][x - 1]->type.symbol);
    printf("Degree: %d / %d\n", forest.cells[y - 1][x - 1]->degree, forest.cells[y - 1][x - 1]->type.degree);
    printf("State: %s\n", forest.cells[y - 1][x - 1]->state? "On fire" : forest.cells[y - 1][x - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL? "Extinguished" : "Normal");

    char type;
    do {
        printf("Enter the new type of cell (use _ for space, type Q to cancel): ");
        do {
            scanf("%c", &type);
        } while (type == '\n' || type == ' ');

        for (int i = 0; i < TYPE_COUNT; ++i) {
            if (type == TYPES[i].symbol || (type == '_' && TYPES[i].symbol == ' ')) {
                type = '\0';
                struct Cell *next = (struct Cell *) malloc(sizeof(struct Cell));
                next->previous = forest.cells[y - 1][x - 1];
                next->type = TYPES[i];

                if (TYPES[i].degree > 0) {
                    do {
                        printf("Enter the new degree of this cell (maximum is %d): ", TYPES[i].degree);
                        n = scanf("%d", &next->degree);
                        if (n < 1) {
                            char tmp[100];
                            scanf("%s", tmp);

                            printf("Please enter a valid degree.\n");
                        } else if (next->degree < 0 || next->degree > TYPES[i].degree)
                            printf("Please enter a degree between 0 and %d.\n", TYPES[i].degree);
                    } while (n < 1 || next->degree < 0 || next->degree > TYPES[i].degree);
                } else next->degree = 0;

                if (TYPES[i].symbol != GROUND_SYMBOL && TYPES[i].symbol != WATER_SYMBOL && TYPES[i].symbol != EXTINGUISHED_ASHES_SYMBOL) {
                    do {
                        printf("Enter the new state of this cell (0 or 1): ");
                        n = scanf("%d", &next->state);
                        if (n < 1) {
                            char tmp[100];
                            scanf("%s", tmp);

                            printf("Please enter a valid state.\n");
                        } else if (next->state != 0 && next->state != 1)
                            printf("Please enter a state between 0 (normal/extinguished) and 1 (on fire).\n");
                    } while (n < 1 || (next->state != 0 && next->state != 1));
                } else next->state = 0;

                for (int y2 = 0; y2 < forest.length; ++y2) {
                    for (int x2 = 0; x2 < forest.width; ++x2) {
                        if (x2 != x - 1 || y2 != y - 1) {
                            struct Cell *cell = (struct Cell *) malloc(sizeof(struct Cell));
                            cell->previous = forest.cells[y2][x2];
                            cell->type = forest.cells[y2][x2]->type;
                            cell->degree = forest.cells[y2][x2]->degree;
                            cell->state = forest.cells[y2][x2]->state;
                            forest.cells[y2][x2] = cell;
                        } else forest.cells[y - 1][x - 1] = next;
                    }
                }
                break;
            }
        }

        if (type != '\0' && type != 'Q' && type != 'q')
            printf("Please enter a valid type.\n");
    } while (type != '\0' && type != 'Q' && type != 'q');
}

void run_simulation(struct Forest *forest) {
    bool fire = false;
    for (int y = 0; y < forest->length && !fire; ++y)
        for (int x = 0; x < forest->width && !fire; ++x)
            if (forest->cells[y][x]->state) fire = true;

    if (!fire) {
        int x, y, n;
        do {
            printf("No cell is on fire, enter the cell to light up (X;Y, e.g. 2;1, type Q to cancel): ");
            n = scanf("%d;%d", &x, &y);
            if (n < 2) {
                char tmp[100];
                scanf("%s", tmp);

                if (tmp[0] == 'Q' || tmp[0] == 'q') return;

                printf("Please enter valid coordinates.\n");
            } else if (x <= 0 || y <= 0 || x > forest->width || y > forest->length)
                printf("Please enter coordinates inside the forest.\n");
            else if (forest->cells[y - 1][x - 1]->type.symbol == GROUND_SYMBOL || forest->cells[y - 1][x - 1]->type.symbol == WATER_SYMBOL || forest->cells[y - 1][x - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL)
                printf("Please enter the coordinates of a flammable cell.\n");
        } while (n < 2 || x <= 0 || y <= 0 || x > forest->width || y > forest->length || forest->cells[y - 1][x - 1]->type.symbol == GROUND_SYMBOL || forest->cells[y - 1][x - 1]->type.symbol == WATER_SYMBOL || forest->cells[y - 1][x - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL);

        for (int y2 = 0; y2 < forest->length; ++y2) {
            for (int x2 = 0; x2 < forest->width; ++x2) {
                struct Cell *cell = (struct Cell *) malloc(sizeof(struct Cell));
                cell->previous = forest->cells[y2][x2];
                cell->type = forest->cells[y2][x2]->type;
                cell->degree = forest->cells[y2][x2]->degree;
                cell->state = x2 != x - 1 || y2 != y - 1? forest->cells[y2][x2]->state : 1;
                forest->cells[y2][x2] = cell;
            }
        }
    }

    int iterations, n;
    do {
        printf("Enter the number of iterations to perform (enter -1 to run the simulation until the end): ");
        n = scanf("%d", &iterations);
        if (n < 1) {
            char tmp[100];
            scanf("%s", tmp);

            printf("Please enter a valid number of iterations.\n");
        } else if (iterations < -1)
            printf("Please enter a positive number of iterations or -1 to run the simulation until the end.\n");
    } while (n < 1 || iterations < -1);

    if (iterations >= 0) {
        for (int i = 1; i <= iterations; ++i) {
            struct Forest previous_forest = *forest;
            *forest = iterate_forest(previous_forest);
            free_forest(previous_forest);

            system("clear");
            printf("Iteration #%d / %d:\n", i, iterations);
            display_forest(*forest);

            nanosleep((const struct timespec[]) {{0, 50000000L}}, NULL);
        }
    } else {
        int i = 1;
        struct Forest previous_forest = *forest;
        *forest = iterate_forest(*forest);

        while (!compare_forests(*forest, previous_forest)) {
            system("clear");
            printf("Iteration #%d:\n", i++);
            display_forest(*forest);

            nanosleep((const struct timespec[]) {{0, 50000000L}}, NULL);
            free_forest(previous_forest);
            previous_forest = *forest;
            *forest = iterate_forest(*forest);
        }

        free_forest(*forest);
        *forest = previous_forest;

        printf("\nSimulation ended!\n");
    }
}

void rollback_simulation(struct Forest forest) {
    int iterations, n;
    do {
        printf("Enter the number of iterations to rollback (enter -1 to return the simulation to its original state): ");
        n = scanf("%d", &iterations);
        if (n < 1) {
            char tmp[100];
            scanf("%s", tmp);

            printf("Please enter a valid number of iterations.\n");
        } else if (iterations < -1)
            printf("Please enter a positive number of iterations or -1 to return the simulation to its original state.\n");
    } while (n < 1 || iterations < -1);

    if (iterations >= 0) {
        for (int i = 1; i <= iterations; ++i) {
            rollback_forest(forest);

            system("clear");
            printf("Rollback #%d / %d:\n", i, iterations);
            display_forest(forest);

            nanosleep((const struct timespec[]) {{0, 50000000L}}, NULL);
        }
    } else {
        int i = 1;
        while (rollback_forest(forest)) {
            system("clear");
            printf("Rollback #%d:\n", i++);
            display_forest(forest);

            nanosleep((const struct timespec[]) {{0, 50000000L}}, NULL);
        }

        printf("\nSimulation restored to its original state.\n");
    }
}

void find_path(struct Forest forest) {
    int xA, yA, n;
    do {
        printf("Enter the coordinates of the starting cell (X;Y, e.g. 2;1, type Q to cancel): ");
        n = scanf("%d;%d", &xA, &yA);
        if (n < 2) {
            char tmp[100];
            scanf("%s", tmp);

            if (tmp[0] == 'Q' || tmp[0] == 'q') return;

            printf("Please enter valid coordinates.\n");
        } else if (xA <= 0 || yA <= 0 || xA > forest.width || yA > forest.length)
            printf("Please enter coordinates inside the forest.\n");
        else if (forest.cells[yA - 1][xA - 1]->type.symbol == GROUND_SYMBOL || forest.cells[yA - 1][xA - 1]->type.symbol == WATER_SYMBOL || forest.cells[yA - 1][xA - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL)
            printf("Please enter the coordinates of a flammable cell.\n");
    } while (n < 2 || xA <= 0 || yA <= 0 || xA > forest.width || yA > forest.length || forest.cells[yA - 1][xA - 1]->type.symbol == GROUND_SYMBOL || forest.cells[yA - 1][xA - 1]->type.symbol == WATER_SYMBOL || forest.cells[yA - 1][xA - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL);

    int xB, yB;
    do {
        printf("Enter the coordinates of the ending cell (X;Y, e.g. 1;2, type Q to cancel): ");
        n = scanf("%d;%d", &xB, &yB);
        if (n < 2) {
            char tmp[100];
            scanf("%s", tmp);

            if (tmp[0] == 'Q' || tmp[0] == 'q') return;

            printf("Please enter valid coordinates.\n");
        } else if (xB <= 0 || yB <= 0 || xB > forest.width || yB > forest.length)
            printf("Please enter coordinates inside the forest.\n");
        else if (forest.cells[yB - 1][xB - 1]->type.symbol == GROUND_SYMBOL || forest.cells[yB - 1][xB - 1]->type.symbol == WATER_SYMBOL || forest.cells[yB - 1][xB - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL)
            printf("Please enter the coordinates of a flammable cell.\n");
    } while (n < 2 || xB <= 0 || yB <= 0 || xB > forest.width || yB > forest.length || forest.cells[yB - 1][xB - 1]->type.symbol == GROUND_SYMBOL || forest.cells[yB - 1][xB - 1]->type.symbol == WATER_SYMBOL || forest.cells[yB - 1][xB - 1]->type.symbol == EXTINGUISHED_ASHES_SYMBOL);

    struct Coords start = { xA - 1, yA - 1 }, goal = { xB - 1, yB - 1 };
    struct Coords *path = a_star(forest, start, goal, &n);

    if (path != NULL) {
        struct Forest copy;
        copy.length = forest.length;
        copy.width = forest.width;

        copy.cells = (struct Cell ***) malloc(sizeof(struct Cell **) * forest.length);

        for (int y = 0; y < forest.length; ++y) {
            copy.cells[y] = (struct Cell **) malloc(sizeof(struct Cell *) * forest.width);

            for (int x = 0; x < forest.width; ++x) {
                copy.cells[y][x] = (struct Cell *) malloc(sizeof(struct Cell));
                copy.cells[y][x]->previous = NULL;
                copy.cells[y][x]->type = forest.cells[y][x]->type;
                copy.cells[y][x]->degree = forest.cells[y][x]->degree;
                copy.cells[y][x]->state = 0;
            }
        }

        for (int i = 0; i < n; ++i) {
            copy.cells[path[i].y][path[i].x]->state = 1;

            system("clear");
            printf("Here is the shortest path between %d;%d and %d;%d (%d iterations):\n", xA, yA, xB, yB, n);
            display_forest(copy);

            nanosleep((const struct timespec[]) {{0, 50000000L}}, NULL);
        }

        free_forest(copy);
        free(path);
    } else {
        printf("No path could be found between these two points.\n");
    }
}

void menu(struct Forest forest) {
    int option, n;
    do {
        printf("Select an option among:\n");
        printf("1. Display the current state of the forest\n");
        printf("2. Randomly fill the forest\n");
        printf("3. Fill the forest manually\n");
        printf("4. Change a cell's type, degree or state\n");
        printf("5. Simulate a fire for a number of iterations\n");
        printf("6. Rollback the simulation by a number of iterations\n");
        printf("7. Find the path between two points fire can spread on\n");
        printf("0. Leave the simulation\n");
        n = scanf("%d", &option);
        if (n < 1) {
            char tmp[100];
            scanf("%s", tmp);

            printf("Please enter a valid option.\n");
        } else if (option < 0 || option > 7)
            printf("Please enter an option between 0 and 7.\n");
    } while (n < 1 || option < 0 || option > 7);
    system("clear");

    switch(option) {
    case 1:
        printf("Here is the current forest:\n");
        display_forest(forest);
        printf("\n");
        break;

    case 2:
        randomize_forest(forest);
        printf("Here is the randomly generated forest:\n");
        display_forest(forest);
        printf("\n");
        break;

    case 3:
        input_forest(forest);
        system("clear");
        printf("Here is the forest:\n");
        display_forest(forest);
        printf("\n");
        break;

    case 4:
        edit_cell(forest);
        system("clear");
        printf("Here is the new forest:\n");
        display_forest(forest);
        printf("\n");
        break;

    case 5:
        run_simulation(&forest);
        printf("\n");
        break;

    case 6:
        rollback_simulation(forest);
        printf("\n");
        break;

    case 7:
        find_path(forest);
        printf("\n");
        break;

    case 0:
        return;
    }

    menu(forest);
}
