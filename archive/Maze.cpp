#include "Maze.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <stack>
#include <chrono>
#include <thread>

Maze::Maze(int width, int height)
{
	x_size = width;
	y_size = height;
	grid = new char* [x_size];
	visited = new bool* [x_size];
	for (int x = 0; x < x_size; x++) {
		grid[x] = new char[y_size];
		visited[x] = new bool[y_size];
		for (int y = 0; y < y_size; y++) {
			grid[x][y] = 'X';
			visited[x][y] = false;
		}
	}
}

Maze::Maze(std::string directory)
{
	//Only a directory or empty data entered, set default to maze.txt in exe directory
	directory = directory.back() == '\\' ? directory + "maze.txt" : directory;
	//No .txt on end? make it a .txt
	directory = directory.substr(directory.size() - 4) == ".txt" ? directory : directory + ".txt";
	std::cout << "Opening file in directory: " << directory << "\n";
	std::ifstream file(directory);
	std::string line;
	try {
		std::getline(file, line);
		x_size = std::stoi(line);
		std::getline(file, line);
		y_size = std::stoi(line);
		grid = new char* [x_size];
		visited = new bool* [x_size];
		for (int x = 0; x < x_size; x++) {
			grid[x] = new char[y_size];
			visited[x] = new bool[y_size];
		}
		int pos = 0;
		while (std::getline(file, line)) {
			for (int i = 0; i < line.size(); i++) {
				grid[i][pos] = (char)line[i];
			}
			pos++;
		}
		file.close();
		generated = true;
		centre.x = x_size / 2;
		centre.y = y_size / 2;
	}
	catch (const std::exception& e)
	{
		std::cout << "Error reading file. Creating default maze.\nMake sure mazes are saved in the format:\n-Width\n-Height\n-{\n--Data\n-}\n";
		x_size = 10;
		y_size = 10;
		grid = new char* [x_size];
		visited = new bool* [x_size];
		for (int x = 0; x < x_size; x++) {
			grid[x] = new char[y_size];
			visited[x] = new bool[y_size];
			for (int y = 0; y < y_size; y++) {
				grid[x][y] = 'X';
				visited[x][y] = false;
			}
		}
	}
}

void Maze::Generate()
{
	generated = true;
	trace.emplace(CoordToVal(x_size/2, y_size/2));
	while (!(trace.empty())) {
		int x_curr = XFromVal(trace.front());
		int y_curr = YFromVal(trace.front());
		grid[x_curr][y_curr] = ' ';
		visited[x_curr][y_curr] = true;

		int walls = rand() % 15;
		grid[x_curr][y_curr - 1] = ((walls >> 0) & 1 && (y_curr - 2 < 0 || !visited[x_curr][y_curr - 1]))			? ' ' : 'X';
		grid[x_curr + 1][y_curr] = ((walls >> 1) & 1 && (x_curr + 2 > x_size - 1 || !visited[x_curr + 1][y_curr]))	? ' ' : 'X';
		grid[x_curr][y_curr + 1] = ((walls >> 2) & 1 && (y_curr + 2 > y_size - 1 || !visited[x_curr][y_curr + 1]))	? ' ' : 'X';
		grid[x_curr - 1][y_curr] = ((walls >> 3) & 1 && (x_curr - 2 < 0 || !visited[x_curr - 1][y_curr]))			? ' ' : 'X';

		trace.pop();
		//Open Paths first
		if (y_curr - 2 > 0 && !visited[x_curr][y_curr - 2] && ((walls >> 0) & 1))			track.emplace(CoordToVal(x_curr, y_curr - 2));
		if (x_curr + 2 < x_size - 1 && !visited[x_curr + 2][y_curr] && ((walls >> 1) & 1))	track.emplace(CoordToVal(x_curr + 2, y_curr));
		if (y_curr + 2 < x_size - 1 && !visited[x_curr][y_curr + 2] && ((walls >> 2) & 1))	track.emplace(CoordToVal(x_curr, y_curr + 2));
		if (x_curr - 2 > 0 && !visited[x_curr - 2][y_curr] && ((walls >> 3) & 1))			track.emplace(CoordToVal(x_curr - 2, y_curr));

		//Then closed paths
		if (y_curr - 2 > 0 && !visited[x_curr][y_curr - 2] && !((walls >> 0) & 1))			track.emplace(CoordToVal(x_curr, y_curr - 2));
		if (x_curr + 2 < x_size - 1 && !visited[x_curr + 2][y_curr] && !((walls >> 1) & 1))	track.emplace(CoordToVal(x_curr + 2, y_curr));
		if (y_curr + 2 < x_size - 1 && !visited[x_curr][y_curr + 2] && !((walls >> 2) & 1))	track.emplace(CoordToVal(x_curr, y_curr + 2));
		if (x_curr - 2 > 0 && !visited[x_curr - 2][y_curr] && !((walls >> 3) & 1))			track.emplace(CoordToVal(x_curr - 2, y_curr));
		
		//Reorder the queue
		OrderQueue();

	}
	//Generate the central 3x3 room
	GenerateRoom();
	//Wall off the exterior
	GenerateWalls();
}

