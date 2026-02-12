#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <Windows.h>
#include <string>
#include<chrono>

using namespace std;

/*
    Enumeration of possible directions of movement
    of the empty tile (0).
*/
enum direction {
    None,
    LEFT,
    DOWN,
    RIGHT,
    UP
};

/*
    Structure that describes one node (state)
    of the search tree.

    field         - current puzzle configuration
    point_x/y     - coordinates of empty tile
    index         - unique state number
    parent_index  - index of parent state
    depth         - level in search tree
    dir           - direction that produced this state
*/
struct State
{
    vector<vector<int>> field;
    int point_x;
    int point_y;

    int index;
    int parent_index;
    int depth;

    direction dir;
};

// ---------- Utility ----------

/*
    Converts 2D puzzle configuration into a string.
    This string is used as a unique key inside
    unordered_set to quickly detect repeated states.
*/
string encode(const vector<vector<int>>& field)
{
    string code;

    for (int i = 0; i < field.size(); i++)
    {
        for (int j = 0; j < field[i].size(); j++)
        {
            code += char('0' + field[i][j]);
        }
    }

    return code;
}

/*
    Checks if puzzle configuration is solvable.

    For 3x3 puzzle:
    If number of inversions is even -> solvable.
    If odd -> not solvable.

    Inversion: pair of elements where
    larger number appears before smaller one.
*/
bool isSolution(const vector<vector<int>>& initial)
{
    string key = encode(initial);

    int count = 0;
    int inverse = 0;

    for (int i = 0; i < key.size(); i++)
    {
        int num = key[i] - '0';

        // Skip empty tile
        if (num == 0)
        {
            continue;
        }

        for (int j = i + 1; j < key.size(); j++)
        {
            int el = key[j] - '0';

            if (el < num && el != 0)
            {
                count++;
            }
        }

        inverse += count;
        count = 0;
    }

    if (inverse % 2 == 0)
        return true;

    return false;
}

/*
    Restores solution path from goal state
    back to initial state using parent_index.
*/
vector<State> restorePath(const vector<State>& visited, const int& goal_index)
{
    vector<State> path;
    State state = visited[goal_index];

    while (state.parent_index != -1)
    {
        path.emplace(path.begin(), state);
        state = visited[state.parent_index];
    }

    path.emplace(path.begin(), state);

    return path;
}

/*
    Prints puzzle configuration to console.
    Zero is displayed as '.' for clarity.
*/
void printField(const vector<vector<int>>& field)
{
    for (int i = 0; i < field.size(); i++)
    {
        for (int j = 0; j < field[i].size(); j++)
        {
            if (field[i][j] == 0)
                cout << ". ";
            else
                cout << field[i][j] << " ";
        }
        cout << endl;
    }
}

/*
    Prints textual representation of move direction.
*/
void printDir(direction dir)
{
    if (dir == UP)
        cout << "UP";
    else if (dir == DOWN)
        cout << "DOWN";
    else if (dir == LEFT)
        cout << "LEFT";
    else if (dir == RIGHT)
        cout << "RIGHT";
}

/*
    Prints current BFS queue content.
*/
void printQueue(queue<State> q)
{
    cout << "queue = ";

    while (!q.empty())
    {
        cout << "S" << q.front().index << " ";
        q.pop();
    }

    cout << endl;
}

/*
    Prints all visited states.
*/
void printVisited(const vector<State>& visited)
{
    cout << "visited = ";

    for (int i = 0; i < visited.size(); i++)
    {
        cout << "S" << visited[i].index << " ";
    }

    cout << endl << endl;
}

// ---------- BFS ----------

