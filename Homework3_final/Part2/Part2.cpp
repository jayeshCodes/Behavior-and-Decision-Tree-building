// Dijkstras and A*
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cmath>
#include <map>
#include <queue>
#include <stdexcept>
#include <chrono>
#include <unordered_map>

using namespace std;

struct Coordinate
{
    double latitude;
    double longitude;
};

class Graph
{
private:
    map<string, vector<pair<string, double>>> adjacency_list;

public:
    const map<string, vector<pair<string, double>>> &getAdjacencyList() const
    {
        return adjacency_list;
    }

    void addEdge(const string &source, const string &destination, double distance)
    {
        adjacency_list[source].push_back(make_pair(destination, distance));
    }

    void printEdgesWithHeuristics()
    {
        int i = 0;
        for (const auto &node : adjacency_list)
        {
            for (const auto &edge : node.second)
            {

                cout << node.first << " " << edge.first << " " << edge.second << endl;
            }
            i++;
            if (i == 10)
                return;
        }
    }
};

class Large_Graph
{
private:
    map<int, vector<pair<int, double>>> adjacency_list;

public:
    const map<int, vector<pair<int, double>>> &getAdjacencyList() const
    {
        return adjacency_list;
    }

    void addEdge(const int &source, const int &destination, double distance)
    {
        adjacency_list[source].push_back(make_pair(destination, distance));
    }
};

double heuristic(vector<pair<double, double>> coordinates, const int source, const int destination)
{
    auto p1 = coordinates[source];
    auto p2 = coordinates[destination];
    double x = pow(p1.first - p2.first, 2);
    double y = pow(p1.second - p2.second, 2);
    return sqrt(x + y);
}

double heuristic(map<string, pair<double, double>> coordinates, const string source, const string destination)
{
    auto p1 = coordinates[source];
    auto p2 = coordinates[destination];
    double x = pow(p1.first - p2.first, 2);
    double y = pow(p1.second - p2.second, 2);
    return sqrt(x + y);
}

void dijkstra(const Graph &graph, const string &start, const string &goal)
{
    auto start_time = chrono::high_resolution_clock::now(); // Start measuring time
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list)
    {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty())
    {
        // Print current fringe
        cout << "Fringe: ";
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> temp_pq = pq;
        while (!temp_pq.empty())
        {
            cout << temp_pq.top().second << "(" << temp_pq.top().first << ") ";
            temp_pq.pop();
        }
        cout << endl;

        auto current = pq.top().second;
        pq.pop();

        if (current == goal)
        {
            // Reconstruct path and print
            vector<string> path;
            while (current != start)
            {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Shortest path found using Dijkstra's algorithm: ";
            for (const auto &node : path)
            {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now();                               // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration

            cout << "Dijkstra's algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first])
            {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                pq.push(make_pair(new_dist, neighbor.first));
            }
        }
    }

    cout << "No path found using Dijkstra's algorithm!" << endl;
}

void astar(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double>> coordinates)
{
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list)
    {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty())
    {
        // Print current fringe
        cout << "Fringe: ";
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> temp_pq = pq;
        while (!temp_pq.empty())
        {
            cout << temp_pq.top().second << "(" << temp_pq.top().first << ") ";
            temp_pq.pop();
        }
        cout << endl;

        auto current = pq.top().second;
        pq.pop();

        if (current == goal)
        {
            // Reconstruct path and print
            vector<string> path;
            while (current != start)
            {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Path found: ";
            for (const auto &node : path)
            {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now();                               // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first])
            {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                double priority = new_dist + heuristic(coordinates, neighbor.first, goal);
                pq.push(make_pair(priority, neighbor.first));
            }
        }
    }

    cout << "No path found!" << endl;
}

double large_dijkstra(const Large_Graph &graph, const int &start, const int &goal)
{
    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    const auto &adjacency_list = graph.getAdjacencyList();

    for (const auto &node : adjacency_list)
    {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty())
    {
        auto current = pq.top().second;
        pq.pop();

        if (current == goal)
        {
            vector<int> path;
            double cost = dist[goal]; // Get the cost of the path
            while (current != start)
            {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Shortest path found using Dijkstra's algorithm: ";
            for (const auto &node : path)
            {
                cout << node << " ";
            }
            return cost; // Return the cost of the path
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first])
            {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                pq.push(make_pair(new_dist, neighbor.first));
            }
        }
    }

    cout << "No path found using Dijkstra's algorithm!" << endl;
    return numeric_limits<double>::infinity(); // Return infinity if no path is found
}

double a_star_2(const Large_Graph &graph, const int &start, const int &goal, vector<pair<double, double>> coordinates)
{
    unordered_map<int, double> dist;
    unordered_map<int, int> prev;
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;

    const auto &adjacency_list = graph.getAdjacencyList();

    for (const auto &node : adjacency_list)
    {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(heuristic(coordinates, start, goal), start));

    while (!pq.empty())
    {
        auto current = pq.top().second;
        pq.pop();

        if (current == goal)
        {
            vector<int> path;
            double cost = dist[goal]; // Get the cost of the path
            while (current != start)
            {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Shortest path found using A* algorithm: ";
            for (const auto &node : path)
            {
                cout << node << " ";
            }
            return cost; // Return the cost of the path
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first])
            {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                pq.push(make_pair(new_dist + heuristic(coordinates, neighbor.first, goal), neighbor.first));
            }
        }
    }

    cout << "No path found using A* algorithm!" << endl;
    return numeric_limits<double>::infinity(); // Return infinity if no path is found
}

