# programming-algorithms

Algorithms in C, one folder per topic.

| File | Question it answers |
| --- | --- |
| [graphs/can_travel_to.c](graphs/can_travel_to.c) | Can a boat get from one grid cell to another by stepping through water? A depth-first search that shares one visited array across all branches. |

Build and run a file:

```sh
mkdir -p build
gcc -std=c99 -Wall -Wextra graphs/can_travel_to.c -o build/can_travel_to
./build/can_travel_to
```