/*
    Breadth-First Search algorithm.

    Explores puzzle states level by level.
    Uses:
        queue - for BFS traversal
        unordered_set - for fast duplicate detection
*/
void bfs(State start, const vector<vector<int>>& goal, const bool& step_by_step)
{
    queue<State> q;
    vector<State> visited_states;
    unordered_set<string> visited_keys;

    int generated_states = 0;   // total generated states
    int stored_states = 0;      // states added to queue
    int discarded_states = 0;   // duplicate states

    q.push(start);
    visited_states.push_back(start);
    visited_keys.insert(encode(start.field));

    // ---------- LEVEL 0 ----------
    if (step_by_step)
    {
        cout << "Level 0\n\n";
        cout << "S0:\n\n";
        printField(start.field);
        cout << "\nqueue = S0\n";
        cout << "visited = S0\n";
        cout << "parent[S0] = none\n\n";
        cout << "Generated states: " << generated_states << endl << endl;
        cout << "Stored states: " << stored_states << endl << endl;
        cout << "Discarded states: " << discarded_states << endl << endl;
        cout << "------------------------\n\n";
        cout << "Press ENTER to continue...";
        cin.get();
        cout << "\n\n";
    }

    int current_level = -1;

    while (!q.empty())
    {
        State current = q.front();
        q.pop();

        if (step_by_step)
        {
            if (current.depth != current_level)
            {
                if (current_level != -1)
                {
                    cout << "Generated states: " << generated_states << endl << endl;
                    cout << "Stored states: " << stored_states << endl << endl;
                    cout << "Discarded states: " << discarded_states << endl << endl;

                    cout << "\n------------------------\n\n";
                    cout << "Press ENTER to continue...";
                    cin.get();
                    cout << "\n\n";
                }

                current_level = current.depth;
                cout << "Level " << (current_level + 1) << "\n\n";
            }

            cout << "Current state S" << current.index << "\n\n";
            printField(current.field);
            cout << endl;
        }

        /*
            Goal test.
            If goal configuration is reached,
            restore and print solution path.
        */
        if (current.field == goal)
        {
            vector<State> path = restorePath(visited_states, current.index);

            cout << "\n------------------------\n\n";
            cout << "Sequence of moves:\n\n";
            printField(path[0].field);
            cout << "\n------------------------\n\n";

            for (int i = 1; i < path.size(); i++)
            {
                cout << "Move: ";
                printDir(path[i].dir);
                cout << "\n\n";
                printField(path[i].field);
                cout << "\n------------------------\n\n";
            }

            int tree_depth = path.size() - 1;

            cout << "Generated states: " << generated_states << endl << endl;
            cout << "Stored states: " << stored_states << endl << endl;
            cout << "Discarded states: " << discarded_states << endl << endl;
            cout << "Depth of tree: " << tree_depth << endl << endl;

            return;
        }

        // LEFT move generation
        if (current.point_y - 1 >= 0)
        {
            State next = current;

            swap(next.field[next.point_x][next.point_y],
                next.field[next.point_x][next.point_y - 1]);

            string key = encode(next.field);
            generated_states++;

            if (!visited_keys.count(key))
            {
                next.dir = LEFT;
                next.point_y--;
                next.parent_index = current.index;
                next.index = visited_states.size();
                next.depth = current.depth + 1;
                stored_states++;

                visited_states.push_back(next);
                visited_keys.insert(key);
                q.push(next);

                if (step_by_step)
                {
                    cout << "New state S" << next.index << ":\n\n";
                    printField(next.field);
                    cout << "\n";
                    cout << "parent[S" << next.index << "] = S"
                        << current.index << endl << endl;
                }
            }
            else
            {
                discarded_states++;
            }
        }

        // DOWN move generation
        if (current.point_x + 1 < 3)
        {
            State next = current;

            swap(next.field[next.point_x][next.point_y],
                next.field[next.point_x + 1][next.point_y]);

            string key = encode(next.field);
            generated_states++;

            if (!visited_keys.count(key))
            {
                next.dir = DOWN;
                next.point_x++;
                next.parent_index = current.index;
                next.index = visited_states.size();
                next.depth = current.depth + 1;
                stored_states++;

                visited_states.push_back(next);
                visited_keys.insert(key);
                q.push(next);

                if (step_by_step)
                {
                    cout << "New state S" << next.index << ":\n\n";
                    printField(next.field);
                    cout << "\n";
                    cout << "parent[S" << next.index << "] = S"
                        << current.index << endl << endl;
                }
            }
            else
            {
                discarded_states++;
            }
        }

        // RIGHT move generation
        if (current.point_y + 1 < 3)
        {
            State next = current;

            swap(next.field[next.point_x][next.point_y],
                next.field[next.point_x][next.point_y + 1]);

            string key = encode(next.field);
            generated_states++;

            if (!visited_keys.count(key))
            {
                next.dir = RIGHT;
                next.point_y++;
                next.parent_index = current.index;
                next.index = visited_states.size();
                next.depth = current.depth + 1;
                stored_states++;

                visited_states.push_back(next);
                visited_keys.insert(key);
                q.push(next);

                if (step_by_step)
                {
                    cout << "New state S" << next.index << ":\n\n";
                    printField(next.field);
                    cout << "\n";
                    cout << "parent[S" << next.index << "] = S"
                        << current.index << endl << endl;
                }
            }
            else
            {
                discarded_states++;
            }
        }

        // UP move generation
        if (current.point_x - 1 >= 0)
        {
            State next = current;

            swap(next.field[next.point_x][next.point_y],
                next.field[next.point_x - 1][next.point_y]);

            string key = encode(next.field);
            generated_states++;

            if (!visited_keys.count(key))
            {
                next.dir = UP;
                next.point_x--;
                next.parent_index = current.index;
                next.index = visited_states.size();
                next.depth = current.depth + 1;
                stored_states++;

                visited_states.push_back(next);
                visited_keys.insert(key);
                q.push(next);

                if (step_by_step)
                {
                    cout << "New state S" << next.index << ":\n\n";
                    printField(next.field);
                    cout << "\n";
                    cout << "parent[S" << next.index << "] = S"
                        << current.index << endl << endl;
                }
            }
            else
            {
                discarded_states++;
            }
        }

        if (step_by_step)
        {
            printQueue(q);
            printVisited(visited_states);
        }
    }

    cout << "\n------------------------\n\n";
    cout << "Search finished.\n";
}

