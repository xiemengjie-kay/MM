#include "Maze.hpp"
#include <math.h>
#include "API.hpp"
#include <queue>
#include <iostream>
#include <string>


Maze::Maze() 
    : mousePos{0, 0}
    , mouseDir(NORTH)
    , goalPos(NULL)
    , numGoals(0)
    , back(false)
    , calculating(false) {
    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 16; j++){
            walls[i][j] = 0;
            visited[i][j] = false;
        }
    }
}


void Maze::setGoalPos(Coord goals[], int numGoals) {
    this->numGoals = numGoals;
    goalPos = goals;
    for (int i = 0; i < numGoals; i++) {
        flood[goals[i].x][goals[i].y] = 0;
    }
}


void Maze::initializeFlood() {
    for (int r=0; r <= 15; r++) {
        for (int c=0; c <= 15; c++) {
            bool isGoal = false;
            for (int i = 0; i < numGoals; i++) {
                if (r == goalPos[i].x && c == goalPos[i].y) isGoal = true;
            }
            if (!isGoal) {
                int min_dis = abs(r-goalPos[0].x)+abs(c-goalPos[0].y);
                for (int i = 1; i < numGoals; i++) {
                    int dis = abs(r-goalPos[i].x) + abs(c-goalPos[i].y);
                    if (dis < min_dis) min_dis = dis;
                }
                flood[r][c] = min_dis;
            }
        }
    }

    for (int i = 0; i < 16; i++){
        for (int j = 0; j < 16; j++){
            API::setText(i, j, std::to_string(flood[i][j]));
        }
    }
}


void Maze::setInitialPos(int x, int y) {
    mousePos.x = x;
    mousePos.y = y;
    visited[x][y] = true;
}


bool Maze::search() {
    API::setColor(mousePos.x, mousePos.y, 'G');
    floodFill();

    setBack(true);
    Coord newGoalPos[] = {{0, 0}};
    setGoalPos(newGoalPos, 1);
    initializeFlood(); 
    floodFill();
    setBack(false);

    return true;
}


void Maze::floodFill() {
    std::queue<Coord> q;
    while (!reachGoalPos()) {
        if (!calculating) {
            detectWalls(mousePos.x, mousePos.y, API::wallFront(), API::wallRight(), API::wallLeft());
        }
        q.push(mousePos);
        if (calculating) {
            // Implement BFS to recalculate flood values for the whole maze
            Coord mousePosAccessNeighbors[4];
            memset(mousePosAccessNeighbors, -1, sizeof(mousePosAccessNeighbors));
            accessibleNeighbors(mousePos.x, mousePos.y, mousePosAccessNeighbors);
            for (auto neighbor : mousePosAccessNeighbors) {
                if (neighbor.x != -1 && neighbor.y != -1 && !isGoals(neighbor.x, neighbor.y)) q.push(neighbor); 
                else break;
            }
        }
        while (!q.empty()) {
            Coord front = q.front();
            q.pop();

            Coord accessNeighbors[4];
            memset(accessNeighbors, -1, sizeof(accessNeighbors));
            accessibleNeighbors(front.x, front.y, accessNeighbors);

            int minValue = minFloodValue(accessNeighbors);
            if (flood[front.x][front.y] != minValue+1) {
                flood[front.x][front.y] = minValue+1;
                API::setText(front.x, front.y, std::to_string(flood[front.x][front.y]));
                Coord allNeighbors_[4];
                memset(allNeighbors_, -1, sizeof(allNeighbors_));
                allNeighbors(front.x, front.y, allNeighbors_);
                for (auto neighbor : allNeighbors_) {
                    if (neighbor.x != -1 && neighbor.y != -1 && !isGoals(neighbor.x, neighbor.y)) {
                        q.push(neighbor);
                    }
                }
            }
        }
        Coord accessNeighbors[4];
        memset(accessNeighbors, -1, sizeof(accessNeighbors));
        accessibleNeighbors(mousePos.x, mousePos.y, accessNeighbors);
        Coord nextMove_ = nextMove(accessNeighbors);
        move(nextMove_.x, nextMove_.y);
        mousePos = nextMove_;
        API::setColor(mousePos.x, mousePos.y, 'G');
    }
}


bool Maze::reachGoalPos() {
    for (int i = 0; i < numGoals; i++) {
        if (mousePos.x == goalPos[i].x && mousePos.y == goalPos[i].y) {
            return true;
        }
    }
    return false;
}