void Maze::OrderQueue() {
	while (!trace.empty()) {
		//Place and remove the first item from the trace queue into the track queue
		track.emplace(trace.front());
		trace.pop();
	}
	//Swap the queues around so that the track queue is empty and the trace queue has an ordered set of nodes
	trace.swap(track);
}

void Maze::GenerateRoom() {
	//Lable the centre as the flag 'F'
	grid[x_size / 2][y_size / 2] = 'F';
	//Define the centre node point (for A*)
	centre.x = x_size / 2;
	centre.y = y_size / 2;
	//Create the centre room tile by tile
	grid[(x_size / 2) - 1][(y_size / 2) - 1] = ' ';
	grid[(x_size / 2)][(y_size / 2) - 1] = ' ';
	grid[(x_size / 2) + 1][(y_size / 2) - 1] = ' ';
	grid[(x_size / 2) - 1][(y_size / 2)] = ' ';
	grid[(x_size / 2) + 1][(y_size / 2)] = ' ';
	grid[(x_size / 2) - 1][(y_size / 2) + 1] = ' ';
	grid[(x_size / 2)][(y_size / 2) + 1] = ' ';
	grid[(x_size / 2) + 1][(y_size / 2) + 1] = ' ';
}

void Maze::GenerateWalls() {
	//Loop down and across each of the 4 edges to set them as walls
	for (int i = 0; i < x_size - 1; i++) {
		grid[i][0] = 'X';
	}
	for (int i = 0; i < x_size - 1; i++) {
		grid[i][y_size - 1] = 'X';
	}
	for (int i = 0; i < y_size - 1; i++) {
		grid[0][i] = 'X';
	}
	for (int i = 0; i < y_size - 1; i++) {
		grid[x_size - 1][i] = 'X';
	}
}

