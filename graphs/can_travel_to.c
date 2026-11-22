/*
 * can_travel_to.c
 *
 * Question: on a grid of water (true) and land (false) cells, can a boat get
 * from (from_row, from_column) to (to_row, to_column) by stepping one cell
 * up, down, left or right at a time, through water only?
 *
 * Idea
 * ----
 * Write C(a,b) for the set of cells that some path of such steps connects to
 * the start (a,b). Paths join end to end: if (c,d) is in C(a,b) and (e,f) is
 * in C(c,d), then (e,f) is in C(a,b). So C(a,b) can be grown from (a,b) one
 * step at a time, because every water neighbour of a cell in C(a,b) is also
 * in C(a,b). The search stops with true as soon as the target enters C(a,b).
 * It stops with false once every cell in C(a,b) has had its neighbours
 * checked and none of them was the target.
 *
 * A cell's neighbours never change, so checking them once is enough. The
 * recursion therefore shares one visited array across all its branches, and
 * a call skips any cell that is already marked: the call that marked it has
 * checked, or is still checking, that cell's neighbours. Without the marks,
 * two neighbouring water cells would call search on each other forever.
 * Sharing one array keeps memory at one entry per cell, where a copy per
 * branch would cost one array per level of recursion.
 *
 * Only the caller's grid says how many cells there are, so can_travel_to
 * allocates visited on the heap with rows * cols entries for each query and
 * frees it before returning.
 *
 * Cost: each cell gets at most one search call, so a query makes at most
 * rows * cols calls. When the water forms one long winding path, those calls
 * all nest, and the stack holds up to rows * cols frames.
 *
 * Compile with -DTRACE to print every search call.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/*
 * Search C(a,b) depth first from (from_row, from_column), and return true as
 * soon as some call reaches (to_row, to_column).
 *
 * game_matrix and visited both hold a rows x cols grid stored row after row,
 * so cell (r, c) sits at index r * cols + c in each. Each entry of visited
 * holds one of three states:
 *    0  not looked at yet
 *    1  water the boat can reach from the start, i.e. a member of C(a,b)
 *   -1  land, already looked at, so no call reads it from the grid again
 *
 * Each call looks at the four neighbours of its from cell in turn. An
 * unmarked water neighbour joins C(a,b) and the call searches from it at
 * once; an unmarked land neighbour gets marked -1.
 *
 * search does not check its from and to cells: can_travel_to has checked
 * that both lie inside the grid and are water, and search only ever calls
 * itself on a water cell inside the grid.
 */
static bool search(bool *game_matrix, int *visited, int rows, int cols,
                   int from_row, int from_column, int to_row, int to_column)
{
#ifdef TRACE
    printf("search: from=(%d, %d) - to=(%d, %d)\n", from_row, from_column, to_row, to_column);
#endif

    /* Stop with true: this call stands on the target. */
    if (from_row == to_row && from_column == to_column) {
        return true;
    }

    /* Add this cell to C(a,b). Only the start cell gets its mark here: every
       other cell was marked by the call that found it. */
    visited[from_row * cols + from_column] = 1;

    /* Neighbour below: (from_row + 1, from_column), if it lies inside the grid. */
    if (from_row+1 < rows) {
        /* Already marked 1 or -1: skip it. */
        if (visited[(from_row + 1) * cols + from_column] != 0) {
           // do nothing
        } else if (*(game_matrix + (from_row + 1) * cols + from_column)) {
            /* Unmarked water: add it to C(a,b) and search from it. A true
               answer from that branch ends the whole search. */
            visited[(from_row + 1) * cols + from_column] = 1;
            if (search(game_matrix, visited, rows, cols, from_row+1, from_column, to_row, to_column)) {
                return true;
            }
        } else {
            /* Unmarked land: mark it, so no call reads it from the grid again. */
            visited[(from_row + 1) * cols + from_column] = -1;
        }
    }

    /* Neighbour to the right: (from_row, from_column + 1). Same steps as above. */
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

    /* Neighbour above: (from_row - 1, from_column). Same steps as above. */
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

    /* Neighbour to the left: (from_row, from_column - 1). Same steps as above. */
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

    /* No branch from this cell reached the target. When the first call
       returns here, every cell in C(a,b) has had its neighbours checked and
       none was the target, so no path exists. */
    return false;
}

