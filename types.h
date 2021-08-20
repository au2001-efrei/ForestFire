#ifndef FFSP_TYPES_H
#define FFSP_TYPES_H

struct Type {
    char symbol;
    int degree;
};

struct Cell {
    struct Type type;
    int state, degree;
    struct Cell *previous;
};

struct Forest {
    int length, width;
    struct Cell ***cells;
};

#define GRASS_SYMBOL 'x'
#define GRASS_DEGREE 3
static const struct Type GRASS_TYPE = { GRASS_SYMBOL, GRASS_DEGREE };

#define TREE_SYMBOL '*'
#define TREE_DEGREE 4
static const struct Type TREE_TYPE = { TREE_SYMBOL, TREE_DEGREE };

#define GROUND_SYMBOL '+'
#define GROUND_DEGREE 0
static const struct Type GROUND_TYPE = { GROUND_SYMBOL, GROUND_DEGREE };

#define LEAF_SYMBOL ' '
#define LEAF_DEGREE 2
static const struct Type LEAF_TYPE = { LEAF_SYMBOL, LEAF_DEGREE };

#define WATER_SYMBOL '/'
#define WATER_DEGREE 0
static const struct Type WATER_TYPE = { WATER_SYMBOL, WATER_DEGREE };

#define ROCK_SYMBOL '#'
#define ROCK_DEGREE 5
static const struct Type ROCK_TYPE = { ROCK_SYMBOL, ROCK_DEGREE };

#define ASHES_SYMBOL '-'
#define ASHES_DEGREE 1
static const struct Type ASHES_TYPE = { ASHES_SYMBOL, ASHES_DEGREE };

#define EXTINGUISHED_ASHES_SYMBOL '.'
#define EXTINGUISHED_ASHES_DEGREE 0
static const struct Type EXTINGUISHED_ASHES_TYPE = { EXTINGUISHED_ASHES_SYMBOL, EXTINGUISHED_ASHES_DEGREE };

static const int TYPE_COUNT = 8;
static const struct Type TYPES[] = {{ GRASS_SYMBOL, GRASS_DEGREE }, { TREE_SYMBOL, TREE_DEGREE }, { GROUND_SYMBOL, GROUND_DEGREE }, { LEAF_SYMBOL, LEAF_DEGREE }, { WATER_SYMBOL, WATER_DEGREE }, { ROCK_SYMBOL, ROCK_DEGREE }, { ASHES_SYMBOL, ASHES_DEGREE }, { EXTINGUISHED_ASHES_SYMBOL, EXTINGUISHED_ASHES_DEGREE }};

#endif
