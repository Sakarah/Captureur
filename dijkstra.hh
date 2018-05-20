#ifndef DIJKSTRA_HH
#define DIJKSTRA_HH

#include "map.hh"

struct Path
{
    int cost;
    std::deque<Move> path;
};

Path quickest_path(position from, position to, int view_mask,
                   int turn_limit = 5*NB_POINTS_ACTION, int adversaire = adversaire());

#endif // DIJKSTRA_HH
