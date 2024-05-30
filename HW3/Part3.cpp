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
struct Coordinate {
    double latitude;
    double longitude;
};

// Structure to represent the graph
class Graph {
private:
    map<string, vector< tuple<string, double, double> > > adjacency_list;

public:
    const map<string, vector<tuple<string, double, double> > >& getAdjacencyList() const {
        return adjacency_list;
    }

    // Method to add an edge to the graph
    void addEdge(const string& source, const string& destination, double distance, double timeTaken) {
        adjacency_list[source].push_back(make_tuple(destination, distance, timeTaken));
    }
};

double heuristic(const map<string, pair<double, double> >& coordinates, const string& source, const string& destination) {
    auto p1 = coordinates.at(source);
    auto p2 = coordinates.at(destination);
    double x = pow(p1.first - p2.first, 2);
    double y = pow(p1.second - p2.second, 2);
    return sqrt(x + y);
}

void astar(const Graph& graph, const string& start, const string& goal, map<string, pair<double, double> >& coordinates) {
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto& adjacency_list = graph.getAdjacencyList();

    for (const auto& node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {
        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Path found: ";
            for (const auto& node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto& neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = heuristic(coordinates, get<0>(neighbor), goal);
            if (new_dist < dist[get<0>(neighbor)]) {
                dist[get<0>(neighbor)] = new_dist;
                prev[get<0>(neighbor)] = current;
                double priority = new_dist + heuristic_value;
                pq.push(make_pair(priority, get<0>(neighbor)));
            }
        }
    }

    cout << "No path found!" << endl;
}

void astar_with_time_heuristic(const Graph& graph, const string& start, const string& goal, map<string, pair<double, double> >& coordinates) {
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto& adjacency_list = graph.getAdjacencyList();

    for (const auto& node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {
        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Path found: ";
            for (const auto& node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
            cout << "A* algorithm with time heuristic took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto& neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + get<1>(neighbor);
            double heuristic_value = get<2>(neighbor);
            if (new_dist < dist[get<0>(neighbor)]) {
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
    ifstream inputFile("small_graph_with_time.txt");
    if (!inputFile.is_open())
    {
        cerr << "Failed to open file." << endl;
        return 1;
    }

    map<string, pair<double, double> > coordinates;
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
        // graph.addEdge(destination, source, distance); // Assuming bidirectional roads
    }

    cout<<"A* with euclidean distance as heuristic"<<endl;
    astar(graph, "Viman", "Kondhwa", coordinates);
    cout<<'\n';


    cout<<"A* with time as heuristic"<<endl;
    astar_with_time_heuristic(graph, "Viman", "Kondhwa", coordinates);


    return 0;
}
