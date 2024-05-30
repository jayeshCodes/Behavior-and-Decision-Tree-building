#include <iostream>
#include <vector>
#include <queue>
#include <limits>

// Define graph structure
struct Edge {
    int target;
    int weight;
};

typedef std::vector<std::vector<Edge>> Graph;

// Dijkstra's Algorithm
std::vector<int> dijkstra(const Graph& graph, int source) {
    int n = graph.size();
    std::vector<int> distances(n, std::numeric_limits<int>::max());
    std::vector<bool> visited(n, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    distances[source] = 0;
    pq.push({0, source});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        for (const auto& edge : graph[u]) {
            int v = edge.target;
            int weight = edge.weight;
            if (!visited[v] && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                pq.push({distances[v], v});
            }
        }
    }

    return distances;
}

// A* Algorithm
std::vector<int> astar(const Graph& graph, int source, int target, std::vector<int>& heuristic) {
    int n = graph.size();
    std::vector<int> distances(n, std::numeric_limits<int>::max());
    std::vector<bool> visited(n, false);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<std::pair<int, int>>> pq;

    distances[source] = 0;
    pq.push({0 + heuristic[source], source});

    while (!pq.empty()) {
        int u = pq.top().second;
        pq.pop();

        if (visited[u]) continue;
        visited[u] = true;

        if (u == target) break;

        for (const auto& edge : graph[u]) {
            int v = edge.target;
            int weight = edge.weight;
            if (!visited[v] && distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                pq.push({distances[v] + heuristic[v], v});
            }
        }
    }

    return distances;
}

int main() {
    // Construct your graph here

    // Run Dijkstra's Algorithm and A* Algorithm

    return 0;
}
