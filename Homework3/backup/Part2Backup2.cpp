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


using namespace std;

struct Coordinate
{
    double latitude;
    double longitude;
};

class Graph
{
private:
    map<string, vector<pair<string, double> > > adjacency_list;

public:
    const map<string, vector<pair<string, double> > > &getAdjacencyList() const {
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
            if(i==10)
            return;
        }
    }
};



double heuristic(map<string, pair<double, double> > coordinates, const string source, const string destination)
{
    auto p1 = coordinates[source];
    auto p2 = coordinates[destination];
    double x = pow(p1.first-p2.first,2);
    double y = pow(p1.second-p2.second,2);
    return sqrt(x+y);
}

void dijkstra(const Graph &graph, const string &start, const string &goal) {
    auto start_time = chrono::high_resolution_clock::now(); // Start measuring time
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {
        // Print current fringe
        cout << "Fringe: ";
        priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > temp_pq = pq;
        while (!temp_pq.empty()) {
            cout << temp_pq.top().second << "(" << temp_pq.top().first << ") ";
            temp_pq.pop();
        }
        cout << endl;

        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            // Reconstruct path and print
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Shortest path found using Dijkstra's algorithm: ";
            for (const auto &node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now(); // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration

            cout << "Dijkstra's algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first]) {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                pq.push(make_pair(new_dist, neighbor.first));
            }
        }
    }

    cout << "No path found using Dijkstra's algorithm!" << endl;

}


void astar(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double> > coordinates) {
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {
        // Print current fringe
        cout << "Fringe: ";
        priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > temp_pq = pq;
        while (!temp_pq.empty()) {
            cout << temp_pq.top().second << "(" << temp_pq.top().first << ") ";
            temp_pq.pop();
        }
        cout << endl;

        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            // Reconstruct path and print
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Path found: ";
            for (const auto &node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now(); // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first]) {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                double priority = new_dist + heuristic(coordinates, neighbor.first, goal);
                pq.push(make_pair(priority, neighbor.first));
            }
        }
    }

    cout << "No path found!" << endl;
}

void large_dijkstra(const Graph &graph, const string &start, const string &goal) {
    auto start_time = chrono::high_resolution_clock::now(); // Start measuring time
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {

        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            // Reconstruct path and print
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Shortest path found using Dijkstra's algorithm: ";
            for (const auto &node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now(); // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration

            cout << "Dijkstra's algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first]) {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                pq.push(make_pair(new_dist, neighbor.first));
            }
        }
    }

    cout << "No path found using Dijkstra's algorithm!" << endl;

}


void large_astar(const Graph &graph, const string &start, const string &goal, map<string, pair<double, double> > coordinates) {
    auto start_time = chrono::high_resolution_clock::now();
    unordered_map<string, double> dist;
    unordered_map<string, string> prev;
    priority_queue<pair<double, string>, vector<pair<double, string> >, greater<pair<double, string> > > pq;

    const auto &adjacency_list = graph.getAdjacencyList(); // Access adjacency list using the public member function

    // Initialize distances
    for (const auto &node : adjacency_list) {
        dist[node.first] = numeric_limits<double>::infinity();
    }
    dist[start] = 0.0;

    pq.push(make_pair(0.0, start));

    while (!pq.empty()) {

        auto current = pq.top().second;
        pq.pop();

        if (current == goal) {
            // Reconstruct path and print
            vector<string> path;
            while (current != start) {
                path.push_back(current);
                current = prev[current];
            }
            path.push_back(start);
            reverse(path.begin(), path.end());

            cout << "Path found: ";
            for (const auto &node : path) {
                cout << node << " ";
            }
            cout << endl;

            auto end_time = chrono::high_resolution_clock::now(); // End measuring time
            auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time); // Calculate duration
            cout << "A* algorithm took " << duration.count() << " microseconds." << endl;
            return;
        }

        for (const auto &neighbor : adjacency_list.at(current)) {
            double new_dist = dist[current] + neighbor.second;

            if (new_dist < dist[neighbor.first]) {
                dist[neighbor.first] = new_dist;
                prev[neighbor.first] = current;
                double priority = new_dist + heuristic(coordinates, neighbor.first, goal);
                pq.push(make_pair(priority, neighbor.first));
            }
        }
    }

    cout << "No path found!" << endl;
}


int main()
{

    // for small graph
    ifstream inputFile("small_graph.txt");
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

    // for (auto &key : coordinates)
    // {
    //     cout << key.first << " " << key.second.first << " " << key.second.second << endl;
    // }

    // // cout << "Edges with Heuristics:" << endl;
    // graph.printEdgesWithHeuristics();
    cout<<"A* on small graph"<<endl;
    astar(graph, "Airport", "Wagholi", coordinates);
    cout<<'\n';
    cout<<"Dijkstras on small graph"<<endl;
    dijkstra(graph, "Airport", "Wagholi");
    cout<<'\n';

    // for large graph
// for large graph
ifstream inputFile2("large_graph_with_coordinates.txt");
if (!inputFile2.is_open())
{
    cerr << "Failed to open file." << endl;
    return 1;
}

map<string, pair<double, double> > coordinates2;
Graph large_graph;
Coordinate srccoord2, destcoord2;
string source2, destination2;
double distance2;
double sX, sY, dX, dY;
while (inputFile2 >> source2 >> sX >> sY >> destination2 >> dX >> dY >> distance2)
{
    // Ensure coordinates are unique
    // if (coordinates2.find(source2) == coordinates2.end()) {
    //     coordinates2[source2] = make_pair(0.0, 0.0);
    // }
    // if (coordinates2.find(destination2) == coordinates2.end()) {
    //     coordinates2[destination2] = make_pair(0.0, 0.0);
    // }
    srccoord2.latitude = sX;
    srccoord2.longitude = sY;
    destcoord2.latitude = dX;
    destcoord2.longitude = dY;
    coordinates2[source2] = make_pair(sX, sY);
    coordinates2[destination2] = make_pair(dX, dY);


    large_graph.addEdge(source2, destination2, distance2);
    large_graph.addEdge(destination2, source2, distance2); // Assuming bidirectional roads
}

    cout<<"Dijkstras on large graph"<<endl;
    large_dijkstra(large_graph, "1", "1000");
    cout<<'\n';
    cout<<"A* on large graph"<<endl;
    astar(large_graph, "1", "1000", coordinates2);
    cout<<'\n';

    // large_graph.printEdgesWithHeuristics();






    return 0;
}
