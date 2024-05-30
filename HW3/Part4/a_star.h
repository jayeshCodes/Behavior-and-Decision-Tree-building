#ifndef A_STAR_H
#define A_STAR_H

#include <functional>
#include <map>
#include <vector>
#include <SFML/System/Vector2.hpp> // Include SFML Vector2 header

typedef sf::Vector2i NodeID;
typedef float Cost;
typedef std::pair<size_t, Cost> Edge;
typedef std::map<size_t, std::map<size_t, Cost>> AdjacencyList;
typedef std::vector<size_t> Path;
typedef std::function<Cost(int, int, int, int)> HeuristicFn; // Update HeuristicFn signature

// Update function signature to accept SFML Vector2i for start and target
Path a_star(AdjacencyList &adjacency, 
            sf::Vector2i start,
            sf::Vector2i target,
            HeuristicFn& heuristic);

#endif // A_STAR_H
