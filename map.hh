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
Path quickest_path(position from, position to, int turn_limit = 3*NB_POINTS_ACTION);

direction opposite(direction dir);
direction turn_trigo(direction dir);
bool can_push_toward(position pos, direction dir);

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

struct Threat
{
    position adv_pos;  /* <- Position de l'adversaire */
    int value;  /* <- Valeur de l'élimination de la menace */
    position ally_pos; /* <- Position de l'allié */
    //ThreatAxis threat_axis; /* <- Position & direction de la menace */
};

/// Liste les axes de menaces depuis une case
std::vector<ThreatAxis> pos_threats_axies(position pos);

/// Liste les menaces de l'adversaire sur une case
std::vector<position> list_current_threats(position origin);

/// Ajoute les menaces en tant que faux aliens dans la liste
std::vector<Threat> compute_threats(const std::vector<alien_info>& aliens);

#endif // MAP_HH
