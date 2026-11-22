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
 * Cost: each cell gets at most one search call, so a run makes at most
 * rows * cols calls. When the water forms one long winding path, those calls
 * all nest, and the stack holds up to rows * cols frames.
 *
 * The code is kept as first written. Comments marked NOTE describe known
 * problems in it.
 */

#include <stdio.h>
#include <stdbool.h>

/*
 * One entry per grid cell, shared by every call to search:
 *    0  not looked at yet
 *    1  water the boat can reach from the start, i.e. a member of C(a,b)
 *   -1  land, already looked at, so no call reads it from the grid again
 *
 * NOTE: the array holds 100 x 100 cells, and nothing checks that rows and
 * cols stay within 100.
 */
int visited[100][100];

/* Clear the part of visited that the grid uses, so that a call to
   can_travel_to starts without marks left by the previous call. */
void init_visited(int rows, int cols) {
    for (int i = 0; i<rows; i++) {
        for (int j = 0; j<cols; j++) {
            visited[i][j] = 0;
        }
    }
}

/*
 * Search C(a,b) depth first from (from_row, from_column), and return true as
 * soon as some call reaches (to_row, to_column).
 *
 * game_matrix points at the first cell of a rows x cols grid stored row after
 * row, so cell (r, c) sits at game_matrix + r * cols + c.
 *
 * Each call looks at the four neighbours of its from cell in turn. An
 * unmarked water neighbour joins C(a,b) and the call searches from it at
 * once; an unmarked land neighbour gets marked -1. The target is the same in
 * every call; only the from cell changes.
 */
static bool search(bool *game_matrix, int rows, int cols,
                   int from_row, int from_column, int to_row, int to_column)
{
    /* Trace each call, to watch the search spread across the grid. */
    printf("search: from=(%d, %d) - to=(%d, %d)\n", from_row, from_column, to_row, to_column);

    /* Stop with true: this call stands on the target.
       NOTE: this check runs before the validity checks below, so it answers
       true whenever start and target are the same cell, even if that cell is
       land or outside the grid. */
    if (from_row == to_row && from_column == to_column) {
        return true;
    }

    /* The four checks below only matter in the first call, from
       can_travel_to. Every later call starts on a water cell inside the
       grid, because the neighbour checks further down only ever call search
       on such a cell, and the target never changes. */

    /* The target must be water, or no path can end there.
       NOTE: this line reads the target cell before the bounds checks below
       have run. With a target outside the grid, such as (6, 2) in the third
       test in main or any negative coordinate, it reads memory outside
       game_matrix, which is undefined behaviour; gcc -fsanitize=address
       reports a stack-buffer-overflow here. Moving the two bounds checks
       above this one fixes it. */
    if (!(*(game_matrix + to_row * cols + to_column))) {
        return false;
    }

    /* Both cells must lie inside the grid: below rows and cols ... */
    if (!(from_row<rows && from_column<cols && to_row<rows && to_column<cols)) {
        return false;
    }
    /* ... and at or above 0. */
    if (from_row<0 || from_column<0 || to_row<0 || to_column<0) {
        return false;
    }

    /* The start must be water. */
    if (!(*(game_matrix + from_row * cols + from_column))) {
        return false;
    }

    /* Add this cell to C(a,b). Only the start cell gets its mark here: every
       other cell was marked by the call that found it. */
    visited[from_row][from_column] = 1;

    /* Neighbour below: (from_row + 1, from_column), if it lies inside the grid. */
    if (from_row+1 < rows) {
        /* Already marked 1 or -1: skip it. */
        if (visited[from_row+1][from_column] != 0) {
           // do nothing
        } else if (*(game_matrix + (from_row + 1) * cols + from_column)) {
            /* Unmarked water: add it to C(a,b) and search from it. A true
               answer from that branch ends the whole search. */
            visited[from_row+1][from_column] = 1;
            if (search(game_matrix, rows, cols, from_row+1, from_column, to_row, to_column)) {
                return true;
            }
        } else {
            /* Unmarked land: mark it, so no call reads it from the grid again. */
            visited[from_row+1][from_column] = -1;
        }
    }

    /* Neighbour to the right: (from_row, from_column + 1). Same steps as above. */
    if (from_column+1 < cols) {
        if (visited[from_row][from_column+1] != 0) {
           // do nothing
        } else if (*(game_matrix + from_row * cols + (from_column + 1))) {
            visited[from_row][from_column+1] = 1;
            if (search(game_matrix, rows, cols, from_row, from_column+1, to_row, to_column)) {
                return true;
            }
        } else {
            visited[from_row][from_column+1] = -1;
        }
    }

    /* Neighbour above: (from_row - 1, from_column). Same steps as above. */
    if (0 <= from_row - 1) {
        if (visited[from_row-1][from_column] != 0) {
           // do nothing
        } else if (*(game_matrix + (from_row - 1) * cols + from_column)) {
            visited[from_row-1][from_column] = 1;
            if (search(game_matrix, rows, cols, from_row-1, from_column, to_row, to_column)) {
                return true;
            }
        } else {
            visited[from_row-1][from_column] = -1;
        }
    }

    /* Neighbour to the left: (from_row, from_column - 1). Same steps as above. */
    if (0 <= from_column - 1) {
        if (visited[from_row][from_column-1] != 0) {
           // do nothing
        } else if (*(game_matrix + from_row * cols + (from_column - 1))) {
            visited[from_row][from_column-1] = 1;
            if (search(game_matrix, rows, cols, from_row, from_column-1, to_row, to_column)) {
                return true;
            }
        } else {
            visited[from_row][from_column-1] = -1;
        }
    }

    /* No branch from this cell reached the target. When the first call
       returns here, every cell in C(a,b) has had its neighbours checked and
       none was the target, so no path exists. */
    return false;
}

/* Clear the marks from any earlier call, then search from the start. */
bool can_travel_to(bool *game_matrix, int rows, int cols, int from_row,
                   int from_column, int to_row, int to_column)
{
    init_visited(rows, cols);
    return search(game_matrix, rows, cols, from_row, from_column, to_row, to_column);
}

/* Compile with -DRunTests to leave out this demo, for example when a test
   harness supplies its own main. */
#ifndef RunTests
int main()
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

    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 2, 2)); // true, Valid move

    /* NOTE: this program prints 1 here, because the boat reaches (3, 4)
       through water by (3,2) -> (2,2) -> (2,3) -> (2,4) -> (3,4), as the
       trace shows. The expected false fits a version of the problem in which
       the boat must arrive in a single move, which would have to cross the
       land cell (3, 3). */
    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 3, 4)); // false, Can't travel through land

    /* NOTE: row 6 lies outside the grid; see the NOTE in search on reading
       the target cell. */
    printf("%d\n", can_travel_to((bool*) game_matrix, 6, 6, 3, 2, 6, 2)); // false, Out of bounds
}
#endif
