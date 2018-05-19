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
const direction DIR[] = {NORD, OUEST, SUD, EST};

position dir_to_vec(direction dir);
bool is_empty(position pos);
position glide_dest(position from, direction dir);
erreur perform_move(int id_agent, Move move);
int dist(position a, position b);
Path quickest_path(position from, position to);

direction opposite(direction dir);
bool can_push_toward(position pos, direction dir);

// Plus un alien est proche de la capture plus il vaut "cher"
int alien_score(alien_info alien);

#endif // MAP_HH
