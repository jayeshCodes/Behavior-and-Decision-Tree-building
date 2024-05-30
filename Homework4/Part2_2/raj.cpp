#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include "includes/KinematicData.h"
#include <SFML/Graphics.hpp>
#include "includes/SteeringOutput.h"
#include "includes/SteeringBehavior.h"
// #include "PositionMatching.cpp"
#include "Wander.cpp"
// #include "OrientationMatching.cpp"
#include <deque>
#include <utility>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/graph_traits.hpp>
#include <cmath>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;

const double infinity = std::numeric_limits<double>::infinity();
#define INF INT_MAX

const sf::Vector2f defaultPosition(100.f, 355.f);
const sf::Vector2f spawnPosition1(25.f, 25.f);
const sf::Vector2f spawnPosition2(325.f, 25.f);
const sf::Vector2f spawnPosition3(25.f, 325.f);
const sf::Vector2f spawnPosition4(325.f, 325.f);
int spawnIndex = 1;

// Intialize Behavior Matching class
PositionMatching positionMatching;
OrientationMatching orientationMatching;
Wander wander;
sf::Vector2f wanderPoint = spawnPosition1;
float wanderOffset = 75.f;
int wanderRadius = 20; // max change in orientation
int wanderDistance = rand() % 50;

sf::RenderTexture renderTextureOG;

sf::Texture texture;
sf::Texture textureTarget;

sf::Texture texture6;
sf::Texture texture7;
sf::Texture texture8;
sf::Texture texture9;
sf::Texture texture10;
sf::Texture texture11;

sf::Clock deltaClock;
sf::Clock deltaClockWander;

sf::Vector2f getSpawnPosition()
{
    // Create a random number generator engine
    std::random_device rd;  // Used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()

    // Create a uniform distribution for integers between 1 and 4
    std::uniform_int_distribution<> distrib(1, 4);

    // Generate a random number
    spawnIndex = distrib(gen);
    cout << "\nspawnIndex : " << spawnIndex << "\n";

    if (spawnIndex == 1)
    {
        return spawnPosition1;
    }
    else if (spawnIndex == 2)
    {
        return spawnPosition2;
    }
    else if (spawnIndex == 3)
    {
        return spawnPosition3;
    }
    else
    {
        return spawnPosition4;
    }
}

