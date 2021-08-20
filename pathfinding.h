#ifndef FFSP_PATHFINDING_H
#define FFSP_PATHFINDING_H

#include "types.h"

struct Coords {
    int x, y;
};

struct Entry {
    struct Coords coords;
    double f_score, g_score;
    int children;
    struct Entry *from, *next;
};

int distance(struct Coords from, struct Coords to);
struct Coords *get_neighbors(struct Forest forest, struct Coords coords, int *n);

struct Entry *get_entry(struct Entry *open_set, struct Coords coords);
void add_entry(struct Entry **open_set, struct Entry *entry);
bool remove_entry(struct Entry **open_set, struct Coords entry);
void free_open_set(struct Entry **open_set);

struct Coords *reconstruct_path(struct Entry current, int *n);
struct Coords *a_star(struct Forest forest, struct Coords start, struct Coords goal, int *n);

#endif
