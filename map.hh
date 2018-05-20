#ifndef MAP_HH
#define MAP_HH

#include "prologin.hh"
#include <deque>

struct Move
{
    action_type typ;
    direction dir;
};

const position INVALID_POS = position{-1,-1};
const direction DIR[] = {NORD, OUEST, SUD, EST};

const int MOI = 0b01;
const int ADV = 0b10;

position dir_to_vec(direction dir);
bool is_empty(position pos, int view_mask);
int opponent(int player);
position glide_dest(position from, direction dir, int view_mask);
erreur perform_move(int id_agent, Move move);
int dist(position a, position b);

direction opposite(direction dir);
direction turn_trigo(direction dir);
bool can_push_toward(position pos, direction dir, int view_mask);

// Plus un alien est proche de la capture plus il vaut "cher"
int alien_score(alien_info alien);
// Prendre en compte la "défendabilité" de l'alien
double alien_def_score(alien_info alien);

direction find_dir(position origin, position target);

struct ThreatAxis
{
    position push_pos;
    direction dir;
};

/// Liste les axes de menaces depuis une case
std::vector<ThreatAxis> pos_threats_axies(position pos);

/// Compte le nombre de menaces de l'adversaire sur une case
int count_threats(position origin, int view_mask);

struct AttackInfo
{
    position target;
    double score;
    position attack_pos;
};

AttackInfo best_opponent_attack(int opponent_id, int view_mask);

#endif // MAP_HH
