/*
 * Can a boat get from one cell of a grid to another, stepping up, down, left
 * or right through water (true) only?
 *
 * If (c,d) is reachable from (a,b) and (e,f) is reachable from (c,d), then
 * (e,f) is reachable from (a,b). So the search only has to grow the set of
 * cells reachable from the start, one water neighbour at a time, until the
 * target turns up or no new cell is left.
 *
 * All branches of the recursion share one visited array: a cell marked by
 * one branch is skipped by the others, so each cell is searched at most once.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static bool search(bool *game_matrix, int *visited, int rows, int cols,
                   int from_row, int from_column, int to_row, int to_column)
{
    printf("search: from=(%d, %d) - to=(%d, %d)\n", from_row, from_column, to_row, to_column);
    if (from_row == to_row && from_column == to_column) {
        return true;
    }

    visited[from_row * cols + from_column] = 1;

    /* For each neighbour (below, right, above, left): skip it if it is
       already marked, search from it if it is water, mark it -1 if it is land. */
    if (from_row+1 < rows) {
        if (visited[(from_row + 1) * cols + from_column] != 0) {
           // do nothing
        } else if (*(game_matrix + (from_row + 1) * cols + from_column)) {
            visited[(from_row + 1) * cols + from_column] = 1;
            if (search(game_matrix, visited, rows, cols, from_row+1, from_column, to_row, to_column)) {
                return true;
            }
        } else {
            visited[(from_row + 1) * cols + from_column] = -1;
        }
    }

    if (from_column+1 < cols) {
        if (visited[from_row * cols + (from_column + 1)] != 0) {
           // do nothing
        } else if (*(game_matrix + from_row * cols + (from_column + 1))) {
            visited[from_row * cols + (from_column + 1)] = 1;
            if (search(game_matrix, visited, rows, cols, from_row, from_column+1, to_row, to_column)) {
                return true;
            }
        } else {
            visited[from_row * cols + (from_column + 1)] = -1;
        }
    }

    if (0 <= from_row - 1) {
        if (visited[(from_row - 1) * cols + from_column] != 0) {
           // do nothing
        } else if (*(game_matrix + (from_row - 1) * cols + from_column)) {
            visited[(from_row - 1) * cols + from_column] = 1;
            if (search(game_matrix, visited, rows, cols, from_row-1, from_column, to_row, to_column)) {
                return true;
            }
        } else {
            visited[(from_row - 1) * cols + from_column] = -1;
        }
    }

    if (0 <= from_column - 1) {
        if (visited[from_row * cols + (from_column - 1)] != 0) {
           // do nothing
        } else if (*(game_matrix + from_row * cols + (from_column - 1))) {
            visited[from_row * cols + (from_column - 1)] = 1;
            if (search(game_matrix, visited, rows, cols, from_row, from_column-1, to_row, to_column)) {
                return true;
            }
        } else {
            visited[from_row * cols + (from_column - 1)] = -1;
        }
    }

    return false;
}

bool can_travel_to(bool *game_matrix, int rows, int cols, int from_row,
                   int from_column, int to_row, int to_column)
{
    /* Check bounds before reading the grid at either cell. */
    if (!(from_row<rows && from_column<cols && to_row<rows && to_column<cols)) {
        return false;
    }
    if (from_row<0 || from_column<0 || to_row<0 || to_column<0) {
        return false;
    }
    if (!(*(game_matrix + from_row * cols + from_column))) {
        return false;
    }
    if (!(*(game_matrix + to_row * cols + to_column))) {
        return false;
    }

    /* One entry per cell, cell (r, c) at r * cols + c, all 0 from calloc:
       0 = not seen yet, 1 = reachable water, -1 = land. */
    int *visited = calloc((size_t) rows * cols, sizeof *visited);
    if (visited == NULL) {
        return false;
    }
    bool found = search(game_matrix, visited, rows, cols, from_row, from_column, to_row, to_column);
    free(visited);
    return found;
}

#ifndef RunTests
int main()
{
    bool game_matrix[6][6] = {
        {false, true,  true,  false, false, false},
        {true,  true,  true,  false, false, false},
        {true,  true,  true,  true,  true,  true},
        {false, true,  true,  false, true,  true},
        {false, true,  true,  true,  false, true},
        {false, false, false, false, false, false},
    };

    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 2, 2)); // true, Valid move
    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 3, 4)); // true, around the land at (3,3) through row 2
    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 6, 2)); // false, Out of bounds
}
#endif
