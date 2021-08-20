#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "types.h"
#include "pathfinding.h"

int distance(struct Coords from, struct Coords to) {
    return fmax(abs(from.x - to.x), abs(from.y - to.y));
}

struct Coords *get_neighbors(struct Forest forest, struct Coords coords, int *n) {
    struct Coords *neighbors = (struct Coords *) malloc(sizeof(struct Coords) * 8);

    *n = 0;
    if (coords.y > 0) {
        if (coords.x > 0) {
            char symbol = forest.cells[coords.y - 1][coords.x - 1]->type.symbol;
            if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
                neighbors[*n].x = coords.x - 1;
                neighbors[*n].y = coords.y - 1;
                ++*n;
            }
        }

        if (coords.x < forest.width - 1) {
            char symbol = forest.cells[coords.y - 1][coords.x + 1]->type.symbol;
            if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
                neighbors[*n].x = coords.x + 1;
                neighbors[*n].y = coords.y - 1;
                ++*n;
            }
        }

        char symbol = forest.cells[coords.y - 1][coords.x]->type.symbol;
        if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
            neighbors[*n].x = coords.x;
            neighbors[*n].y = coords.y - 1;
            ++*n;
        }
    }

    if (coords.y < forest.length - 1) {
        if (coords.x > 0) {
            char symbol = forest.cells[coords.y + 1][coords.x - 1]->type.symbol;
            if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
                neighbors[*n].x = coords.x - 1;
                neighbors[*n].y = coords.y + 1;
                ++*n;
            }
        }

        if (coords.x < forest.width - 1) {
            char symbol = forest.cells[coords.y + 1][coords.x + 1]->type.symbol;
            if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
                neighbors[*n].x = coords.x + 1;
                neighbors[*n].y = coords.y + 1;
                ++*n;
            }
        }

        char symbol = forest.cells[coords.y + 1][coords.x]->type.symbol;
        if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
            neighbors[*n].x = coords.x;
            neighbors[*n].y = coords.y + 1;
            ++*n;
        }
    }

    if (coords.x > 0) {
        char symbol = forest.cells[coords.y][coords.x - 1]->type.symbol;
        if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
            neighbors[*n].x = coords.x - 1;
            neighbors[*n].y = coords.y;
            ++*n;
        }
    }

    if (coords.x < forest.width - 1) {
        char symbol = forest.cells[coords.y][coords.x + 1]->type.symbol;
        if (symbol != GROUND_SYMBOL && symbol != WATER_SYMBOL && symbol != EXTINGUISHED_ASHES_SYMBOL) {
            neighbors[*n].x = coords.x + 1;
            neighbors[*n].y = coords.y;
            ++*n;
        }
    }

    neighbors = realloc(neighbors, sizeof(struct Coords) * *n);
    return neighbors;
}

struct Entry *get_entry(struct Entry *open_set, struct Coords coords) {
    struct Entry *current = open_set;
    while (current != NULL) {
        if (current->coords.x == coords.x && current->coords.y == coords.y)
            return current;

        current = current->next;
    }

    return NULL;
}

void add_entry(struct Entry **open_set, struct Entry *entry) {
    if (*open_set == NULL || (*open_set)->f_score >= entry->f_score) {
        entry->next = *open_set;
        *open_set = entry;
        return;
    }

    struct Entry *current = *open_set;
    while (current->next != NULL) {
        if (current->next->f_score >= entry->f_score) {
            entry->next = current->next;
            current->next = entry;
            return;
        }

        current = current->next;
    }

    entry->next = NULL;
    current->next = entry;
}

bool remove_entry(struct Entry **open_set, struct Coords entry) {
    if (*open_set == NULL) return false;

    if ((*open_set)->coords.x == entry.x && (*open_set)->coords.y == entry.y) {
        *open_set = (*open_set)->next;
        return true;
    }

    struct Entry *current = *open_set;
    while (current->next != NULL) {
        if (current->next->coords.x == entry.x && current->next->coords.y == entry.y) {
            current->next = current->next->next;
            return true;
        }

        current = current->next;
    }

    return false;
}

void free_open_set(struct Entry **open_set) {
    if (*open_set == NULL) return;

    struct Entry *entry = *open_set;

    if (entry->from != NULL && --entry->from->children <= 0) {
        entry->from->next = entry->next;
        *open_set = entry->from;
        entry->next = NULL;
    } else {
        *open_set = entry->next;
    }

    free(entry);
    free_open_set(open_set);
}

struct Coords *reconstruct_path(struct Entry goal, int *n) {
    *n = 0;
    struct Entry *current = &goal;
    while (current != NULL) {
        ++*n;
        current = current->from;
    }

    struct Coords *total_path = (struct Coords *) malloc(sizeof(struct Coords) * *n);

    current = &goal;
    for (int i = 0; current != NULL; ++i) {
        total_path[*n - i - 1] = current->coords;
        current = current->from;
    }

    return total_path;
}

struct Coords *a_star(struct Forest forest, struct Coords start, struct Coords goal, int *n) {
    bool *closed_set = (bool *) malloc(sizeof(bool) * forest.length * forest.width);
    for (int i = 0; i < forest.length * forest.width; ++i)
        closed_set[i] = false;

    struct Entry *open_set = (struct Entry *) malloc(sizeof(struct Entry));
    open_set->coords = start;
    open_set->f_score = distance(start, goal);
    open_set->g_score = 0;
    open_set->children = 0;
    open_set->from = NULL;
    open_set->next = NULL;

    while (open_set != NULL) {
        struct Entry *current = open_set;
        if (current->coords.x == goal.x && current->coords.y == goal.y) {
            struct Coords *path = reconstruct_path(*current, n);
            free_open_set(&open_set);
            free(closed_set);
            return path;
        }

        open_set = current->next;
        current->next = NULL;
        closed_set[current->coords.y * forest.width + current->coords.x] = true;

        int n;
        struct Coords *neighbors = get_neighbors(forest, current->coords, &n);

        for (int i = 0; i < n; ++i) {
            struct Coords neighbor = neighbors[i];
            if (closed_set[neighbor.y * forest.width + neighbor.x])
                continue;

            int tentative_g_score = current->g_score + 1;

            struct Entry *entry = get_entry(open_set, neighbor);
            if (entry == NULL) {
                struct Entry *newEntry = (struct Entry *) malloc(sizeof(struct Entry));
                newEntry->coords = neighbor;
                newEntry->f_score = tentative_g_score + distance(neighbor, goal);
                newEntry->g_score = tentative_g_score;
                newEntry->children = 0;
                newEntry->from = current;
                newEntry->next = NULL;
                ++current->children;
                add_entry(&open_set, newEntry);
            } else if (tentative_g_score < entry->g_score) {
                remove_entry(&open_set, neighbor);
                if (entry->from != NULL && --entry->from->children <= 0)
                    free(entry->from);

                entry->from = current;
                entry->g_score = tentative_g_score;
                entry->f_score = tentative_g_score + distance(neighbor, goal);
                ++current->children;
                add_entry(&open_set, entry);
            }
        }

        free(neighbors);

        if (current->children <= 0)
            free(current);
    }

    free(closed_set);

    *n = 0;
    return NULL;
}
