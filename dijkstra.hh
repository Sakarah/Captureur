#ifndef DIJKSTRA_HH
#define DIJKSTRA_HH

#include "map.hh"

struct Path
{
    int cost;
    std::deque<Move> path;
};

class Dijkstra
{
public:
    Dijkstra(position start, int action_limit = 20*NB_POINTS_ACTION, int adv = adversaire());
    Path quickest_path(position to);
private:
    position from;
    position prev[TAILLE_BANQUISE][TAILLE_BANQUISE];
    int adv;
};

extern std::vector<Dijkstra> my_dijkstra;
extern std::vector<Dijkstra> opp_dijkstra;

void update_static_tabs();

#endif // DIJKSTRA_HH
