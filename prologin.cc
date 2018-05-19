#include "map.hh"

/// Fonction appelée au début de la partie.
void partie_init()
{
    // Rien à initialiser
}

bool is_capturable(alien_info alien, int time_to_reach)
{
    int arrivee_agent = tour_actuel() + time_to_reach;

    int tour_invasion = alien.tour_invasion;
    if(tour_invasion > arrivee_agent) return false;

    if(alien.capture_en_cours == NB_TOURS_CAPTURE) return false;

    /*int tour_depart = tour_invasion + alien.duree_invasion;
    int tour_capture = arrivee_agent + NB_TOURS_CAPTURE - alien.capture_en_cours;
    if(tour_capture <= tour_depart) return false;*/

    return true;
}

/// Fonction appelée à chaque tour.
void jouer_tour()
{
    for(int agent = 0; agent < NB_AGENTS; agent++)
    {
        position my_position = position_agent(moi(), agent);
        std::deque<Move> best_moves = std::deque<Move>();
        position wanted_dest = INVALID_POS;
        float best_score = 0;
        for(alien_info alien : liste_aliens())
        {
            if(!is_capturable(alien, 0)) continue;

            Path p = quickest_path(my_position, alien.pos);
            int turns = (p.cost+7)/8;
            if(!is_capturable(alien, turns)) continue;

            float score = alien.points_capture / ((turns/8)+1);
            if(score > best_score)
            {
                std::swap(best_moves, p.path);
                best_score = score;
                wanted_dest = alien.pos;
            }
        }

        for(Move m: best_moves)
        {
            perform_move(agent, m);
        }
    }
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}