float distanceWander(sf::Vector2f p1, sf::Vector2f p2)
{
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

// Define custom comparison function for pairs based on the second element
struct CompareSecond
{
    bool operator()(const pair<int, double> &left, const pair<int, double> &right) const
    {
        return left.second > right.second; // Min heap based on the second element
    }
};

// Attributes for room
class Room
{
public:
    int room_id;
    sf::Vector2f top_left, top_right, bottom_right, bottom_left;
    vector<sf::Vector2f> room_entry;
    int num_entries;

    Room(vector<sf::Vector2f> coordinates, int room_id, vector<sf::Vector2f> enteries)
    {
        this->top_left = coordinates[0];
        this->top_right = coordinates[1];
        this->bottom_right = coordinates[2];
        this->bottom_left = coordinates[3];
        this->room_id = room_id;
        for (auto re : enteries)
        {
            this->room_entry.push_back(re);
        }

        this->num_entries = room_entry.size();
    }
};

// Attributes for wall
class Wall : sf::RectangleShape
{
public:
    Wall()
    {
    }

    Wall(sf::Vector2f position, sf::Vector2f size, float rotation)
    {
        this->setFillColor(sf::Color::Red);
        this->setPosition(position);
        this->setSize(size);
        this->setRotation(rotation);
    }

    Wall(sf::Vector2f position, sf::Vector2f size, float rotation, sf::Color color)
    {
        this->setFillColor(color);
        this->setPosition(position);
        this->setSize(size);
        this->setRotation(rotation);
    }

    void draw(sf::RenderWindow *window)
    {
        window->draw(*this);
    }

    float get_rot()
    {
        return this->getRotation();
    }

private:
    void setPos(sf::Vector2f position)
    {
        this->setPosition(position);
    }

    sf::Vector2f get_pos()
    {
        return this->getPosition();
    }
};

class crumb : sf::CircleShape
{
public:
    crumb()
    {
        this->setRadius(2.f);
        srand(time(NULL));
        this->setFillColor(sf::Color(0, 0, 255, 255));
        this->setPosition(-100, -100);
    }

    // tell breadcrumb to render self, using current render window
    void draw(sf::RenderWindow *window)
    {
        window->draw(*this);
    }

    // set position of breadcrumb
    void drop(float x, float y)
    {
        this->setPosition(x, y);
    }

    // set position of breadcrumb
    void drop(sf::Vector2f position)
    {
        this->setPosition(position);
    }

    sf::Vector2f get()
    {
        return this->getPosition();
    }
};

deque<crumb> breadcrumbs;

bool mouseClicked = false;
// breadcrumbs
sf::Vector2f nextCrumbLocation = sf::Vector2f(0.f, 0.f);
int crumbCount = 0;
const int CRUMB_SIZE = 7;
sf::Clock deltaClockSM;

struct Vector2fHash
{
    std::size_t operator()(const sf::Vector2f &v) const
    {
        // Combine the hash values of x and y
        return std::hash<float>()(v.x) ^ (std::hash<float>()(v.y) << 1);
    }
};

// Custom equality operator for sf::Vector2f
struct Vector2fEqual
{
    bool operator()(const sf::Vector2f &v1, const sf::Vector2f &v2) const
    {
        // Compare x and y components for equality
        return v1.x == v2.x && v1.y == v2.y;
    }
};

// Define vertex properties for label
struct VertexProperties
{
    std::string label;
    double x;
    double y;
};

// Define the graph type with vertex and edge properties
typedef adjacency_list<vecS, vecS, directedS, VertexProperties, property<edge_weight_t, double>> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
typedef property_map<Graph, vertex_index_t>::type IndexMap;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;

// Custom property writer for vertex properties
struct VertexPropertyWriter
{
    const Graph &g;

    VertexPropertyWriter(const Graph &graph) : g(graph) {}

    template <class VertexDescriptor>
    void operator()(std::ostream &out, const VertexDescriptor &v) const
    {
        const auto &properties = boost::get(boost::vertex_bundle, g)[v];
        out << "[label=\"" << properties.label << "\", x=" << properties.x << ", y=" << properties.y << "]";
    }
};

// Custom property writer for edge properties
struct EdgePropertyWriter
{
    const Graph &g;

    EdgePropertyWriter(const Graph &graph) : g(graph) {}

    template <class EdgeDescriptor>
    void operator()(std::ostream &out, const EdgeDescriptor &e) const
    {
        out << "[weight=" << boost::get(boost::edge_weight, g)[e] << "]";
    }
};

sf::RenderWindow window(sf::VideoMode(500.f, 500.f), "HW4 Task 2");

vector<Wall> obstacles;

const int n = 50;
int tile_size = (window.getSize().x / n);

// convert window point to graph vertex coordinates
sf::Vector2f quantize(sf::Vector2f point)
{
    double tileX = floor(point.x / tile_size);
    double tileY = floor(point.y / tile_size);

    return sf::Vector2f(tileX, tileY);
}

// convert graph vertex coordinates to window point which is the center of the tile
sf::Vector2f localize(sf::Vector2f tile)
{
    double X = (tile.x * tile_size) + (tile_size / 2.f);
    double Y = (tile.y * tile_size) + (tile_size / 2.f);

    return sf::Vector2f(X, Y);
}

sf::Vector2f calculateTopLeft(sf::Vector2f point)
{
    int halfSide = tile_size / 2;
    int topLeftX = point.x - halfSide;
    int topLeftY = point.y - halfSide;
    return sf::Vector2f(topLeftX, topLeftY);
}

int getVertexFromTile(sf::Vector2f tile, Graph g)
{
    int numVertices = num_vertices(g);

    for (int i = 0; i < numVertices; i++)
    {
        VertexProperties vp = g[i];
        // std::cout << "Vertex " << i << ": (" << vp.x << ", " << vp.y << ") Label: " << vp.label << std::endl;
        if (vp.x == tile.x && vp.y == tile.y)
        {
            return i;
        }
    }

    return -1;
}

int manhattanDistance(int x1, int y1, int x2, int y2)
{
    return std::abs(x2 - x1) + std::abs(y2 - y1);
}

// creating graph
Graph g;

vector<int> astar(Graph &g, int sourceVertex, int targetVertex, int numVertices)
{

    // cout<<"\n"<<sourceVertex<<"("<<g[sourceVertex].label<<") ==> "<<targetVertex<<"("<<g[targetVertex].label<<")\n";

    vector<bool> visited(numVertices, false);
    vector<double> dist(numVertices, numeric_limits<double>::infinity());
    vector<double> heuristic(numVertices);
    vector<int> parent(numVertices, -1);

    int maxFringe = 1;
    int maxFill = 0;

    // Calculate heuristic for each node (in this case, Euclidean distance to the target)
    for (int i = 0; i < numVertices; ++i)
    {
        heuristic[i] = manhattanDistance(g[i].x, g[i].y, g[targetVertex].x, g[targetVertex].y);
        // cout<<"heuristic["<<i<<"] : "<<heuristic[i]<<"\n";
    }

    // cout<<"\n";

    priority_queue<pair<int, double>, vector<pair<int, double>>, CompareSecond> pq; // fringe

    dist[sourceVertex] = 0;

    pq.push(make_pair(sourceVertex, (dist[sourceVertex] + heuristic[sourceVertex])));

    while (!pq.empty())
    {
        int u = pq.top().first;
        int d = pq.top().second;
        pq.pop();
        maxFill++;

        if (u == targetVertex) // Target reached
            break;

        if (visited[u])
            continue;
        visited[u] = true;

        Graph::out_edge_iterator ei, ei_end;
        for (tie(ei, ei_end) = out_edges(vertex(u, g), g); ei != ei_end; ++ei)
        {
            Vertex s = source(*ei, g);
            Vertex v = target(*ei, g);
            double weight = get(edge_weight_t(), g, *ei);

            if (!visited[v] && dist[u] != numeric_limits<double>::infinity() && dist[u] + weight < dist[v])
            {
                dist[v] = dist[u] + weight;
                pq.push(make_pair(v, (dist[v] + heuristic[v])));
                parent[v] = u;
                maxFringe++;
            }
        }

        // if(maxFill >= 10){
        //     targetVertex = u;
        //     break;
        // }
    }

    stack<int> myStack;

    // cout<<"\nparent chain : ";
    int t = targetVertex;
    while (true)
    {
        // cout<<t<<"("<<heuristic[t]<<") "<<" <- ";
        if (parent[t] != -1)
        {
            myStack.push(t);
        }

        int p = parent[t];
        t = p;
        if (t == -1)
        {
            break;
        }
    }

    std::vector<int> res;

    // Copy elements from the stack to the vector
    while (!myStack.empty())
    {
        res.push_back(myStack.top());
        myStack.pop();
    }

    // cout<<"\n";
    // cout<<"Max Fringe Size (Open List yet to be visited): "<<maxFringe<<"\n";
    // cout<<"Max Fill size (Closed List already visited and shortest distance found) : "<<maxFill<<"\n";

    return res;
}

void loadGraph()
{
    // typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;

    //  loading the graph
    std::ifstream file("graph.dot");

    if (!file.is_open())
    {
        std::cerr << "Error opening file for reading!" << std::endl;
        return;
    }

    boost::dynamic_properties dp(boost::ignore_other_properties);
    dp.property("label", boost::get(&VertexProperties::label, g));
    dp.property("x", boost::get(&VertexProperties::x, g));
    dp.property("y", boost::get(&VertexProperties::y, g));
    dp.property("weight", boost::get(boost::edge_weight, g));
    boost::read_graphviz(file, g, dp);

    file.close();
}

void removeAllAdjacentEdges(int curr_vertex)
{
    typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;

    // remove all associated edges from this vertex to all and all vertices to this vertex as this tile will be the object
    // and the sprite can not reach this tile.

    std::pair<adjacency_iterator, adjacency_iterator> adjacentVertices = boost::adjacent_vertices(curr_vertex, g);

    // Iterate over the adjacent vertices and access corresponding edges
    for (auto it = adjacentVertices.first; it != adjacentVertices.second; ++it)
    {
        // Access the edge between vertex 'curr_vertex' and its neighbor (*it)
        std::pair<boost::graph_traits<Graph>::edge_descriptor, bool> edge = boost::edge(curr_vertex, *it, g);

        // Check if the edge exists
        if (edge.second)
        {
            // Access source, target, and weight of the edge
            Vertex s = boost::source(edge.first, g);
            Vertex t = boost::target(edge.first, g);

            remove_edge(s, t, g);
            remove_edge(t, s, g);
        }
    }
}

void getBoid(KinematicData &spriteData, string color, sf::Texture &texture)
{
    string image_name = "boid" + color + ".png";

    if (!texture.loadFromFile(image_name))
    {
        std::cout << "Error loading the texture!" << std::endl;
        return;
    }

    sf::Sprite newSprite;
    newSprite.setTexture(texture);
    newSprite.scale(0.02, 0.02);

    newSprite.setPosition(spriteData.getPosition());
    newSprite.setRotation(spriteData.getRotation());
    newSprite.setOrigin(spriteData.sprite.getOrigin());

    spriteData.setSprite(newSprite);
}

void getShark(KinematicData &spriteData, string color, sf::Texture &texture, bool reset = false)
{
    string image_name = "shark" + color + ".png";

    if (!texture.loadFromFile(image_name))
    {
        std::cout << "Error loading the texture!" << std::endl;
        return;
    }

    // spriteData.sprite.setTexture(texture);

    // sf::Sprite newSprite;
    // newSprite.setTexture(texture);
    // newSprite.scale(0.02, 0.02);

    // if(reset){
    //     spriteData.setPosition(defaultPosition);
    //     spriteData.setRotation(0.f);
    //     spriteData.sprite.setPosition(defaultPosition);
    //     spriteData.sprite.setRotation(0.f);
    // }
    // else{
    //     newSprite.setPosition(spriteData.sprite.getPosition());
    //     newSprite.setRotation(spriteData.sprite.getRotation());
    // }

    // // newSprite.setOrigin(spriteData.sprite.getOrigin());

    // spriteData.setSprite(newSprite);
    // // spriteData.setPosition(spriteData.sprite.getPosition());
}

sf::Sprite getCircle(sf::RenderTexture *renderTexture, sf::Color color)
{

    // Create a render texture
    renderTexture->create(15, 15);                // Create a 20x20 render texture
    renderTexture->clear(sf::Color::Transparent); // Clear the texture with transparent color
    // Create a circle shape
    sf::CircleShape circleShape(7.5); // Radius 10

    circleShape.setFillColor(sf::Color::Transparent);
    circleShape.setFillColor(color); // Set color to red
    // Set the position of the circle shape within the render texture
    circleShape.setPosition(0, 0); // Top-left corner of the render texture
    // circleShape.setOrigin(renderTexture->getSize().x / 2, renderTexture->getSize().y / 2);
    // Draw the circle shape onto the render texture
    renderTexture->draw(circleShape);
    // End drawing on the render texture
    renderTexture->display();
    // Create a sprite using the render texture's texture
    sf::Sprite circleSprite(renderTexture->getTexture());
    // circleSprite.setOrigin(circleSprite.getLocalBounds().width / 2.f, circleSprite.getLocalBounds().height / 2.f);

    return circleSprite;
}

float minDistance = 100.f;
float maxVelocity = 11.f;
int limit = 3;
int eatenSoFar = 0;

void seek_move(sf::RenderWindow *window, KinematicData *spriteData, KinematicData *finalMouseData, KinematicData *targetData, bool &targetReached, sf::Vector2f targetPosition)
{

    // cout<<"\n\n    ENV Data : \n";
    // cout << node1.distance100 << "," << node1.velocity35 << "+," << node1.islimitExceeded << "," << node1.targetReachedOrNot << "," << node1.isRoomEven << "\n";

    // node1.run(&*spriteData, &*targetData, targetPosition);

    // window->clear(sf::Color::White);

    // Get acceleration linear and angular
    SteeringOutput linearOutput = positionMatching.computeSteering(spriteData, targetData);
    SteeringOutput angularOutput = orientationMatching.computeSteering(spriteData, targetData);

    // int decision = getDecision(HEAD, spriteData, targetData, targetPosition);
    // cout << "===> Decision : " << decision << "\n\n";
    // executeDecision(decision, &*spriteData, &*targetData, &*window, targetReached);

    // linearOutput.getLinear()
    if (linearOutput.getLinear() == sf::Vector2f(0.0f, 0.0f))
    {
        targetReached = true;
        cout << "\ntargetReached : True\n";
        return;
    }

    SteeringOutput steeringOutput;
    steeringOutput.linear = linearOutput.getLinear();    // sf::Vector2f(0.0f, 0.0f);//linearOutput.getLinear();
    steeringOutput.angular = angularOutput.getAngular(); // angularOutput.getAngular();

    // calculate the time elapsed since last frame
    sf::Time deltaTimeClock = deltaClockSM.restart();
    float deltaTime = deltaTimeClock.asSeconds();

    // update sprite velocity and angular rotation to match target(mouse) using steering output
    spriteData->updateSpriteBySteeringOutput(steeringOutput, deltaTime);

    // update sprite postion and orientation
    spriteData->updateSpritePositionOrientation(deltaTime);

    // Breadcrumb logic
    if (sqrt(pow(spriteData->getPosition().x - nextCrumbLocation.x, 2.0) + pow(spriteData->getPosition().y - nextCrumbLocation.y, 2.0)) > 25)
    {
        crumb c;
        c.drop(nextCrumbLocation);
        nextCrumbLocation = spriteData->getPosition();

        if (crumbCount > CRUMB_SIZE)
        {
            breadcrumbs.pop_front();
            breadcrumbs.push_back(c);
        }
        else
        {
            breadcrumbs.push_back(c);
        }

        crumbCount++;
    }

    window->clear(sf::Color::White);

    for (auto x : breadcrumbs)
    {
        x.draw(window);
    }
    for (Wall ww : obstacles)
    {
        ww.draw(window);
    }

    window->draw(finalMouseData->getSprite());

    window->draw(spriteData->getSprite());

    window->display();
}

int getCurrentRoom(sf::Vector2f spritePosition)
{
    if (spritePosition.x >= 0 && spritePosition.x <= 215 && spritePosition.y >= 0 && spritePosition.y <= 255)
    {
        return 1;
    }
    else if (spritePosition.x >= 216 && spritePosition.x <= 500 && spritePosition.y >= 0 && spritePosition.y <= 255)
    {
        return 2;
    }
    else if (spritePosition.x >= 0 && spritePosition.x <= 265 && spritePosition.y >= 256 && spritePosition.y <= 500)
    {
        return 3;
    }
    else if (spritePosition.x >= 267 && spritePosition.x <= 500 && spritePosition.y >= 256 && spritePosition.y <= 500)
    {
        return 4;
    }

    return 0;
}

struct Task
{
    virtual bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) = 0;
};