void Maze::AddExit()
{
	//Weight determines if it will be on row/column 0 or max
	int x_weight = (rand() & 1) * (x_size - 1);
	int y_weight = (rand() & 1) * (y_size - 1);
	//Pos determines the actual position on an axis
	int x_pos = rand() % (x_size - 3) + 1;
	int y_pos = (rand() % (y_size - 3) + 1);
	//Keeps exits on odd coords to follow patttern of the maze
	x_pos += !(x_pos & 1); 
	y_pos += !(y_pos & 1);
	//Exit is on the X axis when flip is 1
	int flip = rand() & 1;

	//Generates edge coordinates using the flip bit and the values generated
	int x_real = (x_weight * flip) + (x_pos * !flip);
	int y_real = (y_weight * !flip) + (y_pos * flip);
	//Sets the selected coord to an exit
	grid[x_real][y_real] = 'E';

	//Creates a copy of the grid for the tunnel boring
	char** copy = new char* [x_size];
	copy = new char* [x_size];
	for (int x = 0; x < x_size; x++) {
		copy[x] = new char[y_size];
		for (int y = 0; y < y_size; y++) {
			copy[x][y] = grid[x][y];
		}
	}

	//Check if solution already available
	bool solved = SolvableFrom(x_real, y_real);
	//Default bore on x axis
	bool bore_axis = (y_real == 0 || y_real == y_size - 1) ? 1 : 0;

	if (!solved) {
		int main_range = (((!bore_axis) * std::abs(centre.x - x_real)) + ((bore_axis) * std::abs(centre.y - y_real)));
		for (int main = 1; main <= main_range; main++) {
			//Reset tunneling by overwriting the bored grid with the backup in copy
			for (int x = 0; x < x_size; x++) {
				for (int y = 0; y < y_size; y++) {
					grid[x][y] = copy[x][y];
				}
			}
			//Get bore direction independant of current axis. Up down left or right. Up/left = -1, Down/right = +1
			int main_bore_direction = ((x_real - centre.x < 0 ? 1 : -1) * (!bore_axis)) + ((y_real - centre.y < 0 ? 1 : -1) * (bore_axis));

			for (int main_bore = 1; main_bore <= main; main_bore++) { //Distance to core x. Offset by 1
				//x_next will be unchanged if x is not the main bore axis
				int x_next = x_real + ((!bore_axis) * (main_bore * main_bore_direction));
				int y_next = y_real + ((bore_axis) * (main_bore * main_bore_direction));
				grid[x_next][y_next] = ' '; //Dig out the main tunnel first using the direction to core
			}
			//Always check if there is a solution
			solved = SolvableFrom(x_real, y_real);
			if (solved) break; //Stop boring if a solution is found
			if ((main + ((!bore_axis) * x_real) + ((bore_axis) * y_real)) & 1) { //Start vertical branches when aligned with the grid
				//Set the secondary bore range to the max of the distance between the main bore and the edge / core
				int secondary_range = (((bore_axis) * std::abs(centre.x - x_real)) + ((!bore_axis)*std::abs(centre.y - y_real)));
				int secondary_bore_direction = ((x_real - centre.x < 0 ? 1 : -1) * (bore_axis)) + ((y_real - centre.y < 0 ? 1 : -1) * (!bore_axis));
				for (int secondary = 1; secondary <= secondary_range && secondary + ((bore_axis * x_real) + (!bore_axis * y_real)) < ((bore_axis * x_size) + (!bore_axis * y_size)); secondary++) {
					for (int secondary_bore = 1; secondary_bore <= secondary; secondary_bore++) {
						//Next is the main bore tip position + secondary bore
						int x_next = (((main * main_bore_direction) + x_real) * (!bore_axis)) + ((x_real + secondary_bore * secondary_bore_direction) * (bore_axis));
						int y_next = (((main * main_bore_direction) + y_real) * (bore_axis)) + ((y_real + secondary_bore * secondary_bore_direction) * (!bore_axis));
						grid[x_next][y_next] = ' '; //Dig out the secondary tunnels
					}
					//Always check if there is a solution
					solved = SolvableFrom(x_real, y_real);
					if (solved) break; //Stop boring if a solution is found
				}
			}
		}
	}
	//Finished with the copy. Delete it
	Free2dArray(copy);
}

int Maze::CountExits()
{
	int count = 0;
	for (int x = 0; x < x_size; x++) {
		for (int y = 0; y < y_size; y++) {
			count += (grid[x][y] == 'E' ? 1 : 0); //Tally up every instance of 'E' in the grid
		}
	}
	return count;
}

void Maze::PlayerPreperation() {
	for (int x = 0; x < x_size; x++) {
		for (int y = 0; y < y_size; y++) {
			if (grid[x][y] == 'E') { //Finds any exits and creates a player at the position.
				//Players as pointers to allow for data manipulation later.
				Player* p = new Player(x, y);
				Node n(x, y);
				p->path = aStar(n, centre);
				//Uses the 1D version of the 2D coords to make a unique identifier for each player for efficiency.
				p->id = CoordToVal(x, y); 
				players.emplace_back(p);
			}
		}
	}
}

