Coursework.exe is the main executable for the project.
No other files are required unless using external .txt files for custom imported mazes.
Coursework.cpp contains the main function.
Maze.h & Maze.cpp contain the rest of the code for generating, solving, and playing.

Code was written and compiled in VS19 and the solution / binaries can be found in the archive folder in this directory.
Coursework.sln is the solution to this project.

The tick sheet is also in this root directory

This solution covers both CW1 and CW2. 'Legacy' code for CW1 is still left in and can be used.
The only file difference being the centre cell being labeled 'F' instead of 'S'.

Maze must be generated before adding exits. Exits will always have a path to the centre cell.
Partial completion of the player A* will never occur due to this.

Adding an exit places one in a viable location, I.e.: follows the grid lines.
There are (width-2)+(height-2) possible exit locations because of this. 1 less if the axis is odd.

testMaze.txt has been added to load a 50x50 maze with 20 exits.
'.txt' is not necessary when opening a file. It will be appended automatically if missing.
Files will save as a default "maze.txt" if no file name is provided.
Files can be opened from any directory if stated.
Files can also be saved to any directory if stated.