#ifndef MAZE_HPP
#define MAZE_HPP

struct Coord {
    int x;
    int y;
};

enum Direction {
	NORTH = 0,
    EAST = 1,
	SOUTH = 2,
    WEST = 3
};

class Maze {
public:
    Maze();

    // setGoalPos() sets the goal postion(s) for the mouse.
    void setGoalPos(Coord goals[], int numGoals);

    // initializeFlood() initializes the flood values for the maze.
    void initializeFlood();

    // setInitialPos() sets the initial position of the mouse.
    void setInitialPos(int x, int y);

    // search() starts exploring the maze using the floodfill algorithm.
    // The mouse is moving.
    bool search();

    // setBack() sets the value of the attribute back.
    void setBack(bool value);

    // setCalculating() sets the value of the attribute calculating.
    void setCalculating(bool value);

    // recalculateFloodValues() calculates flood values based on the wall information
    // obtained so far. If the maze has not been fully explored, the calculated flood
    // values may not lead to the correct optimal shortest path.
    // The mouse is not moving.
    void recalculateFloodValues(Coord* goals, int numGoals, int x, int y);

private:
    // floodFill() executes the floodfill algorithm logic.
    void floodFill();

    // reachGoalPos() returns true if the mouse is at one of the goal positions. False, otherwise.
    bool reachGoalPos();

    // detectWalls() updates the 2D array walls[16][16] at index (x, y).
    void detectWalls(int x, int y, bool wallFront, bool wallRight, bool wallLeft);

    // accessibleNeighbors() adds the coordinations of accessible neighbors of position (x, y) 
    // to Coord* neighbors.
    void accessibleNeighbors(int x, int y, Coord* neighbors);

    // allNeighbors() adds the coordinations of all the neighbors of position (x, y) to
    // Coord* neighbors.
    void allNeighbors(int x, int y, Coord* neighbors);

    // minFloodValue() returns the minimum flood value within Coord* accessNeighbors.
    int minFloodValue(Coord* accessNeighbors);

    // isGoals() returns true if position (x, y) is a goal position. False, otherwise.
    bool isGoals(int x, int y);

    // nextMove() returns the next best move.
    Coord nextMove(Coord* accessNeighbors);

    // move() moves the mouse to position (x, y).
    void move(int x, int y);

private:
    Coord mousePos;        // the current position of the mouse
    Direction mouseDir;    // the current orientation of the mouse
    Coord* goalPos;        // goal positions
    int numGoals;          // the number of goal positions
    bool back;             // whether the mouse is running backward to its original position
    bool calculating;      // whether we are calculating flood values based on wall information obtained
    int flood[16][16];     // flood values
    int walls[16][16];     // wall information
    bool visited[16][16];  // if a cell is visited
};


#endif