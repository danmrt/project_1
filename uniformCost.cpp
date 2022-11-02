#include <iostream>
#include <queue>
#include <string>
#include <vector>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
using namespace std;

class node {
    public:
        int h; //heuristic cost
        int depth = 0; //depth
        bool bruh = false;
        vector<int> parent;  //this is the game itself,
        vector<vector<int> > children; //this will hold our children, [ [holds 1 game], [holds child 2], [holds child 3] , [holds child 4] ]

        int getH() const { return h;}
        int getDepth() const {return depth;}
};
//used https://www.geeksforgeeks.org/implement-min-heap-using-stl/ to implement a custom pq sorting algo that sorts by class and with my conditions
//https://www.geeksforgeeks.org/stl-priority-queue-for-structure-or-class/
class myComparator {
    public:
    int operator() (const node& n1, const node& n2) {
        if (n1.getH() + n1.getDepth() == n2.getH() + n2.getDepth()) {
            return n1.getDepth() > n2.getDepth(); // if h_n + g_n are the same cost for both, order pq by lowest depth
        }
        return n1.getH() + n1.getDepth() > n2.getH() + n2.getDepth(); //orders the priority queue by lowest depth + h //geeksforgeeks
    }
};

const vector<int> goal{1,2,3,4,5,6,7,8,0}; //this is our goal state
const int rows = 3; //rows of our tile puzzle
const int columns = 3; //columns of our tile puzzle
int pq_size = 0; //keeps track of pq size
int maxSize_pq = -1; // keeps track of the maxed pq size has been

//functions
bool reachEnd(node);
void printGame(node);
void getGame(node &);
void select_and_init_algorithm(node &);
void uniform_cost_search(node&, int);
void expanding(node&, vector <vector<int> >&); //expands the children vector of vectors
void printChildren(node);
bool seenState(vector<int>, vector<vector <int> >); //this is our function where we iterate through all repeatedStates vector and try and match it with child vector of parent
int misplaced_tile(vector<int>); //takes in a vector of ints, representing the game returns h_n count
int manhattan_distance(vector<int>);
int findGoalrow(int);
int findGoalcol(int);

int main() {
    node userGame;
    
    getGame(userGame);
    select_and_init_algorithm(userGame);
    return 0;
}

void getGame(node & state) {
    int userValue;
    cout << "Welcome to the 8 tile puzzle solver.\n\n";

    cout << "Enter the first row of your puzzle, indicating the blank-tile with a 0\n\n";
    for (int i = 0; i < columns; i++) {
        cin >> userValue;
        state.parent.push_back(userValue);
    }

    cout << "Enter the second row of your puzzle, indicating the blank-tile with a 0\n\n";
    for (int i = 0; i < columns; i++) {
        cin >> userValue;
        state.parent.push_back(userValue);
    }

    cout << "Enter the third row of your puzzle, indicating the blank-tile with a 0\n\n";
    for (int i = 0; i < columns; i++) {
        cin >> userValue;
        state.parent.push_back(userValue);
    }

    cout << "Traceback begins \n\n";
    //cout << "\nPrinting out inputted game\n\n";
    //printGame(state);
}
bool reachEnd(node state) {

    if (state.parent == goal) { //if the main game itself vector is equal to the vector of the goal state.
        return true;
    }
    else {
        return false;
    }
}
void printGame(node state) { //prints out the node.parent game in a 3 x 3 matrix style

    cout << "[ ";
    for (int i = 0; i < 3; i++) {
        cout << state.parent.at(i) << " ";
    }
    cout << "]\n";

    cout << "[ ";
    for (int i = 3; i < 6; i++) {
        cout << state.parent.at(i) << " ";
    }
    cout << "]\n";

    cout << "[ ";
    for (int i = 6; i < 9; i++) {
        cout << state.parent.at(i) << " ";
    }
    cout << "]\n\n";

    return;
}