class RootSelector : public Task
{
public:
    int targetReachedOrNot = 0, islimitExceeded = 0, distance100 = 0, velocity35 = 0, isRoomEven = 0;

    void writeEnvData(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition)
    {

        // write state of the environment to data.csv file

        // check if target reached or not
        if (mouseData->getPosition() == targetPosition && positionMatching.computeSteering(spriteData, mouseData).getLinear() == sf::Vector2f(0.0f, 0.0f))
        {
            targetReachedOrNot = 1;
        }
        else
        {
            targetReachedOrNot = 0;
        }

        // distance near
        float distance = manhattanDistance(spriteData->getPosition().x, spriteData->getPosition().y, targetPosition.x, targetPosition.y);
        if (distance <= minDistance)
        {
            distance100 = 1;
        }
        else
        {
            distance100 = 0;
        }

        // max Velocity
        float currVelocity = positionMatching.vectorLength(spriteData->velocity);
        if (currVelocity >= maxVelocity)
        {
            velocity35 = 1;
        }
        else
        {
            velocity35 = 0;
        }

        // room even
        int roomNumber = getCurrentRoom(spriteData->getPosition());
        if (roomNumber % 2 == 0)
        {
            isRoomEven = 1;
        }
        else
        {
            isRoomEven = 0;
        }

        // limit exceeded
        if (eatenSoFar >= limit)
        {
            islimitExceeded = 1;
        }
        else
        {
            islimitExceeded = 0;
        }

        // cout<<"\n\n    ENV Data : \n";
        // cout << distance100 << "," << velocity35 << "," << islimitExceeded << "," << targetReachedOrNot << "," << isRoomEven << "\n";

        // file << distance100 << "," << velocity35 << "," << islimitExceeded << "," << targetReachedOrNot << "," << isRoomEven << ",";
    }

