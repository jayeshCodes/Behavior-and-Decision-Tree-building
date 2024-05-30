#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <queue>
#include <limits>
#include <chrono>
#include <cmath>

using namespace std;
using namespace std::chrono;

struct Coordinates {
    double latitude;
    double longitude;
};

// Define a placeholder heuristic function
double heuristic(const Coordinates &currentCoords, const Coordinates &goalCoords)
{
    // Calculate Euclidean distance between two coordinates
    double dx = currentCoords.latitude - goalCoords.latitude;
    double dy = currentCoords.longitude - goalCoords.longitude;
    return sqrt(dx * dx + dy * dy);
}

struct Edge
{
    string dest;
    int weight;
};

typedef unordered_map<string, vector<Edge> > Graph;

void addEdge(Graph &graph, unordered_map<string, Coordinates> &nodeCoordinates, const string &src, const string &dest, int weight, const Coordinates &srcCoords, const Coordinates &destCoords)
{
    Edge newEdge;
    newEdge.dest = dest;
    newEdge.weight = weight;
    graph[src].push_back(newEdge);
    nodeCoordinates[src] = srcCoords;
    nodeCoordinates[dest] = destCoords;
}

void dijkstra(const Graph &graph, const unordered_map<string, Coordinates> &nodeCoordinates, const string &start)
{
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<int, string>, vector<pair<int, string> >, greater<pair<int, string> > > pq;

    // Initialize distances
    for (const auto &node : graph)
    {
        dist[node.first] = numeric_limits<int>::max();
        prev[node.first] = "";
    }
    dist[start] = 0;
    pq.push(make_pair(0, start));

    // Start time measurement
    auto start_time = high_resolution_clock::now();

    while (!pq.empty())
    {
        string u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (d > dist[u])
            continue;

        for (const Edge &e : graph.at(u))
        {
            string v = e.dest;
            int w = e.weight;
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push(make_pair(dist[v], v));
            }
        }
    }

    // End time measurement
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time).count();

    cout << "Dijkstra's algorithm runtime: " << duration << " milliseconds" << endl;
    cout << flush; // Flush the output buffer

    // Print shortest distances from start node
    for (const auto &node : dist)
    {
        cout << "Shortest distance from " << start << " to " << node.first << ": " << node.second << endl;
    }
}

void astar(const Graph &graph, const unordered_map<string, Coordinates> &nodeCoordinates, const string &start, const string &goal)
{
    unordered_map<string, int> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<int, string>, vector<pair<int, string> >, greater<pair<int, string> > > pq;

    // Initialize distances
    for (const auto &node : graph)
    {
        dist[node.first] = numeric_limits<int>::max();
        prev[node.first] = "";
    }
    dist[start] = 0;
    pq.push(make_pair(heuristic(nodeCoordinates.at(start), nodeCoordinates.at(goal)), start));

    // Start time measurement
    auto start_time = high_resolution_clock::now();

    while (!pq.empty())
    {
        string u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (u == goal)
            break;

        if (d > dist[u])
            continue;

        for (const Edge &e : graph.at(u))
        {
            string v = e.dest;
            int w = e.weight;
            if (dist[u] + w < dist[v])
            {
                dist[v] = dist[u] + w;
                prev[v] = u;
                pq.push(make_pair(dist[v] + heuristic(nodeCoordinates.at(v), nodeCoordinates.at(goal)), v));
            }
        }
    }

    // End time measurement
    auto end_time = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end_time - start_time).count();

    cout << "A* algorithm runtime: " << duration << " milliseconds" << endl;

    // Print shortest path from start to goal
    vector<string> path;
    string current = goal;
    while (current != "")
    {
        path.push_back(current);
        current = prev[current];
    }
    reverse(path.begin(), path.end());

    cout << "Shortest path from " << start << " to " << goal << ": ";
    for (const string &node : path)
    {
        cout << node << " -> ";
    }
    cout << endl;
}

int main()
{
    ifstream file("small_graph.txt");
    if (!file.is_open())
    {
        cout << "Failed to open input.txt!\n";
        return 1;
    }

    Graph graph;
    unordered_map<string, Coordinates> nodeCoordinates;

    // string src, dest;
    // double srcLat, srcLong, destLat, destLong;
    // int weight;

    // while (file >> src >> srcLat >> srcLong >> dest >> destLat >> destLong >> weight)
    // {
    //     Coordinates srcCoords = {srcLat, srcLong};
    //     cout<<srcCoords.latitude<<","<<srcCoords.longitude<<'\n';
    //     Coordinates destCoords = {destLat, destLong};
    //     cout<<destCoords.latitude<<","<<destCoords.longitude<<'\n';
    //     addEdge(graph, nodeCoordinates, src, dest, weight, srcCoords, destCoords);
    // }

    // file.close();

    // Edge: Cantt <-> Airport
// Edge: Cantt <-> Airport
addEdge(graph, nodeCoordinates, "Cantt", "Airport", 7, {18.607615207948363, 73.87509718082079}, {18.579546376032056, 73.90897044170211});

// Continue adding edges for all pairs of nodes...



    // Example usage of Dijkstra's algorithm
    // cout << "Dijkstra's algorithm: \n";
    // dijkstra(graph, nodeCoordinates, "Viman");
    // cout << '\n';

    // // Example usage of A* algorithm
    // cout << "A* algorithm: \n";
    // astar(graph, nodeCoordinates, "Viman", "Mundhwa");
    // cout << '\n';

    return 0;
}