int Maze::Play(bool animated)
{
	//Maze not generated and no exits to use;
	if (!generated || CountExits() < 1) return -1;

	//Prepare the players
	PlayerPreperation();

	//Game Loops until all remaining are stuck or all reached the end
	while (GameOver() == 0) {
		//Iterate through each player
		for (Player* p : players) {
			if (!p->finished) {
				//Finds the target location for the player to move to.
				int x_next = p->path.at(p->timeline).x;
				int y_next = p->path.at(p->timeline).y;
				//If the target is the finish point, Ignore collisions and go.
				if (p->path.back().x == x_next && p->path.back().y == y_next) {
					p->finished = true;
					p->moved_previously = true;
					p->x = x_next;
					p->y = y_next;
					continue;
				}
				p->moved_previously = true;
				//Check to see if there is a player in the spot for the next position
				for (Player* e : players) {
					if (*p == *e) continue;
					p->moved_previously &= !(e->x == x_next && e->y == y_next);
				}
				//Updates the player based on if it can move or not.
				p->x = (x_next * p->moved_previously) + (p->x * !p->moved_previously);
				p->y = (y_next * p->moved_previously) + (p->y * !p->moved_previously);
				p->timeline += p->moved_previously;
			}
		}
		//If the user decides to animate the drawing (Clear frame -> draw frame) it can do.
		system(animated ? "cls" : NULL); // Only clears console if animating
		Draw(); //Always draw new frame
		if (!animated) std::cout << '\n';
		std::this_thread::sleep_for(std::chrono::milliseconds(animated ? 200 : 0));
	}
	//Clear the players vector (for when running the game again / init)
	players.clear();
	//Clear the screen
	system(animated ? "cls" : NULL);
	//Redraw without players on Flag (if animated);
	Draw();
	//Return game state code
	return GameOver();
}

int Maze::GameOver()
{
	bool all_finished = true;
	for (Player* p : players) {
		//Exit code 1: all players are finished. True until proven false
		all_finished = p->finished & all_finished;
	}
	bool all_stuck = true;
	for (Player* p : players) {
		//Exit code 2: all remaining players are stuck. True until proven false
		all_stuck = !p->finished ? (!p->moved_previously) & all_stuck : all_stuck;
	}
	return all_finished ? 1 : all_stuck ? 2 : 0;
}

void Maze::SetSeed(int seed)
{
	//Set srand to a determined seed. If seed is 0 (default) then it uses time for a random maze each time.
	srand(seed == 0 ? time(0) : seed);
}

void Maze::Draw()
{
	//Iterate through each grid cell
	for (int y = 0; y < y_size; y++) {
		for (int x = 0; x < x_size; x++) {
			//Check to see if a player is on the cell.
			bool player_here = false;
			for (Player* p : players) {
				if (p->x == x && p->y == y) {
					player_here = true;
				}
			}
			//Draw a 'P' on the cell where there is a player. Don't overwrite data.
			std::cout << (player_here ? 'P' : grid[x][y]) << " ";
		}
		std::cout << '\n';
	}
}

void Maze::Solve()
{
	for (int x = 0; x < x_size; x++) {
		for (int y = 0; y < y_size; y++) {
			if (grid[x][y] == 'E') {
				//Set the start point to wherever there is an exit
				Node start(x, y);
				//Iterate through the path list to the centre from the chosen exit
				for (Node node : aStar(start, centre)) {
					//Replace all open pathways with an 'o'
					grid[node.x][node.y] = grid[node.x][node.y] == ' ' ? 'o' : grid[node.x][node.y];
				}
			}
		}
	}
}

bool Maze::SolvableFrom(int x, int y)
{
	//Check to see if a maze is solvable from a determined location
	Node exit(x, y);
	return aStar(exit, centre).size() > 0 ? true : false;
}

void Maze::Save(std::string directory)
{
	//Only a directory or empty data entered, set default to maze.txt in exe directory
	directory = directory.back() == '\\' || directory.size() < 4 ? directory + "maze.txt" : directory;
	//No .txt on end? make it a .txt
	directory = directory.substr(directory.size() - 5) == ".txt" ? directory : directory + ".txt";
	//Open the file in the directory of choice.
	std::ofstream file(directory);
	//Iterate through rows and colums and save them to the file.
	//Preface files with grid size for validation
	file << x_size << '\n' << y_size << '\n';
	for (int y = 0; y < y_size; y++) {
		for (int x = 0; x < x_size; x++) {
			file << grid[x][y];
		}
		file << '\n';
	}
	//Close the file when done.
	file.close();
	std::cout << "File saved to: " << directory << "\n";
}

template <class T>
void Maze::Free2dArray(T** poi) {
	//Delete all the contents of a 2D array for any type
	for (int i = 0; i < x_size; i++) {
		delete poi[i];
	}
	delete[] poi;
}

