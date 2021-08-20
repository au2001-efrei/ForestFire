#ifndef FFSP_MATRIX_H
#define FFSP_MATRIX_H

#include <stdbool.h>

#include "types.h"

struct Forest create_forest(int length, int width);
void randomize_forest(struct Forest forest);
void input_forest(struct Forest forest);
void free_forest(struct Forest forest);

bool compare_forests(struct Forest forest1, struct Forest forest2);

void display_forest(struct Forest forest);
struct Forest iterate_forest(struct Forest forest);
bool rollback_forest(struct Forest forest);

#endif