// ---------- MAIN ----------

int main()
{
    /*
        Initial configuration of puzzle.
    */
    vector<vector<int>> initial = {
        {2, 3, 8},
        {1, 6, 4},
        {7, 0, 5}
    };

    /*
        Goal configuration.
    */
    vector<vector<int>> goal = {
        {1, 2, 3},
        {4, 5, 6},
        {7, 8, 0}
    };

    State start;
    start.field = initial;

    /*
        Find coordinates of empty tile (0).
    */
    for (int i = 0; i < initial.size(); i++)
    {
        for (int j = 0; j < initial[i].size(); j++)
        {
            if (initial[i][j] == 0)
            {
                start.point_x = i;
                start.point_y = j;
                break;
            }
        }
    }

    start.index = 0;
    start.parent_index = -1;
    start.depth = 0;
    start.dir = None;

    /*
        Ask user whether to show detailed BFS process.
    */
    string answer;
    cout << "Do you want to see all the alculations?\n\n";
    cout << "1 - yes\n\n";
    cout << "2 - no\n\n";
    cout << "Answer: ";
    getline(cin, answer);

    while (answer != "1" && answer != "2")
    {
        system("cls");
        cout << "Do you want to see all the alculations?\n\n";
        cout << "1 - yes\n\n";
        cout << "2 - no\n\n";
        cout << "Answer: ";
        getline(cin, answer);
    }

    system("cls");

    bool step_by_step = false;
    bool is_solution = isSolution(initial);

    if (answer == "1")
    {
        step_by_step = true;
    }

    /*
        If puzzle is not solvable and user chose non-step mode,
        print warning message.
    */
    if (!is_solution && answer == "2")
    {
        cout << "This example has no solutions!!!" << endl << endl;
    }
    else
    {
        auto start_time = chrono::high_resolution_clock::now();

        bfs(start, goal, step_by_step);

        auto end_time = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);


        cout << "\n\nTotal BFS Execution time: " << duration.count() << " ms" << endl;

        

    }
}