void select_and_init_algorithm(node & state) {
    int algorithm;

    cout << "Select algorithm. (1) for Uniform Cost Search, (2) for the Misplace Tile Heuristic, or (3) the Manhattan Distance Heuristic.\n";
    cin >> algorithm;

    //source for outputting time to run function 
    //used this code https://www.geeksforgeeks.org/how-to-measure-time-taken-by-a-program-in-c/
    clock_t t;
    t = clock();
    if (algorithm == 1) { //uniform cost
        uniform_cost_search(state, 0);
    }
    else if (algorithm == 2) { //misplaced tile
        uniform_cost_search(state, 1);
    }
    else {
        uniform_cost_search(state, 2);
    }
    t = clock() - t;
    double time_taken = ((double)t) / CLOCKS_PER_SEC;
    cout << "Execution time: " << setprecision(2) << fixed << time_taken << " seconds\n\n";

    return;
}

void uniform_cost_search(node & game, int heuristic) { 
    vector< vector<int>> repeatedStates; //this vector will hold vectors that we only seen once, we will compare this vector to the parent, if ==, we dont add to pq
    priority_queue<node, vector<node>, myComparator> pq; //this is the priority queue that we gonna use for everything in algo

    int visitNodes = -1;

    if (heuristic == 0) {
        game.h = 0;
    }
    else if (heuristic == 1) {//misplaced tile
        game.h = misplaced_tile(game.parent);
    }
    else if (heuristic == 2) { //manhattan distance
        game.h = manhattan_distance(game.parent);
    }

    //i am pushing the initial state into the pq, and adding it into the vector where we are storing vectors to check for repeated states
    pq.push(game);
    repeatedStates.push_back(game.parent);
    visitNodes++;
    maxSize_pq++;

    while (1) {
        //pq has no more nodes, so we exit
        if (pq.empty() == true) {
            cout << "no possible path,\n";
            return;
        }
        node front = pq.top();
        pq.pop();

        //will print initial state board
        if (visitNodes == 0) {
            cout << "\n\ninitial state\n\n";
            printGame(front);
            cout << "initializing traceback:\n";
        }
        else {
            cout << "The best state to expand with a g(n) = " << front.depth << " and h(n) = " << front.h << " is...\n";
            printGame(front);
        }

        //we add to the overall visited nodes each time we are doing expand func
        if (front.bruh == false) {
            visitNodes = visitNodes + 1;
            front.bruh = true;
        }
        pq_size = pq_size - 1;

        //if front of pq is the goal state
        if (front.parent == goal) {
            cout << "Goal State!\nSolution was at depth:" << front.depth << "\nNumber of nodes expanded: " << visitNodes << " \nMax queue size: " << maxSize_pq << "\n";
            return;
        }
        expanding(front,repeatedStates);
        for (int i = 0; i < front.children.size(); i++) {
            node temp;
            temp.parent = front.children.at(i);
            if (heuristic == 0) {
                temp.depth = front.depth + 1;
                pq.push(temp);
                repeatedStates.push_back(temp.parent);
                pq_size = pq_size + 1;
            }
            else if (heuristic == 1) {
                temp.depth = front.depth + 1;
                temp.h = misplaced_tile(temp.parent);
                pq.push(temp);
                repeatedStates.push_back(temp.parent);
                pq_size = pq_size + 1;
            }
            else {
                temp.depth = front.depth + 1;
                temp.h = manhattan_distance(temp.parent);
                pq.push(temp);
                repeatedStates.push_back(temp.parent);
                pq_size = pq_size + 1;
            }
        }

        //because we added to the overall pq_size when expanding, we check if we have a greater pq_size than max, and set that equal to keep a correct track
        if (maxSize_pq < pq_size) { // pq_size necessary for this operation to make sure maxSize is accurate
            maxSize_pq = pq_size;
        }
    }

    return;
}

bool seenState(vector<int> child, vector<vector<int>> repeatedStates) {
    for (int i = 0; i < repeatedStates.size(); i++) {
        if (repeatedStates.at(i) == child) {
            return true; //this means that repeatedStates already has a vector matching the child
        }
    }

    return false; //we went through the entire vector of repeatedStates and did not find a vector like child, 
}

