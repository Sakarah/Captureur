#ifndef MAP_HH
#define MAP_HH

#include "prologin.hh"
#include <deque>

struct Move
{
    action_type typ;
    direction dir;
};

struct Path
{
    int cost;
    std::deque<Move> path;
};

const position INVALID_POS = position{-1,-1};

position dir_to_vec(direction dir);
bool is_empty(position pos);
position glide_dest(position from, direction dir);
erreur perform_move(int id_agent, Move move);
Path quickest_path(position from, position to);

#endif // MAP_HH
