#include "Maze.hpp"

int main(int argc, char* argv[]) {

    Maze maze = Maze();
    Coord goalPos[] = {{7,7}, {7, 8}, {8,7}, {8,8}};
    maze.setGoalPos(goalPos, 4);
    maze.initializeFlood();
    maze.setInitialPos(0, 0);

    bool exploreButton = true;
    int count = 0;
    if (exploreButton && count == 0) {
        maze.search();
        count++;
        maze.recalculateFloodValues(goalPos, 4, 0, 0);
    }

    // while (exploreButton && 0 < count && count <= 1) {

    return 0;
}