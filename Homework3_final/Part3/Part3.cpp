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
#include <algorithm>
#include <limits>
#include <unordered_map>
#include <tuple>

using namespace std;

// Structure to hold coordinates
struct Coordinate
{
    double latitude;
    double longitude;
};

// Structure to represent the graph
class Graph
{
private:
    map<string, vector<tuple<string, double, double>>> adjacency_list;

public:
    const map<string, vector<tuple<string, double, double>>> &getAdjacencyList() const
    {
        return adjacency_list;
    }

    // Method to add an edge to the graph
    void addEdge(const string &source, const string &destination, double distance, double timeTaken)
    {
        adjacency_list[source].push_back(make_tuple(destination, distance, timeTaken));
    }
};

double heuristic(const map<string, pair<double, double>> &coordinates, const string &source, const string &destination)
{
    auto p1 = coordinates.at(source);
    auto p2 = coordinates.at(destination);
    double x = pow(p1.first - p2.first, 2);
    double y = pow(p1.second - p2.second, 2);
    return sqrt(x + y);
}

double heuristic_manhattan(const map<string, pair<double, double>> &coordinates, const string &source, const string &destination)
{
    auto p1 = coordinates.at(source);
    auto p2 = coordinates.at(destination);
    return pow((abs(p1.first - p2.first) + abs(p1.second - p2.second)) * 1000.0, 4);
}

void astar(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double>> &coordinates)
{
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

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

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = heuristic(coordinates, get<0>(neighbor), goal);
            if (new_dist < dist[get<0>(neighbor)])
            {
                dist[get<0>(neighbor)] = new_dist;
                prev[get<0>(neighbor)] = current;
                double priority = new_dist + heuristic_value;
                pq.push(make_pair(priority, get<0>(neighbor)));
            }
        }
    }

    cout << "No path found!" << endl;
}

void astar_with_time_heuristic(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double>> &coordinates)
{
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

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

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm with time heuristic took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = get<2>(neighbor);
            if (new_dist < dist[get<0>(neighbor)])
            {
                dist[get<0>(neighbor)] = new_dist;
                prev[get<0>(neighbor)] = current;
                double priority = new_dist + heuristic_value;
                pq.push(make_pair(priority, get<0>(neighbor)));
            }
        }
    }

    cout << "No path found!" << endl;
}

void astar_with_euclidean_square(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double>> &coordinates)
{
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

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

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = heuristic(coordinates, get<0>(neighbor), goal);
            heuristic_value = heuristic_value * heuristic_value;
            if (new_dist < dist[get<0>(neighbor)])
            {
                dist[get<0>(neighbor)] = new_dist;
                prev[get<0>(neighbor)] = current;
                double priority = new_dist + heuristic_value;
                pq.push(make_pair(priority, get<0>(neighbor)));
            }
        }
    }

    cout << "No path found!" << endl;
}

void astar_with_manhattan(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double>> &coordinates)
{
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;

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

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current))
        {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = heuristic_manhattan(coordinates, get<0>(neighbor), goal);
            if (new_dist < dist[get<0>(neighbor)])
            {
                dist[get<0>(neighbor)] = new_dist;
                prev[get<0>(neighbor)] = current;
                double priority = new_dist + heuristic_value;
                pq.push(make_pair(priority, get<0>(neighbor)));
            }
        }
    }

    cout << "No path found!" << endl;
}

int main()
{

    // for small graph
    ifstream inputFile("../Part1/small_graph_with_time.txt");
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
    double timeTaken;
    while (inputFile >> source >> srcX >> srcY >> destination >> destX >> destY >> distance >> timeTaken)
    {
        srccoord.latitude = srcX;
        srccoord.longitude = srcY;
        destcoord.latitude = destX;
        destcoord.longitude = destY;
        coordinates[source] = make_pair(srcX, srcY);
        coordinates[destination] = make_pair(destX, destY);

        graph.addEdge(source, destination, distance, timeTaken);
        // graph.addEdge(destination, source, distance); // Assuming bidirectional roads // will not work since travel time is different for each direction
    }

    // Perform Dijkstra and A* for each combination of source and destination
    vector<string> vertices;
    for (const auto &entry : graph.getAdjacencyList())
    {
        vertices.push_back(entry.first);
    }

    auto total_astar_time_euclidean = 0.0;
    auto total_astar_time_time = 0.0;
    auto total_astar_time_esquare = 0.0;
    auto total_astar_time_manhattan = 0.0;

    int combinations = 0;

    for (const auto &source : vertices)
    {
        for (const auto &destination : vertices)
        {
            if (source != destination)
            {
                cout << "Source: " << source << ", Destination: " << destination << endl;

                // A* with euclidean distance heuristic algorithm
                cout << "A* with Euclidean distance: " << endl;
                auto start_astar_euclidean = chrono::high_resolution_clock::now();
                astar(graph, source, destination, coordinates);
                auto end_astar_euclidean = chrono::high_resolution_clock::now();
                auto duration_astar_1 = chrono::duration_cast<chrono::microseconds>(end_astar_euclidean - start_astar_euclidean);
                total_astar_time_euclidean += duration_astar_1.count();
                cout << '\n';

                // A* with time heuristic algorithm
                cout << "A* with time: " << endl;
                auto start_astar_time = chrono::high_resolution_clock::now();
                astar_with_time_heuristic(graph, source, destination, coordinates);
                auto end_astar_time = chrono::high_resolution_clock::now();
                auto duration_astar_2 = chrono::duration_cast<chrono::microseconds>(end_astar_time - start_astar_time);
                total_astar_time_time += duration_astar_2.count();
                cout << '\n';

                // A* with euclidean distance squared algorithm
                cout << "A* with Euclidean distance squared: " << endl;
                auto start_astar_esquare = chrono::high_resolution_clock::now();
                astar_with_euclidean_square(graph, source, destination, coordinates);
                auto end_astar_esquare = chrono::high_resolution_clock::now();
                auto duration_astar_3 = chrono::duration_cast<chrono::microseconds>(end_astar_esquare - start_astar_esquare);
                total_astar_time_esquare += duration_astar_3.count();
                cout << '\n';

                // A* with manhattan distance ^ 4 algorithm
                cout << "A* with manhattan^4: " << endl;
                auto start_astar_manhattan = chrono::high_resolution_clock::now();
                astar_with_manhattan(graph, source, destination, coordinates);
                auto end_astar_manhattan = chrono::high_resolution_clock::now();
                auto duration_astar_4 = chrono::duration_cast<chrono::microseconds>(end_astar_manhattan - start_astar_manhattan);
                total_astar_time_manhattan += duration_astar_3.count();
                cout << '\n';

                combinations++;
            }
        }
    }

    // Calculate average times
    double avg_astar_time_euclidean = total_astar_time_euclidean / combinations;
    double avg_astar_time_time = total_astar_time_time / combinations;
    double avg_astar_time_esquare = total_astar_time_esquare / combinations;
    double avg_astar_time_manhattan = total_astar_time_manhattan / combinations;

    cout << "Average time taken by A* (using euclidean distance as heuristic): " << avg_astar_time_euclidean << " microseconds." << endl;
    cout << "Average time taken by A* (using time as heuristic): " << avg_astar_time_time << " microseconds." << endl;
    cout << "Average time taken by A* (using euclidean distance squared as heuristic): " << avg_astar_time_esquare << " microseconds." << endl;
    cout << "Average time taken by A* (using manhattan distance ^ 4 as heuristic): " << avg_astar_time_manhattan << " microseconds." << endl;

    return 0;
}