void Maze::detectWalls(int x, int y, bool wallFront, bool wallRight, bool wallLeft) {
    if (wallFront) {
        switch (mouseDir) {
            case NORTH:
                API::setWall(x, y, 'n');
                walls[x][y] |= 0b0010;
                if (y+1 <= 15) walls[x][y+1] |= 0b1000;
                break;
            case EAST:
                API::setWall(x, y, 'e');
                walls[x][y] |= 0b0100;
                if (x+1 <= 15) walls[x+1][y] |= 0b0001;
                break;
            case WEST:
                API::setWall(x, y, 'w');
                walls[x][y] |= 0b0001;
                if (x-1 >= 0) walls[x-1][y] |= 0b0100;
                break;
            case SOUTH:
                API::setWall(x, y, 's');
                walls[x][y] |= 0b1000;
                if (y-1 >= 0) walls[x][y-1] |= 0b0010;
                break;
            default:
                break;
        }
    }
    if (wallRight) {
        switch (mouseDir) {
            case NORTH:
                API::setWall(x, y, 'e');
                walls[x][y] |= 0b0100;
                if (x+1 <= 15) walls[x+1][y] |= 0b0001;
                break;
            case EAST:
                API::setWall(x, y, 's');
                walls[x][y] |= 0b1000;
                if (y-1 >= 0) walls[x][y-1] |= 0b0010;
                break;
            case WEST:
                API::setWall(x, y, 'n');
                walls[x][y] |= 0b0010;
                if (y+1 <= 15) walls[x][y+1] |= 0b1000;
                break;
            case SOUTH:
                API::setWall(x, y, 'w');
                walls[x][y] |= 0b0001;
                if (x-1 >= 0) walls[x-1][y] |= 0b0100;
                break;
            default:
                break;
        }
    }
    if (wallLeft) {
        switch (mouseDir) {
            case NORTH:
                API::setWall(x, y, 'w');
                walls[x][y] |= 0b0001;
                if (x-1 >= 0) walls[x-1][y] |= 0b0100;
                break;
            case EAST:
                API::setWall(x, y, 'n');
                walls[x][y] |= 0b0010;
                if (y+1 <= 15) walls[x][y+1] |= 0b1000;
                break;
            case WEST:
                API::setWall(x, y, 's');
                walls[x][y] |= 0b1000;
                if (y-1 >= 0) walls[x][y-1] |= 0b0010;
                break;
            case SOUTH:
                API::setWall(x, y, 'e');
                walls[x][y] |= 0b0100;
                if (x+1 <= 15) walls[x+1][y] |= 0b0001;
                break;
            default:
                break;
        }
    }
}


void Maze::accessibleNeighbors(int x, int y, Coord* neighbors) {
    int idx = 0;
    if ((walls[x][y] & 0b0001) == 0) {
        if (x-1 >= 0) {
            neighbors[idx].x = x-1;
            neighbors[idx].y = y;
            idx++;
        }
    }
    if ((walls[x][y] & 0b0010) == 0) {
        if (y+1 <= 15) {
            neighbors[idx].x = x;
            neighbors[idx].y = y+1;
            idx++;
        }
    }
    if ((walls[x][y] & 0b0100) == 0) {
        if (x+1 <= 15) {
            neighbors[idx].x = x+1;
            neighbors[idx].y = y;
            idx++;
        }
    }
    if ((walls[x][y] & 0b1000) == 0) {
        if (y-1 >= 0) {
            neighbors[idx].x = x;
            neighbors[idx].y = y-1;
        }
    }
}


void Maze::allNeighbors(int x, int y, Coord* neighbors) {
    int idx = 0;
    if (x-1 >= 0) {
        neighbors[idx].x = x-1;
        neighbors[idx].y = y;
        idx++;
    }
    if (y+1 <= 15) {
        neighbors[idx].x = x;
        neighbors[idx].y = y+1;
        idx++;
    }
    if (x+1 <= 15) {
        neighbors[idx].x = x+1;
        neighbors[idx].y = y;
        idx++;
    }
    if (y-1 >= 0) {
        neighbors[idx].x = x;
        neighbors[idx].y = y-1;
    }
}


int Maze::minFloodValue(Coord* accessNeighbors) {
    int ans = flood[accessNeighbors[0].x][accessNeighbors[0].y];
    int idx = 1;
    while (idx <= 3 && accessNeighbors[idx].x != -1 && accessNeighbors[idx].y != -1) {
        if (flood[accessNeighbors[idx].x][accessNeighbors[idx].y] < ans) {
            ans = flood[accessNeighbors[idx].x][accessNeighbors[idx].y];
        }
        idx++;
    }
    return ans;
}