int main()
{

    // for small graph
    ifstream inputFile("../Part1/small_graph.txt");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    map<string, pair<double, double>> coordinates;
    Graph graph;
    string source, destination;
    Coordinate srccoord, destcoord;
    double distance;
    double srcX, srcY, destX, destY;
    while (inputFile >> source >> srcX >> srcY >> destination >> destX >> destY >> distance)
    {
        srccoord.latitude = srcX;
        srccoord.longitude = srcY;
        destcoord.latitude = destX;
        destcoord.longitude = destY;
        coordinates[source] = make_pair(srcX, srcY);
        coordinates[destination] = make_pair(destX, destY);
        // cout<<source<<srcX<<srcY<<destination<<destX<<destY<<distance;

        graph.addEdge(source, destination, distance);
        graph.addEdge(destination, source, distance); // Assuming bidirectional roads
    }

    // Perform Dijkstra and A* for each combination of source and destination
    vector<string> vertices;
    for (const auto &entry : graph.getAdjacencyList())
    {
        vertices.push_back(entry.first);
    }

    auto total_dijkstra_time = 0.0;
    auto total_astar_time = 0.0;

    int combinations = 0;

    for (const auto &source : vertices)
    {
        for (const auto &destination : vertices)
        {
            if (source != destination)
            {
                cout << "Source: " << source << ", Destination: " << destination << endl;
                // Dijkstra's algorithm
                auto start_dijkstra = chrono::high_resolution_clock::now();
                dijkstra(graph, source, destination);
                auto end_dijkstra = chrono::high_resolution_clock::now();
                auto duration_dijkstra = chrono::duration_cast<chrono::microseconds>(end_dijkstra - start_dijkstra);
                total_dijkstra_time += duration_dijkstra.count();
                cout << '\n';

                // A* algorithm
                auto start_astar = chrono::high_resolution_clock::now();
                astar(graph, source, destination, coordinates);
                auto end_astar = chrono::high_resolution_clock::now();
                auto duration_astar = chrono::duration_cast<chrono::microseconds>(end_astar - start_astar);
                total_astar_time += duration_astar.count();
                cout << '\n';

                combinations++;
            }
        }
    }

    // Calculate average times
    double avg_dijkstra_time = total_dijkstra_time / combinations;
    double avg_astar_time = total_astar_time / combinations;

    cout << "Average time taken by Dijkstra: " << avg_dijkstra_time << " microseconds." << endl;
    cout << "Average time taken by A*: " << avg_astar_time << " microseconds." << endl;

    // for large graph
    // for large graph
    ifstream inputFile2("../Part1/large_graph_with_coordinates.txt");
    if (!inputFile2.is_open())
    {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    vector<pair<double, double>> coordinates2;
    Large_Graph large_graph;
    Coordinate srccoord2, destcoord2;
    int source2, destination2;
    double distance2;
    double sX, sY, dX, dY;

    // Get the maximum vertex ID to determine the size of the array
    int max_vertex_id = 0;
    while (inputFile2 >> source2 >> sX >> sY >> destination2 >> dX >> dY >> distance2)
    {
        srccoord2.latitude = sX;
        srccoord2.longitude = sY;
        destcoord2.latitude = dX;
        destcoord2.longitude = dY;

        // Update max_vertex_id if necessary
        max_vertex_id = max(max_vertex_id, max(source2, destination2));

        // Push coordinates into the vector
        if (source2 >= coordinates2.size())
            coordinates2.resize(source2 + 1);
        if (destination2 >= coordinates2.size())
            coordinates2.resize(destination2 + 1);
        coordinates2[source2] = make_pair(sX, sY);
        coordinates2[destination2] = make_pair(dX, dY);

        large_graph.addEdge(source2, destination2, distance2);
        large_graph.addEdge(destination2, source2, distance2); // Assuming bidirectional edges
    }

    // Ensure that coordinates2 contains all vertices up to max_vertex_id
    coordinates2.resize(max_vertex_id + 1, make_pair(0.0, 0.0));

    cout << "Printing coordinates : " << endl;
    for (int i = 0; i <= 10; i++)
    {
        cout << "Coordinates[" << i << "] = " << coordinates2[i].first << "," << coordinates2[i].second << endl;
    }

    cout << '\n';

    cout << "Dijkstras on large graph" << endl;
    auto start_dijkstra = chrono::high_resolution_clock::now();
    double dcost = large_dijkstra(large_graph, 100, 200);
    auto end_dijkstra = chrono::high_resolution_clock::now();
    cout << endl;
    cout << "Time taken by dijkstra's algorithm: " << chrono::duration_cast<chrono::microseconds>(end_dijkstra - start_dijkstra).count() << " microseconds" << endl;
    cout << "Cost: " << dcost << endl;
    cout << '\n';

    cout << "A* on large graph" << endl;
    auto start_astar = chrono::high_resolution_clock::now();
    double acost = a_star_2(large_graph, 100, 200, coordinates2);
    auto end_astar = chrono::high_resolution_clock::now();
    cout << endl;
    cout << "Time taken by dijkstra's algorithm: " << chrono::duration_cast<chrono::microseconds>(end_astar - start_astar).count() << " microseconds" << endl;
    cout << "Cost: " << acost << endl;
    cout << '\n';

    return 0;
}
