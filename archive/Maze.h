#pragma once
#include <queue>
#include <fstream>
#include <string>
struct Node
{
	int y;
	int x;
	int x_parent;
	int y_parent;
	float g_cost;
	float h_cost;
	float f_cost;

	Node() {
		y = 0;
		x = 0;
		f_cost = FLT_MAX;
		g_cost = FLT_MAX;
		h_cost = FLT_MAX;
		x_parent = -1;
		y_parent = -1;
	};

	Node(int x_pos, int y_pos) {
		y = y_pos;
		x = x_pos;
		f_cost = FLT_MAX;
		g_cost = FLT_MAX;
		h_cost = FLT_MAX;
		x_parent = -1;
		y_parent = -1;
	};
};

struct Player 
{
	int x;
	int y;
	int id;
	std::vector<Node> path;
	int timeline;
	bool moved_previously;
	bool finished;
	Player() {
		x = -1;
		y = -1;
		id = -1;
		timeline = 0;
		moved_previously = true;
		finished = false;
	};

	Player(int x_pos, int y_pos) {
		x = x_pos;
		y = y_pos;
		id = -1;
		timeline = 0;
		moved_previously = true;
		finished = false;
	};
};

inline bool operator== (const Player& lhs, const Player& rhs)
{
	return lhs.id == rhs.id;
}

class Maze
{
public:
	Maze(int width = 10, int height = 10);

	Maze(std::string directory);

	~Maze() { Free2dArray(grid); Free2dArray(visited); };

	void Generate();
	void OrderQueue();
	void GenerateRoom();
	void GenerateWalls();
	bool Generated() {return generated;};

	void AddExit();
	int CountExits();

	void PlayerPreperation();
	int Play(bool animated = 0);
	int GameOver();

	void SetSeed(int seed);

	void Draw();

	void Solve();
	bool SolvableFrom(int x, int y);

	void Save(std::string directory);
private:

	int x_size, y_size = 10;
	char** grid;
	bool** visited;
	Node centre;

	std::queue<int> trace;
	std::queue<int> track;

	std::vector<Player*> players;

	bool generated = 0;
	
	//Maze Generation Functions
	int CoordToVal(int x, int y) {
		return x + (y * x_size);
	}

	int XFromVal(int val) {
		return val % x_size;
	}

	int YFromVal(int val) {
		return val / x_size;
	}

	//A* Functions
	bool IsValid(int x, int y) { //If our Node is an obstacle it is not valid
		return (y < y_size - 1 ? (y > 0 ? (x < x_size - 1 ? (x > 0 ? (grid[x][y] == 'X' ? false : true) : false) : false) : false) : false);
	}

	float calculateH(int x, int y, Node dest) {
		float H = (sqrtf(((float)x - (float)dest.x) * ((float)x - (float)dest.x) + ((float)y - (float)dest.y) * ((float)y - (float)dest.y)));
		return H;
	}

	bool isDestination(int x, int y, Node dest) {
		if (x == dest.x && y == dest.y) {
			return true;
		}
		return false;
	}

	std::vector<Node> aStar(Node start, Node dest);
	std::vector<Node> makePath(Node** map, Node dest);

	//Other Functions
	template<class T>
	void Free2dArray(T** poi);
};