    vector<Task *> tasks;

    void addTask(Task *task)
    {
        tasks.push_back(task);
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        this->writeEnvData(spriteData, mouseData, targetPosition);

        for (Task *task : tasks)
        {
            if (task->run(spriteData, mouseData, targetPosition))
            {
                return true; // Return true if any task succedd
            }
        }
        return false; // Return false if all tasks fail
    }
};

class Selector : public Task
{
public:
    Selector() {}

    vector<Task *> tasks;

    void addTask(Task *task)
    {
        tasks.push_back(task);
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        for (Task *task : tasks)
        {
            if (task->run(spriteData, mouseData, targetPosition))
            {
                return true; // Return true if any task succedd
            }
        }
        return false; // Return false if all tasks fail
    }
};

class limitDecorator : public Task
{
public:
    limitDecorator(int l)
    {
        limit = l;
    }

    vector<Task *> tasks;

    void addTask(Task *task)
    {
        tasks.push_back(task);
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        if (eatenSoFar >= limit)
        {
            // reset game
            spriteData->sprite.setPosition(defaultPosition);
            spriteData->setPosition(defaultPosition);
            getShark(*spriteData, "-grey", texture, true);

            mouseData->sprite.setTexture(textureTarget);
            wanderPoint = getSpawnPosition();
            mouseData->setPosition(wanderPoint);

            breadcrumbs.clear();
            nextCrumbLocation = sf::Vector2f(0.f, 0.f);
            crumbCount = 0;

            return false;
        }

        eatenSoFar += 1;

        for (Task *task : tasks)
        {
            if (task->run(spriteData, mouseData, targetPosition))
            {
                return true; // Return true if any task succedd
            }
        }

        return false; // Return false if all tasks fail
    }
};

class SequenceNode : public Task
{
public:
    SequenceNode() {}

    vector<Task *> tasks;

    void addTask(Task *task)
    {
        tasks.push_back(task);
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        for (Task *task : tasks)
        {
            if (!task->run(spriteData, mouseData, targetPosition))
            {
                return false; // Return false if any task fail
            }
        }
        return true; // Return true if all tasks succeed
    }
};

class RandomNode : public Task
{
public:
    vector<Task *> tasks;
    std::mt19937 generator{static_cast<std::mt19937::result_type>(std::time(nullptr))}; // Random number generator

    void addTask(Task *task)
    {
        tasks.push_back(task);
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        if (tasks.size() == 0)
        {
            cout << "There are no Tasks in TRandom";
            return false;
        }

        std::uniform_int_distribution<int> distribution(0, tasks.size() - 1);
        int randomIndex = distribution(generator);

        // Run the randomly selected task
        if (randomIndex >= 0 && randomIndex < tasks.size())
        {
            return tasks[randomIndex]->run(spriteData, mouseData, targetPosition);
        }
        else
        {
            std::cerr << "Invalid task index" << std::endl;
            return false;
        }
    }
};

class targetReached : public Task
{

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // target Reached
        // if (mouseData->getPosition() == targetPosition && positionMatching.computeSteering(spriteData, mouseData).getLinear() == sf::Vector2f(0.0f, 0.0f))
        // {
        //     return true;
        // }
        float dd = manhattanDistance(spriteData->getPosition().x, spriteData->getPosition().y, mouseData->getPosition().x, mouseData->getPosition().y);
        if (dd <= 14)
        {
            // update boid color
            cout << "Target is nearly REACHEEDDDD!!\n\n";
            return true;
        }
        return false; // Return false if target not reached
    }
};

class targetNotReached : public Task
{

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // target Reached
        // if (mouseData->getPosition() == targetPosition && positionMatching.computeSteering(spriteData, mouseData).getLinear() == sf::Vector2f(0.0f, 0.0f))
        // {
        //     return false;
        // }
        // return true; // Return true if target not reached
        float dd = manhattanDistance(spriteData->getPosition().x, spriteData->getPosition().y, mouseData->getPosition().x, mouseData->getPosition().y);
        if (dd > 14)
        {
            return true;
        }
        return false;
    }
};

class changeColor : public Task
{

    string color = "-grey";

public:
    changeColor(string color)
    {
        this->color = color;
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {

        getShark(*spriteData, color, texture);

        double mx = mouseData->getPosition().x;
        double my = mouseData->getPosition().y;

        sf::Vector2f target_tile = quantize(sf::Vector2f(mx, my)); // get the tile (x,y) where the mouse was clicked

        int target_vertex = getVertexFromTile(target_tile, g); // get the vertex number of the tile
        sf::Vector2f target_tile_center_point = localize(target_tile);

        double x = spriteData->getPosition().x;
        double y = spriteData->getPosition().y;

        sf::Vector2f source_tile = quantize(sf::Vector2f(x, y));
        int source_vertex = getVertexFromTile(source_tile, g);
        sf::Vector2f source_tile_center_point = localize(source_tile);

        vector<int> parent_path = astar(g, source_vertex, target_vertex, num_vertices(g));

        int t;
        if (parent_path.size() == 0)
        {
            return true;
        }
        else if (parent_path.size() == 1)
        {
            ;
            t = parent_path[0];
        }
        else
        {
            t = parent_path[1];
        }

        sf::Vector2f curr_target_tile = sf::Vector2f(g[t].x, g[t].y);   // tile coordinates
        sf::Vector2f curr_target_position = localize(curr_target_tile); // tile center point coordinates

        KinematicData targetData;
        targetData.setOrientation(0.f);
        targetData.setRotation(0.f);
        targetData.setPosition(curr_target_position);

        bool targetReached = false;
        deltaClockSM.restart();

        while (targetReached == false)
        {
            if (distanceWander(mouseData->getPosition(), wanderPoint) <= wanderOffset)
            {
                // cout<<"\n\nSpawnIndex :  "<<spawnIndex<<"\n";

                if (spawnIndex == 1)
                {
                    wanderPoint = sf::Vector2f(rand() % 180, rand() % 205);
                    if (wanderPoint.y <= 50)
                    {
                        wanderPoint.y = 50.f;
                    }
                    if (wanderPoint.x <= 50)
                    {
                        wanderPoint.x = 50;
                    }
                }
                else if (spawnIndex == 2)
                {
                    wanderPoint = sf::Vector2f(rand() % 460, rand() % 205);
                    if (wanderPoint.y <= 50)
                    {
                        wanderPoint.y = 50;
                    }
                    if (wanderPoint.x <= 340)
                    {
                        wanderPoint.x = 340.f;
                    }
                }
                else if (spawnIndex == 3)
                {

                    wanderPoint = sf::Vector2f(rand() % 180, rand() % 380);
                    if (wanderPoint.y <= 250)
                    {
                        wanderPoint.y = 250;
                    }
                    if (wanderPoint.x <= 50)
                    {
                        wanderPoint.x = 50.f;
                    }
                }
                else
                {

                    wanderPoint = sf::Vector2f(rand() % 460, rand() % 380);
                    if (wanderPoint.y <= 255)
                    {
                        wanderPoint.y = 255;
                    }
                    if (wanderPoint.x <= 320)
                    {
                        wanderPoint.x = 320.f;
                    }
                }
            }
            KinematicData targetWanderData;
            targetWanderData.setOrientation(0.f);
            targetWanderData.setPosition(wanderPoint);
            targetWanderData.setVelocity(sf::Vector2f(0.f, 0.f));
            targetWanderData.setRotation(0.f);

            SteeringOutput steeringOutput = wander.computeSteering(mouseData, &targetWanderData);

            sf::Time deltaTimeClockWander = deltaClockWander.restart();
            float deltaTimeWander = deltaTimeClockWander.asSeconds() / 10;

            // update sprite velocity and angular rotation to match target(mouse) using steering output
            mouseData->updateSpriteBySteeringOutput(steeringOutput, deltaTimeWander);

            // update sprite postion and orientation
            mouseData->updateSpritePositionOrientation(deltaTimeWander);



            seek_move(&window, spriteData, mouseData, &targetData, targetReached, target_tile_center_point);
        }

        return true;
    }
};

class targetNear : public Task
{
public:
    targetNear() {}