void expanding(node & state, vector <vector<int>> & repeatedStates) {
    int index; //this will get the location of blank-tile in our vector

    for (int i = 0; i < state.parent.size(); i++) {
        if (state.parent.at(i) == 0) {
            index = i;
        }
    }

    //gonna write the moving up tile check first
    //we can move up if index where 0 is found - 3 > 0

    if ( (index - 3) >= 0) {
        int temp;
        vector<int> up = state.parent; //up copies og puzzle state vector

        temp = up.at(index - 3); //saves value above blank to temp
        up.at(index) = temp;
        up.at(index - 3) = 0;

        //add a function here where we go through every vector in repeatedStates and see if ==, if not, add to state.children vector
        if (seenState(up,repeatedStates) == false) {
            state.children.push_back(up); //adds up as one of the childs of parent
        }
    }

    //gonna write the moving down tile check here
    //we can move down if index where 0 is found + 3 < 9
    if ( (index + 3) < 9) {
        int temp;
        vector<int> down = state.parent; //down copies og puzzle state vector

        temp = down.at(index + 3); //saves value below blank tile to variable temp
        down.at(index) = temp;
        down.at(index + 3) = 0;

        //same as above, add function
        if (seenState(down,repeatedStates) == false) {
            state.children.push_back(down); //adds down as one of childs
        }
    }

    //gonna write the moving left tile check
    //we can move left if index where 0 is found % totalColumns != 0

    if ( (index % columns) != 0) {
        int temp;
        vector<int> left = state.parent;

        temp = left.at(index - 1);
        left.at(index) = temp;
        left.at(index - 1) = 0;

        //function check if statement clause here
        if (seenState(left,repeatedStates) == false) {
            state.children.push_back(left);
        }
    }

    //gonna write the moving right tile check here.
    //we can move right if index where 0 is found % totalcolumns != 2

    if ( (index % columns) != 2) {
        int temp;
        vector<int> right = state.parent;

        temp = right.at(index + 1);
        right.at(index) = temp;
        right.at(index + 1) = 0;

        //function check if statement clause here
        if (seenState(right,repeatedStates) == false) {
            state.children.push_back(right);
        }
    }
    return;
}

void printChildren(node state) { //for bug testing
    for (int i = 0; i < state.children.size(); i++) { //goes through 4 times max
        for (int j = 0; j < state.children.at(i).size(); j++) { //goes through each piece (iterates 9 times)
            if (j % rows == 0) {
                cout << "[ ";
            }
            cout << state.children.at(i).at(j) << " ";
            if (j % rows == 2) {
                cout << "]\n";
            }
        }
        cout << "\n\n";
    }
}

int misplaced_tile(vector<int> game) {
    int tiles = 0;
    for (int i = 0; i < game.size(); i++) {
        if (game.at(i) != i+1 && (i + 1 != 9)) {
            tiles++;
        }
    }
    return tiles;
}

int manhattan_distance(vector<int> game) {
    int tiles = 0;
    int currRow;
    int currCol;
    int gRow;
    int gCol;
    //take absolute value of goal row - currRow (where value is located)
    for (int i =0; i < game.size(); i++) {
        if ((game.at(i) != i + 1) && (game.at(i) != 0)) {//we ignore blank space and only enter if misplaced tile
            currRow = i / rows;//find row of misplaced value
            currCol = i % columns;//find col of misplaced value
            gRow = findGoalrow(game.at(i));//find the row where the misplaced value SHOULD BE
            gCol = findGoalcol(game.at(i));// find the col where the misplaced value SHOULD BE
            tiles = tiles + abs(currRow - gRow) + abs(currCol - gCol); //take abs value to figure out how many rows and cols we need to move misplaced tile to reach appropriate location
        }
    }

    return tiles;
}

int findGoalrow(int value) {
    vector<int> goalS = {1, 2,3,4,5,6,7,8,0};
    int gRow;

    for (int i = 0; i < goalS.size();i++) {
        if (value == goalS.at(i)) {
            gRow = i / rows; //gets row where the value in paremeter is located in the goal state
        }
    }
    return gRow;
}

int findGoalcol(int value) {
    vector<int> goalS = {1, 2,3,4,5,6,7,8,0};
    int gCol;

    for (int i = 0; i < goalS.size();i++) {
        if (value == goalS.at(i)) {
            gCol = i % columns; //gets col where the value in parameter is located in goal state
        }
    }
    return gCol;
}