bool Maze::isGoals(int x, int y) {
    for (int i=0; i < numGoals; i++) {
        if (x == goalPos[i].x && y == goalPos[i].y) {
            return true;
        }
    }
    return false;
}


Coord Maze::nextMove(Coord* accessNeighbors) {
    Coord next = {-1, -1};
    if (back) {
        //  1st choice: unvisited cells with min flood value
        //  2nd choice: unvisited cells with higher flood value
        //  3rd choice: visited cells with min flood value
        for (int i = 0; i < 4; i++) {
            if (accessNeighbors[i].x != -1 && accessNeighbors[i].y != -1) {
                if (next.x == -1 && next.y == -1) {
                    next = accessNeighbors[0];
                } else {
                    if (!visited[accessNeighbors[i].x][accessNeighbors[i].y]) {
                        if (!visited[next.x][next.y]) {
                            if (flood[accessNeighbors[i].x][accessNeighbors[i].y] < flood[next.x][next.y]) {
                                next = accessNeighbors[i];
                            }
                        } else {
                            next = accessNeighbors[i];
                        }
                    } else {
                        if (!visited[next.x][next.y]) {
                            continue;
                        } else {
                            if (flood[accessNeighbors[i].x][accessNeighbors[i].y] < flood[next.x][next.y]) {
                                next = accessNeighbors[i];
                            }
                        }
                    }
                }
            }
        }
    } else {
        for (int i = 0; i < 4; i++) {
            if (accessNeighbors[i].x != -1 && accessNeighbors[i].y != -1) { 
                if (next.x == -1 && next.y == -1) {
                    next = accessNeighbors[i];
                } else {
                    if (flood[accessNeighbors[i].x][accessNeighbors[i].y] < flood[next.x][next.y]) {
                        next = accessNeighbors[i];
                    }
                }
            }
        }
    }
    return next;
}


void Maze::move(int x, int y) {
    visited[x][y] = true;
    if (mouseDir==NORTH){
        if (x - mousePos.x == 1) {
            if (!calculating) {
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = EAST;
        } else if (x - mousePos.x == -1) {
            if (!calculating) {
                API::turnLeft();
                API::moveForward(1);
            }
            mouseDir = WEST;
        } else if (y - mousePos.y == 1) {
            if (!calculating) {
                API::moveForward(1);
            }
        } else if (y - mousePos.y == -1) {
            if (!calculating) {
                API::turnRight();
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = SOUTH;
        }
    } else if (mouseDir==EAST){
        if (x - mousePos.x == 1) {
            if (!calculating) { API::moveForward(1); }
        } else if (x - mousePos.x == -1) {
            if (!calculating) {
                API::turnRight();
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = WEST;
        } else if (y - mousePos.y == 1) {
            if (!calculating) {
                API::turnLeft();
                API::moveForward(1);
            }
            mouseDir = NORTH;
        } else if (y - mousePos.y == -1) {
            if (!calculating) {
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = SOUTH;
        }
    } else if (mouseDir==SOUTH){
        if (x - mousePos.x == 1) {
            if (!calculating) {
                API::turnLeft();
                API::moveForward(1);
            }
            mouseDir = EAST;
        } else if (x - mousePos.x == -1) {
            if (!calculating) {
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = WEST;
        } else if (y - mousePos.y == 1) {
            if (!calculating) {
                API::turnRight();
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = NORTH;
        } else if (y - mousePos.y == -1) {
            if (!calculating) { API::moveForward(1); }
        }
    } else {
        // mouseDir == WEST
        if (x - mousePos.x == 1) {
            if (!calculating) {
                API::turnRight();
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = EAST;
        } else if (x - mousePos.x == -1) {
            if (!calculating) { API::moveForward(1); }
        } else if (y - mousePos.y == 1) {
            if (!calculating) {
                API::turnRight();
                API::moveForward(1);
            }
            mouseDir = NORTH;
        } else if (y - mousePos.y == -1) {
            if (!calculating) {
                API::turnLeft();
                API::moveForward(1);
            }
            mouseDir = SOUTH;
        }
    } 
}


void Maze::setBack(bool value) {
    back = value;
}


void Maze::setCalculating(bool value) {
    calculating = value;
}

void Maze::recalculateFloodValues(Coord* goals, int numGoals, int x, int y) {
    setCalculating(true);
    setGoalPos(goals, numGoals);
    initializeFlood();
    setInitialPos(x, y);
    floodFill();
    setCalculating(false);
}