/*
 * Check both cells once, then search from the start. Returns false when
 * either cell lies outside the grid or on land, and also when visited cannot
 * be allocated, so a false can mean out of memory as well as no path.
 */
bool can_travel_to(bool *game_matrix, int rows, int cols, int from_row,
                   int from_column, int to_row, int to_column)
{
    /* Bounds come first: the water checks below read the grid at both cells,
       and a cell outside the grid would make them read memory outside
       game_matrix. These checks also reject a grid with rows or cols <= 0. */
    if (!(from_row<rows && from_column<cols && to_row<rows && to_column<cols)) {
        return false;
    }
    if (from_row<0 || from_column<0 || to_row<0 || to_column<0) {
        return false;
    }

    /* Both cells must be water: the boat cannot start on land, and no path
       can end there. */
    if (!(*(game_matrix + from_row * cols + from_column))) {
        return false;
    }
    if (!(*(game_matrix + to_row * cols + to_column))) {
        return false;
    }

    /* calloc sets every entry to 0, "not looked at yet", so the search starts
       without marks from any earlier query. */
    int *visited = calloc((size_t) rows * cols, sizeof *visited);
    if (visited == NULL) {
        return false;
    }

    bool found = search(game_matrix, visited, rows, cols, from_row, from_column, to_row, to_column);
    free(visited);
    return found;
}

/* Compile with -DRunTests to leave out these tests, for example when a test
   harness supplies its own main. */
#ifndef RunTests

/* main exits with 1 when any check fails, so a script can tell. */
static int failures = 0;

static void check(const char *name, bool got, bool expected)
{
    printf("%s  %s\n", got == expected ? "PASS" : "FAIL", name);
    if (got != expected) {
        failures++;
    }
}

int main(void)
{
    /* true = water, false = land. Row 0 is the top row. */
    bool game_matrix[6][6] = {
        {false, true,  true,  false, false, false},
        {true,  true,  true,  false, false, false},
        {true,  true,  true,  true,  true,  true},
        {false, true,  true,  false, true,  true},
        {false, true,  true,  true,  false, true},
        {false, false, false, false, false, false},
    };
    bool *m = (bool *) game_matrix;

    check("(3,2) -> (2,2): water next door",
          can_travel_to(m, 6, 6, 3, 2, 2, 2), true);
    check("(3,2) -> (3,4): round the land at (3,3) by (2,2), (2,3), (2,4)",
          can_travel_to(m, 6, 6, 3, 2, 3, 4), true);
    check("(3,2) -> (3,3): target on land",
          can_travel_to(m, 6, 6, 3, 2, 3, 3), false);
    check("(0,0) -> (1,1): start on land",
          can_travel_to(m, 6, 6, 0, 0, 1, 1), false);
    check("(3,2) -> (6,2): target below the grid",
          can_travel_to(m, 6, 6, 3, 2, 6, 2), false);
    check("(3,2) -> (-1,2): target above the grid",
          can_travel_to(m, 6, 6, 3, 2, -1, 2), false);
    check("(3,2) -> (3,2): start is the target",
          can_travel_to(m, 6, 6, 3, 2, 3, 2), true);
    check("(0,0) -> (0,0): start is the target, on land",
          can_travel_to(m, 6, 6, 0, 0, 0, 0), false);
    check("(6,6) -> (6,6): start is the target, outside the grid",
          can_travel_to(m, 6, 6, 6, 6, 6, 6), false);

    /* A column of land splits this grid in two, so the search must run out
       of cells without finding the target. */
    bool split[3][3] = {
        {true, false, true},
        {true, false, true},
        {true, false, true},
    };
    check("split grid (0,0) -> (0,2): water behind a wall of land",
          can_travel_to((bool *) split, 3, 3, 0, 0, 0, 2), false);

    /* A grid larger than 100 x 100, all water. */
    static bool big[150][150];
    for (int i = 0; i < 150; i++) {
        for (int j = 0; j < 150; j++) {
            big[i][j] = true;
        }
    }
    check("150 x 150 open water (0,0) -> (149,149)",
          can_travel_to((bool *) big, 150, 150, 0, 0, 149, 149), true);

    return failures != 0;
}
#endif
