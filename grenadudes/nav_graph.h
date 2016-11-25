#pragma once

// nodes used for pathfinding & functions to generate graphs of them

#include <SFML/Graphics.hpp>
#include <vector>

class TerrainGrid;

class NavNode {
public:
    NavNode();
    NavNode(sf::Vector2f worldPosition);

    sf::Vector2f worldPosition;
    
    std::vector<NavNode*> walkingEdge;
    std::vector<NavNode*> jumpingEdge;
    std::vector<NavNode*> fallingEdge;

    // pathfinding (recalculated every time a node is visited)
    bool flag;
    bool visited;
    NavNode *parent;
    bool jump;
    int g, h;
    
    // debug
    bool marked;
private:
};

// no need for a class
// actually maybe a class would have been good..
typedef std::vector<NavNode> NavGraph;
NavGraph generate_nav_graph_nodes(TerrainGrid &grid);
void generate_nav_graph_edges(NavGraph &graph, TerrainGrid &grid);
void draw_nav_graph(sf::RenderWindow &window, NavGraph &navGraph);
NavNode *get_closest_node(NavGraph &navGraph, sf::Vector2f position);
bool find_path(NavNode *start, NavNode *goal, NavGraph navGraph, std::vector<sf::Vector3f> *path); // marks nodes
bool _find_path(NavNode *satrt, NavNode *current, NavNode *goal, std::vector<sf::Vector3f> *path);
std::vector<sf::Vector2i> get_line(sf::Vector2i start, sf::Vector2i end);