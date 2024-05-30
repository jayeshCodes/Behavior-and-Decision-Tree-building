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

// Define a placeholder heuristic function
double heuristic(const string &current, const string &goal)
{
    // You can implement a heuristic function here.
    // For example, Euclidean distance can be used if the nodes have coordinates.
    return 0; // Placeholder for simplicity
}

struct Edge
{
    string dest;
    int weight;
};

typedef unordered_map<string, vector<Edge> > Graph;

void addEdge(Graph &graph, const string &src, const string &dest, int weight)
{
    Edge newEdge;
    newEdge.dest = dest;
    newEdge.weight = weight;
    graph[src].push_back(newEdge);
}

void dijkstra(const Graph &graph, const string &start)
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

void astar(const Graph &graph, const string &start, const string &goal)
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
    pq.push(make_pair(heuristic(start, goal), start));

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
                pq.push(make_pair(dist[v] + heuristic(v, goal), v));
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
        cout << "Failed to open small_graph.txt!\n";
        return 1;
    }

    Graph small_graph;

    string src, dest;
    int weight;

    while (file >> src >> dest >> weight)
    {
        addEdge(small_graph, src, dest, weight);
    }

    file.close();

    ifstream file2("graph_with_weights.txt");
    if (!file2.is_open())
    {
        cout << "Failed to open graph_with_weights.txt!\n";
        return 1;
    }

    Graph large_graph;

    string src2, dest2;
    int weight2;

    while (file2 >> src2 >> dest2 >> weight2)
    {
        addEdge(large_graph, src2, dest2, weight2);
    }

    file2.close();

    // Example usage of Dijkstra's algorithm
    cout << "Dijkstra's on large graph: \n";
    dijkstra(large_graph, "0");
    cout << '\n';

    // Example usage of A* algorithm
    cout << "A* on large graph: \n";
    astar(large_graph, "1", "0");
    cout << '\n';

    cout << "Dijkstra's on small graph: \n";
    dijkstra(small_graph, "Kalyani");
    cout << '\n';

    // Example usage of A* algorithm
    cout << "A* on small graph: \n";
    astar(small_graph, "Viman", "Mundhwa");
    cout << '\n';

    return 0;
}