std::vector<Node> Maze::aStar(Node start, Node dest)
{
	//Create a closed list and a map of nodes (Struct in .h)
	bool** closed_list;
	Node** all_map;
	closed_list = new bool* [x_size];
	all_map = new Node * [x_size];
	for (int x = 0; x < x_size; x++) {
		closed_list[x] = new bool[y_size];
		all_map[x] = new Node[y_size];
		for (int y = 0; y < y_size; y++) {
			all_map[x][y] = Node(x, y);
			closed_list[x][y] = false;
		}
	}

	//Initialize the start point as the passed parameter
	int x = start.x;
	int y = start.y;
	all_map[x][y].f_cost = 0.0;
	all_map[x][y].g_cost = 0.0;
	all_map[x][y].h_cost = 0.0;
	all_map[x][y].x_parent = x;
	all_map[x][y].y_parent = y;

	std::queue<Node> open_list;
	open_list.emplace(all_map[x][y]);

	while (!open_list.empty() && open_list.size() < ((size_t)x_size * (size_t)y_size)) {
		Node node;
		node = open_list.front();
		open_list.pop();

		x = node.x;
		y = node.y;
		closed_list[x][y] = true;
		x = node.x;
		y = node.y;
		closed_list[x][y] = true;

		//For each neighbour starting from North-West to South-East
		for (int x_new = -1; x_new < 2; x_new++) {
			for (int y_new = -1; y_new < 2; y_new++) {
				//Don't allow the searching of diagonals
				if ((x_new == -1 && y_new == -1) || (x_new == -1 && y_new == 1) || (x_new == 1 && y_new == -1) || (x_new == 1 && y_new == 1)) {
					continue;
				}
				//Temp values for f, g, and h
				float g_new, h_new, f_new;
				//Check if the neighbour is a valid space (I.E.: not a wall)
				if (IsValid(x + x_new, y + y_new)) {
					//Check if the cell checked is the destination
					if (isDestination(x + x_new, y + y_new, dest))
					{
						//Destination found - make path
						all_map[x + x_new][y + y_new].x_parent = x;
						all_map[x + x_new][y + y_new].y_parent = y;

						//Free memory on completion
						std::vector<Node> path = makePath(all_map, dest);
						Free2dArray(closed_list);
						Free2dArray(all_map);
						return path;
					}
					//Check if the neighbour has not been looked at yet
					else if (closed_list[x + x_new][y + y_new] == false)
					{
						g_new = node.g_cost + 1.0f;
						h_new = calculateH(x + x_new, y + y_new, dest);
						f_new = g_new + h_new;
						// Check if this path is better than the one already present
						if (all_map[x + x_new][y + y_new].f_cost == FLT_MAX || all_map[x + x_new][y + y_new].f_cost > f_new)
						{
							// Update the details of this neighbour node
							all_map[x + x_new][y + y_new].f_cost = f_new;
							all_map[x + x_new][y + y_new].g_cost = g_new;
							all_map[x + x_new][y + y_new].h_cost = h_new;
							all_map[x + x_new][y + y_new].x_parent = x;
							all_map[x + x_new][y + y_new].y_parent = y;
							open_list.emplace(all_map[x + x_new][y + y_new]);
						}
					}
				}
			}
		}
	}
	Free2dArray(closed_list);
	Free2dArray(all_map);
	std::vector<Node> empty;
	return empty;
}

std::vector<Node> Maze::makePath(Node** map, Node dest) {
	//Use backtracking to find the path from the destination node
	int x = dest.x;
	int y = dest.y;
	std::stack<Node> path;
	std::vector<Node> usable_path;

	//Since the starting node parents are -1, -1; go until this is found
	while (!(map[x][y].x_parent == x && map[x][y].y_parent == y) && map[x][y].x != -1 && map[x][y].y != -1)
	{
		path.push(map[x][y]);
		int tempX = map[x][y].x_parent;
		int tempY = map[x][y].y_parent;
		x = tempX;
		y = tempY;
	}
	path.push(map[x][y]);

	while (!path.empty()) {
		//Convert from a stack to a vector
		//Also reverts the direction of the path so that the start node is first
		Node top = path.top();
		path.pop();
		usable_path.emplace_back(top);
	}
	return usable_path;
}