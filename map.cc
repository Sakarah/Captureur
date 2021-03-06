#include "dijkstra.hh"

position dir_to_vec(direction dir)
{
    if(dir == NORD) return position{-1, 0};
    if(dir == SUD)  return position{+1,0};
    if(dir == OUEST) return position{0,-1};
    if(dir == EST) return position{0,+1};

    return position{0,0};
}

bool is_empty(position pos)
{
    return type_case(pos) == LIBRE && agent_sur_case(pos) == -1;
}

/// Retrun the position reached with a glide in the specified direction
position glide_dest(position from, direction dir)
{
    position dir_vec = dir_to_vec(dir);
    while(true)
    {
        position next = from + dir_vec;
        if(!is_empty(next)) return from;
        from = next;
    }
}

erreur perform_move(int id_agent, Move move)
{
    switch(move.typ)
    {
    case ACTION_DEPLACER:
        return deplacer(id_agent, move.dir);
    case ACTION_GLISSER:
        return glisser(id_agent, move.dir);
    case ACTION_POUSSER:
        return pousser(id_agent, move.dir);
    }

    return ACTION_INVALIDE;
}

int dist(position a, position b)
{
    return std::abs(a.ligne - b.ligne) + std::abs(a.colonne - b.colonne);
}

direction opposite(direction dir)
{
    if(dir == NORD) return SUD;
    else if(dir == SUD) return NORD;
    else if(dir == OUEST) return EST;
    else if(dir == EST) return OUEST;
    return INVALIDE;
}

direction turn_trigo(direction dir)
{
    if(dir == NORD) return OUEST;
    else if(dir == OUEST) return SUD;
    else if(dir == SUD) return EST;
    else if(dir == EST) return NORD;
    return INVALIDE;
}

bool can_push_toward(position pos, direction dir)
{
    position obstacle = pos + dir_to_vec(dir);
    return is_empty(obstacle);
}


int alien_score(alien_info alien)
{
    return alien.points_capture * (1+alien.duree_invasion);
}

const int NB_POSSIBLE_THREATS = 8;
const int NB_NORMAL_THREATS = 4;

// TODO: Vérifier la pertinence de cette approche
double alien_def_score(alien_info alien)
{
    // TODO: precompute this
    //                                     vvvvvvvvvvvvvvvvvvvvvvvvvvvv
    std::vector<ThreatAxis> threat_axies = pos_threats_axies(alien.pos);
    int score = alien_score(alien);
    if(threat_axies.size() < 4)
    {
        score *= (1+(4-threat_axies.size())/4);
    }
    return score;
}

direction find_dir(position origin, position target)
{
    if(origin.ligne == target.ligne)
    {
        if(origin.colonne < target.colonne) return EST;
        else return OUEST;
    }
    else if(origin.colonne == target.colonne)
    {
        if(origin.ligne < target.ligne) return SUD;
        else return NORD;
    }
    return INVALIDE;
}

std::vector<ThreatAxis> pos_threats_axies(position origin)
{
    std::vector<ThreatAxis> axies;
    for(direction dir : DIR)
    {
        if(!can_push_toward(origin, opposite(dir))) continue;

        ThreatAxis threat;
        threat.push_pos = origin + dir_to_vec(dir);

        if(type_case(threat.push_pos) != LIBRE) continue;
        threat.dir = dir;
        axies.push_back(threat);

        // Add attack axies using nearby walls
        direction turned_dir = turn_trigo(dir);
        position left = threat.push_pos + dir_to_vec(turned_dir);
        position right = threat.push_pos + dir_to_vec(opposite(turned_dir));
        if(type_case(left) != LIBRE)
        {
            if(type_case(right) == LIBRE)
            {
                threat.dir = opposite(turned_dir);
                axies.push_back(threat);
            }
        }
        else
        {
            if(type_case(right) != LIBRE)
            {
                threat.dir = turned_dir;
                axies.push_back(threat);
            }
        }
    }
    return axies;
}

int count_threats(position origin)
{
    int threats = 0;
    for(int opp_id = 0; opp_id < NB_AGENTS; opp_id++)
    {
        position opponent_pos = position_agent(adversaire(), opp_id);
        if(alien_sur_case(opponent_pos)) continue;

        bool threat = false;

        for(direction dir : DIR)
        {
            if(!can_push_toward(origin, dir)) continue;
            position attack_pos = origin + dir_to_vec(opposite(dir));
            if(!is_empty(attack_pos) && opponent_pos != attack_pos) continue;

            Path p = quickest_path(opponent_pos, attack_pos, NB_POINTS_ACTION-COUT_POUSSER, moi());
            if(p.cost <= NB_POINTS_ACTION-COUT_POUSSER) threat = true;
        }

        if(threat) threats++;
    }
    return threats;
}

AttackInfo best_opponent_attack(int opponent_id)
{
    AttackInfo best_attack;
    best_attack.score = 0;
    for(int ally_id = 0 ; ally_id < NB_AGENTS ; ally_id++)
    {
        position opponent_pos = position_agent(adversaire(), opponent_id);
        position ally_pos = position_agent(moi(), ally_id);

        if(!alien_sur_case(ally_pos)) continue;
        alien_info alien = info_alien(ally_pos);

        int tour_depart = alien.tour_invasion + alien.duree_invasion;
        int turns_before_capture = NB_TOURS_CAPTURE - alien.capture_en_cours;
        // Vérifier que la capture par l'allié est toujours possible
        if(tour_depart - tour_actuel() < turns_before_capture) continue;

        int alien_val = alien_score(alien);

        for(direction dir : DIR)
        {
            if(!can_push_toward(ally_pos, dir)) continue;
            position attack_pos = ally_pos + dir_to_vec(opposite(dir));
            if(!is_empty(attack_pos) && opponent_pos != attack_pos) continue;

            Path p = quickest_path(opponent_pos, attack_pos, NB_POINTS_ACTION-COUT_POUSSER, moi());
            if(p.cost > NB_POINTS_ACTION-COUT_POUSSER) continue;

            if(alien_val > best_attack.score)
            {
                best_attack.score = alien_val;
                best_attack.attack_pos = attack_pos;
                best_attack.target = ally_pos;
            }
        }
    }
    return best_attack;
}
