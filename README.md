# programming-algorithms

| File | Question it answers |
| --- | --- |
| [graphs/can_travel_to.c](graphs/can_travel_to.c) | Can a boat get from one grid cell to another by stepping through water? A depth-first search that shares one visited array across all branches. |
| [search/count_numbers.cpp](search/count_numbers.cpp) | How many numbers in a sorted vector are less than a given value? A binary search for the point where the values reach it. |

Build and run a file:

```sh
mkdir -p build
gcc -std=c99 -Wall -Wextra graphs/can_travel_to.c -o build/can_travel_to
./build/can_travel_to
g++ -std=c++17 -Wall -Wextra search/count_numbers.cpp -o build/count_numbers
./build/count_numbers
```