    targetNear(float d)
    {
        minDistance = d;
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // distance between target and monster
        float distance = manhattanDistance(spriteData->getPosition().x, spriteData->getPosition().y, targetPosition.x, targetPosition.y);
        if (distance <= minDistance)
        {
            // update boid color
            cout << "Target is near!!\n\n";
            return true;
        }
        return false; // Return false if target is not near the monster
    }
};

class maxVelocityReached : public Task
{
public:
    maxVelocityReached() {}

    maxVelocityReached(float maxV)
    {
        maxVelocity = maxV;
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // distance between target and monster
        float currVelocity = positionMatching.vectorLength(spriteData->velocity);
        cout << "\nCurrent Velocity :" << currVelocity << "\n\n";
        if (currVelocity >= maxVelocity)
        {
            cout << "Max Velocity Reached\n\n";
            return true;
        }
        return false; // Return false if target velocity is less than max Velocity
    }
};

class pathFind : public Task
{

public:
    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // file << 3 <<"\n";

        double mx = mouseData->getPosition().x;
        double my = mouseData->getPosition().y;

        sf::Vector2f target_tile = quantize(sf::Vector2f(mx, my)); // get the tile (x,y) where the mouse was clicked

        int target_vertex = getVertexFromTile(target_tile, g); // get the vertex number of the tile
        sf::Vector2f target_tile_center_point = localize(target_tile);

        double x = spriteData->getPosition().x;
        double y = spriteData->getPosition().y;

        sf::Vector2f source_tile = quantize(sf::Vector2f(x, y));
        int source_vertex = getVertexFromTile(source_tile, g);
        sf::Vector2f source_tile_center_point = localize(source_tile);

        vector<int> parent_path = astar(g, source_vertex, target_vertex, num_vertices(g));

        int t;
        if (parent_path.size() == 0)
        {
            return true;
        }
        else if (parent_path.size() == 1)
        {
            ;
            t = parent_path[0];
        }
        else
        {
            t = parent_path[1];
        }

        sf::Vector2f curr_target_tile = sf::Vector2f(g[t].x, g[t].y);   // tile coordinates
        sf::Vector2f curr_target_position = localize(curr_target_tile); // tile center point coordinates

        KinematicData targetData;
        targetData.setOrientation(0.f);
        targetData.setRotation(0.f);
        targetData.setPosition(curr_target_position);

        bool targetReached = false;
        deltaClockSM.restart();

        while (targetReached == false)
        {

            //wander target
            if (distanceWander(mouseData->getPosition(), wanderPoint) <= wanderOffset)
            {
                // cout<<"\n\nSpawnIndex :  "<<spawnIndex<<"\n";

                if (spawnIndex == 1)
                {
                    wanderPoint = sf::Vector2f(rand() % 180, rand() % 205);
                    if (wanderPoint.y <= 50)
                    {
                        wanderPoint.y = 50.f;
                    }
                    if (wanderPoint.x <= 50)
                    {
                        wanderPoint.x = 50;
                    }
                }
                else if (spawnIndex == 2)
                {
                    wanderPoint = sf::Vector2f(rand() % 460, rand() % 205);
                    if (wanderPoint.y <= 50)
                    {
                        wanderPoint.y = 50;
                    }
                    if (wanderPoint.x <= 340)
                    {
                        wanderPoint.x = 340.f;
                    }
                }
                else if (spawnIndex == 3)
                {

                    wanderPoint = sf::Vector2f(rand() % 180, rand() % 380);
                    if (wanderPoint.y <= 250)
                    {
                        wanderPoint.y = 250;
                    }
                    if (wanderPoint.x <= 50)
                    {
                        wanderPoint.x = 50.f;
                    }
                }
                else
                {

                    wanderPoint = sf::Vector2f(rand() % 460, rand() % 380);
                    if (wanderPoint.y <= 255)
                    {
                        wanderPoint.y = 255;
                    }
                    if (wanderPoint.x <= 320)
                    {
                        wanderPoint.x = 320.f;
                    }
                }
            }

            
            KinematicData targetWanderData;
            targetWanderData.setOrientation(0.f);
            targetWanderData.setPosition(wanderPoint);
            targetWanderData.setVelocity(sf::Vector2f(0.f, 0.f));
            targetWanderData.setRotation(0.f);

            SteeringOutput steeringOutput = wander.computeSteering(mouseData, &targetWanderData);

            sf::Time deltaTimeClockWander = deltaClockWander.restart();
            float deltaTimeWander = deltaTimeClockWander.asSeconds() / 10;

            // update sprite velocity and angular rotation to match target(mouse) using steering output
            mouseData->updateSpriteBySteeringOutput(steeringOutput, deltaTimeWander);

            // update sprite postion and orientation
            mouseData->updateSpritePositionOrientation(deltaTimeWander);

            //////////


            seek_move(&window, spriteData, mouseData, &targetData, targetReached, target_tile_center_point);
        }

        getShark(*spriteData, "-grey", texture);
        return true;
    }
};

class removeTarget : public Task
{

public:
    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // targetWall = Wall();
        sf::Texture blankTexture;
        mouseData->sprite.setTexture(blankTexture);
        return true;
    }
};

class roomEven : public Task
{
public:
    bool reverse = false;

    roomEven(bool r)
    {
        this->reverse = r;
    }

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        int roomNumber = getCurrentRoom(spriteData->getPosition());
        if (roomNumber % 2 == 0 && this->reverse == false)
        {
            return true;
        }
        else if (roomNumber % 2 != 0 && this->reverse == true)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

class victoryScript1 : public Task
{

public:
    sf::RenderWindow &window;

    victoryScript1(sf::RenderWindow &win) : window(win) {}

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {
        // file << 4 <<"\n";

        int count = 0;
        while (true)
        {
            if (count >= 15)
            {
                break;
            }

            // window->clear(sf::Color::White);

            if (count % 2 == 0)
            {
                getShark(*spriteData, "-green", texture);
            }
            else
            {
                getShark(*spriteData, "-red", texture);
            }

            window.clear(sf::Color::White);

            window.draw(spriteData->getSprite());

            window.display();

            // sleep for one second
            sf::sleep(sf::milliseconds(200));

            count++;
        }

        spriteData->sprite.setPosition(defaultPosition);
        spriteData->setPosition(defaultPosition);
        getShark(*spriteData, "-grey", texture, true);

        // reset target
        mouseData->sprite.setTexture(textureTarget);
        wanderPoint = getSpawnPosition();
        mouseData->setPosition(wanderPoint);
        // mouseData->sprite.setPosition(sf::Vector2f(25.f, 25.f));

        breadcrumbs.clear();
        nextCrumbLocation = sf::Vector2f(0.f, 0.f);
        crumbCount = 0;

        return true;
    }
};

class victoryScript2 : public Task
{

public:
    sf::RenderWindow &window;

    victoryScript2(sf::RenderWindow &win) : window(win) {}

    bool run(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition) override
    {

        // file << 5 <<"\n";

        const float totalRotation = 360.f;                    // Total rotation angle
        const int numFrames = 5;                              // Number of frames for the animation
        const float rotationStep = totalRotation / numFrames; // Rotation angle for each frame

        // while (spriteData->sprite.getRotation() != 355.f)
        for (int i = 0; i < rotationStep; i++)
        {
            // Rotate the sprite
            // spriteData->setRotation(rotationStep);
            spriteData->sprite.rotate(5.f);

            // Clear the window
            window.clear(sf::Color::White);

            // Draw the rotated sprite
            window.draw(spriteData->sprite);

            // Display the window
            window.display();

            // Pause for a short duration to observe the rotation
            sf::sleep(sf::milliseconds(20)); // Adjust the duration as needed (1000 ms / 60 frames)
        }

        spriteData->sprite.setPosition(defaultPosition);
        spriteData->setPosition(defaultPosition);
        getShark(*spriteData, "-grey", texture, true);

        mouseData->sprite.setTexture(textureTarget);
        wanderPoint = getSpawnPosition();
        mouseData->setPosition(wanderPoint);
        // mouseData->sprite.setPosition(sf::Vector2f(25.f, 25.f));

        breadcrumbs.clear();
        nextCrumbLocation = sf::Vector2f(0.f, 0.f);
        crumbCount = 0;

        return true;
    }
};

RootSelector node1 = RootSelector();
SequenceNode node2 = SequenceNode();
SequenceNode node3 = SequenceNode();
targetNotReached node4 = targetNotReached();
Selector node5 = Selector();
targetReached node6 = targetReached();
removeTarget node7 = removeTarget();

limitDecorator node8 = limitDecorator(3);

SequenceNode node9 = SequenceNode();
SequenceNode node10 = SequenceNode();
pathFind node11 = pathFind();

SequenceNode node12 = SequenceNode();
SequenceNode node13 = SequenceNode();

targetNear node14 = targetNear();
changeColor node15 = changeColor("-cyan");
maxVelocityReached node16 = maxVelocityReached();
changeColor node17 = changeColor("-purple");

roomEven node18 = roomEven(false); // checks if room number is even
victoryScript1 node19 = victoryScript1(window);
roomEven node20 = roomEven(true); // checks if room number is odd
victoryScript2 node21 = victoryScript2(window);

void buildBehaviorTree()
{

    node1.addTask(&node2);
    node1.addTask(&node3);

    node2.addTask(&node4);
    node2.addTask(&node5);

    node3.addTask(&node6);
    node3.addTask(&node8);

    node5.addTask(&node9);
    node5.addTask(&node10);
    node5.addTask(&node11);

    node8.addTask(&node12);
    node8.addTask(&node13);

    node9.addTask(&node14);
    node9.addTask(&node15);

    node10.addTask(&node16);
    node10.addTask(&node17);

    node12.addTask(&node18);
    node12.addTask(&node7);
    node12.addTask(&node19);

    node13.addTask(&node20);
    node13.addTask(&node7);
    node13.addTask(&node21);
}

class DecisionNode
{
public:
    int id;
    int value;
    DecisionNode *left;
    DecisionNode *right;

    DecisionNode()
    {
        this->right = nullptr;
        this->left = nullptr;
        this->id = 0;
        this->value = -1;
    }

    DecisionNode(int id)
    {
        this->right = nullptr;
        this->left = nullptr;
        this->id = id;
    }

    DecisionNode(int id, int x)
    {
        this->right = nullptr;
        this->left = nullptr;
        this->id = id;
        this->value = x;
    }

    DecisionNode(int id, int x, DecisionNode *left, DecisionNode *right)
    {
        this->right = right;
        this->left = left;
        this->id = id;
        this->value = x;
    }

    void setLeft(DecisionNode *leftNode)
    {
        left = leftNode;
    }

    void setRight(DecisionNode *rightNode)
    {
        right = rightNode;
    }
};

// createa and return the head of decision tree
DecisionNode *getDecisionTreeHead()
{

    DecisionNode *head = new DecisionNode(1);    // target reached?
    DecisionNode *node2 = new DecisionNode(2);   // distance to target < 20
    DecisionNode *node3 = new DecisionNode(3);   // Room No. odd?
    DecisionNode *node4 = new DecisionNode(4);   // Velocity MAX?
    DecisionNode *node5 = new DecisionNode(5);   // Room No. even?
    DecisionNode *node6 = new DecisionNode(6);   // [change colors]
    DecisionNode *node7 = new DecisionNode(7);   // [move to default position]
    DecisionNode *node8 = new DecisionNode(8);   // [pathfind to target]
    DecisionNode *node9 = new DecisionNode(9);   // [color to yellow]
    DecisionNode *node10 = new DecisionNode(10); // [color to red]
    DecisionNode *node11 = new DecisionNode(11); // [color to green]

    head->left = node2;
    head->right = node3;

    node2->left = node4;
    node2->right = node5;

    node3->left = node6;
    node3->right = node7;

    node4->left = node8;
    node4->right = node9;

    node5->left = node10;
    node5->right = node11;

    return head;
}

// function to get the state of the env respective to each decision Node
int *getData(DecisionNode *HEAD, KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition)
{

    int *data = new int[6];

    // target reached
    if (mouseData->getPosition() == targetPosition && positionMatching.computeSteering(spriteData, mouseData).getLinear() == sf::Vector2f(0.0f, 0.0f))
    {
        data[1] = 1;
    }
    else
    {
        data[1] = 0;
    }

    // distance <= 35
    float distance = manhattanDistance(spriteData->getPosition().x, spriteData->getPosition().y, targetPosition.x, targetPosition.y);
    if (distance <= 100)
    {
        data[2] = 1;
    }
    else
    {
        data[2] = 0;
    }

    // room odd or even
    int roomNo = getCurrentRoom(spriteData->getPosition());
    if (roomNo % 2 == 0)
    {
        data[3] = 0;
        data[5] = 1;
    }
    else
    {
        data[3] = 1;
        data[5] = 0;
    }

    // max velocity = 35
    float currVelocity = positionMatching.vectorLength(spriteData->velocity);
    if (currVelocity >= 35)
    {
        data[4] = 1;
    }
    else
    {
        data[4] = 0;
    }

    return data;
}

// get decision from deicison tree
int getDecision(DecisionNode *HEAD, KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition)
{

    int *data = getData(HEAD, spriteData, mouseData, targetPosition);

    while (HEAD->left != nullptr && HEAD->right != nullptr)
    {
        int id = HEAD->id;
        if (data[id] == 1)
        {
            HEAD = HEAD->right;
        }
        else
        {
            HEAD = HEAD->left;
        }
    }

    return HEAD->id;
}

// change behavior according to decision
void executeDecision(int decision, KinematicData *spriteData, KinematicData *mouseData, sf::RenderWindow *window, bool &targetReached)
{
    switch (decision)
    {
    case 6:
    {
        cout << "===> ExecuteDecision : 6 \n\n";

        int count = 0;
        while (true)
        {
            if (count >= 15)
            {
                break;
            }

            // window->clear(sf::Color::White);
            sf::RenderTexture renderTexture;

            if (count % 2 == 0)
            {
                getBoid(*spriteData, "-green", texture6);
            }
            else
            {
                getBoid(*spriteData, "-red", texture6);
            }

            window->draw(spriteData->getSprite());

            window->display();

            // sleep for one second
            sf::sleep(sf::milliseconds(200));

            count++;
        }

        // spriteData->sprite.setTexture(renderTextureOG.getTexture());
        getBoid(*spriteData, "", texture);

        break;
    }
    case 7:
    {
        cout << "\nDecision is 7\n";
        sf::sleep(sf::milliseconds(500));
        spriteData->setPosition(defaultPosition);
        targetReached = true;
        break;
    }
    case 8:
    {
        // load texture

        // string image_name = "boid.png";
        // if (!texture8.loadFromFile(image_name))
        // {
        //     std::cout << "Error loading the texture!" << std::endl;
        //     return;
        // }

        // sf::Sprite newSprite;
        // newSprite.setTexture(texture8);
        // newSprite.scale(0.02, 0.02);
        // newSprite.setPosition(spriteData->getPosition());
        // newSprite.setRotation(spriteData->getRotation());
        // newSprite.setOrigin(spriteData->sprite.getOrigin());

        // spriteData->setSprite(newSprite);

        getBoid(*spriteData, "", texture8);

        break;
    }
    case 9:
    {
        // sf::RenderTexture renderTexture;
        getBoid(*spriteData, "-purple", texture9);
        break;
    }
    case 10:
    {
        // sf::RenderTexture renderTexture;
        getBoid(*spriteData, "-blue", texture10);
        break;
    }
    case 11:
    {
        // sf::RenderTexture renderTexture;
        getBoid(*spriteData, "-cyan", texture11);
        break;
    }

    default:
        cout << "\n\nDEFAULT CASE\n\n";
        break;
    }
}

void testOnlySeek(KinematicData *spriteData, KinematicData *mouseData, sf::Vector2f targetPosition, int &t)
{
    // file << 3 <<"\n";

    double mx = mouseData->getPosition().x;
    double my = mouseData->getPosition().y;

    sf::Vector2f target_tile = quantize(sf::Vector2f(mx, my)); // get the tile (x,y) where the mouse was clicked

    int target_vertex = getVertexFromTile(target_tile, g); // get the vertex number of the tile
    sf::Vector2f target_tile_center_point = localize(target_tile);

    double x = spriteData->getPosition().x;
    double y = spriteData->getPosition().y;

    sf::Vector2f source_tile = quantize(sf::Vector2f(x, y));
    int source_vertex = getVertexFromTile(source_tile, g);
    sf::Vector2f source_tile_center_point = localize(source_tile);

    vector<int> parent_path = astar(g, source_vertex, target_vertex, num_vertices(g));

    // int t = 0;
    if (parent_path.size() <= 2)
    {
        // t = -1;
        // cout<<"\n\n\n PARENT PATH IS ZEROOOOOOOOOOOOOOOOOOOOOOO000000000000\n\n";
        // return;
        t = parent_path[0];
    }
    else
    {
        t = parent_path[1];
    }

    // t = parent_path[1];

    sf::Vector2f curr_target_tile = sf::Vector2f(g[t].x, g[t].y);   // tile coordinates
    sf::Vector2f curr_target_position = localize(curr_target_tile); // tile center point coordinates

    KinematicData targetData;
    targetData.setOrientation(0.f);
    targetData.setRotation(0.f);
    targetData.setPosition(curr_target_position);

    bool targetReached = false;
    deltaClockSM.restart();

    while (targetReached == false)
    {
        seek_move(&window, spriteData, mouseData, &targetData, targetReached, target_tile_center_point);
    }
}

int main()
{

    typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;

    // filling the graph g
    loadGraph();

    buildBehaviorTree();

    /// loading obstacles
    int numVertices = num_vertices(g);
    std::cout << "Total Vertices : " << numVertices << "\n";

    sf::Color wallColor = sf::Color::White;

    // adding transparent wall aroung obstacle wall
    for (int i = 0; i < numVertices; i++)
    {
        VertexProperties vp = g[i];
        if (vp.label == "obstacle")
        {
            // std::cout << "Obstacles Vertex " << i << ": (" << vp.x << ", " << vp.y << ") Label: " << vp.label << std::endl;

            sf::Vector2f obs_tile(vp.x, vp.y);
            sf::Vector2f obs_tile_center_point = localize(obs_tile);
            sf::Vector2f top_left_point = calculateTopLeft(obs_tile_center_point);
            // Wall w(top_left_point, sf::Vector2f(tile_size, tile_size), 0.f);

            // add a transparent wall aroung the obstacle so that the sprite does not overlap the obstacle wall
            if (top_left_point.x - 10 >= 0)
            {
                // left tile
                sf::Vector2f leftTileTopLeftPoint = sf::Vector2f(top_left_point.x - 10, top_left_point.y);
                Wall wleft(leftTileTopLeftPoint, sf::Vector2f(tile_size, tile_size), 0.f, wallColor);

                int xx = vp.x - 1;
                int yy = vp.y;

                int curr_vertex = -1;

                for (int vv = 0; vv < numVertices; vv++)
                {
                    if (g[vv].x == xx && g[vv].y == yy)
                    {
                        curr_vertex = vv;
                        break;
                    }
                }

                // remove all associated edges from this vertex to all and all vertices to this vertex as this tile will be the object
                // and the sprite can not reach this tile.

                removeAllAdjacentEdges(curr_vertex);

                obstacles.push_back(wleft);
            }
            if (top_left_point.y - 10 >= 0)
            {
                // up tile
                sf::Vector2f upTileTopLeftPoint = sf::Vector2f(top_left_point.x, top_left_point.y - 10);
                Wall wup(upTileTopLeftPoint, sf::Vector2f(tile_size, tile_size), 0.f, wallColor);

                int xx = vp.x;
                int yy = vp.y - 1;

                int curr_vertex = -1;

                for (int vv = 0; vv < numVertices; vv++)
                {
                    if (g[vv].x == xx && g[vv].y == yy)
                    {
                        curr_vertex = vv;
                        break;
                    }
                }

                // remove all associated edges from this vertex to all and all vertices to this vertex as this tile will be the object
                // and the sprite can not reach this tile.

                removeAllAdjacentEdges(curr_vertex);

                obstacles.push_back(wup);
            }
            if (top_left_point.x + 10 < window.getSize().x)
            {
                // right tile
                sf::Vector2f rightTileTopLeftPoint = sf::Vector2f(top_left_point.x + 10, top_left_point.y);
                Wall wright(rightTileTopLeftPoint, sf::Vector2f(tile_size, tile_size), 0.f, wallColor);

                int xx = vp.x + 1;
                int yy = vp.y;

                int curr_vertex = -1;

                for (int vv = 0; vv < numVertices; vv++)
                {
                    if (g[vv].x == xx && g[vv].y == yy)
                    {
                        curr_vertex = vv;
                        break;
                    }
                }

                // remove all associated edges from this vertex to all and all vertices to this vertex as this tile will be the object
                // and the sprite can not reach this tile.

                removeAllAdjacentEdges(curr_vertex);

                obstacles.push_back(wright);
            }
            if (top_left_point.y + 10 < window.getSize().y)
            {
                // down tile
                sf::Vector2f downTileTopLeftPoint = sf::Vector2f(top_left_point.x, top_left_point.y + 10);
                Wall wdown(downTileTopLeftPoint, sf::Vector2f(tile_size, tile_size), 0.f, wallColor);

                int xx = vp.x;
                int yy = vp.y + 1;

                int curr_vertex = -1;

                for (int vv = 0; vv < numVertices; vv++)
                {
                    if (g[vv].x == xx && g[vv].y == yy)
                    {
                        curr_vertex = vv;
                        break;
                    }
                }

                // remove all associated edges from this vertex to all and all vertices to this vertex as this tile will be the object
                // and the sprite can not reach this tile.

                removeAllAdjacentEdges(curr_vertex);

                obstacles.push_back(wdown);
            }

            // obstacles.push_back(w);
        }
    }

    for (int i = 0; i < numVertices; i++)
    {
        VertexProperties vp = g[i];
        if (vp.label == "obstacle")
        {
            // std::cout << "Obstacles Vertex " << i << ": (" << vp.x << ", " << vp.y << ") Label: " << vp.label << std::endl;

            sf::Vector2f obs_tile(vp.x, vp.y);
            sf::Vector2f obs_tile_center_point = localize(obs_tile);
            sf::Vector2f top_left_point = calculateTopLeft(obs_tile_center_point);
            Wall w(top_left_point, sf::Vector2f(tile_size, tile_size), 0.f);

            obstacles.push_back(w);
        }
    }

    cout << "\n\n";

    // loading the decision tree
    DecisionNode *HEAD = getDecisionTreeHead();

    // load texture
    if (!texture.loadFromFile("shark-grey.png"))
    {
        std::cout << "Error loading the texture!" << std::endl;
        return -1;
    }

    // load sprite
    sf::Sprite spriteTemp;
    spriteTemp.setTexture(texture);
    spriteTemp.scale(0.015, 0.015);

    spriteTemp.setPosition(defaultPosition);
    spriteTemp.setOrigin(spriteTemp.getLocalBounds().width / 2, spriteTemp.getLocalBounds().height / 2);
    sf::FloatRect bounds = spriteTemp.getLocalBounds();

    // initialize sprite data
    KinematicData spriteData(spriteTemp);
    spriteData.setPosition(spriteTemp.getPosition());
    spriteData.setVelocity(sf::Vector2f(0.f, 0.f));
    spriteData.setOrientation(0.f); // degrees
    spriteData.setRotation(0.f);    // degrees
    // spriteData.sprite.setColor(sf::Color::Red);

    // load target texture
    if (!textureTarget.loadFromFile("circle.png"))
    {
        std::cout << "Error loading the texture!" << std::endl;
        return -1;
    }

    sf::Sprite spriteTarget;
    spriteTarget.setTexture(textureTarget);
    spriteTarget.setPosition(spawnPosition1);
    spriteTarget.scale(0.015, 0.015);
    float centerx = (spriteTarget.getLocalBounds().width / 2);
    float centery = (spriteTarget.getLocalBounds().height / 2);
    spriteTarget.setOrigin(sf::Vector2f(centerx, centery));

    KinematicData mouseData(spriteTarget);
    mouseData.setPosition(spriteTarget.getPosition());
    mouseData.setOrientation(0.f);
    mouseData.setRotation(0.f);

      

    deltaClock.restart();
    deltaClockWander.restart();
    while (window.isOpen())
    {

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                cout << "\n\n\nClosing....\n\n\n";
                // file.close();
                window.close();
            }

            // if (event.type == sf::Event::MouseButtonPressed)
            // {
            //     if (event.mouseButton.button == sf::Mouse::Left)
            //     {
            //         sf::Vector2f mousePosition = sf::Vector2f(event.mouseButton.x, event.mouseButton.y);
            //         cout << "Mouse Clicked on position x : " << mousePosition.x << "  y : " << mousePosition.y << "\n\n";
            //         mouseData.setPosition(mousePosition);
            //         mouseClicked = true;
            //
            //         sf::Vector2f tile = quantize(sf::Vector2f(mousePosition.x, mousePosition.y));
            //         int target_tile_number = getVertexFromTile(tile, g); // get the vertex number of the tile
            //         sf::Vector2f tile_center_point = localize(tile);
            //
            //         sf::Vector2f top_left_point = calculateTopLeft(tile_center_point);
            //         Wall w(top_left_point, sf::Vector2f(tile_size, tile_size), 0.f, sf::Color::Green);
            //
            //         // spriteData.setPosition(defaultPosition);
            //         breadcrumbs.clear();
            //
            //
            //     }
            // }
        }

        // BEHAVIOR TREE RUN
        deltaClockSM.restart();
        node1.run(&spriteData, &mouseData, mouseData.getPosition());

        window.clear(sf::Color::White);

        for (Wall ww : obstacles)
        {
            ww.draw(&window);
        }

        window.draw(mouseData.getSprite());

        window.draw(spriteData.getSprite());

        window.display();

    }

    return 0;
}
