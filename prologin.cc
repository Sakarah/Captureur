#include "map.hh"
#include <iostream>
#include <chrono>

/// Fonction appelée au début de la partie.
void partie_init()
{
    // Rien à initialiser
}

/// Fonction appelée à chaque tour.
void jouer_tour()
{
    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();

    std::vector<alien_info> aliens = liste_aliens();
    std::cout << aliens.size() << std::endl;

    for(int agent = 0; agent < NB_AGENTS; agent++)
    {
        std::cout << agent << ":";

        position my_position = position_agent(moi(), agent);
        if(alien_sur_case(my_position)) continue;

        std::deque<Move> best_moves = std::deque<Move>();
        position best_alien = position{-1,-1};
        float best_score = 0;
        for(alien_info alien : aliens)
        {
            int tour_depart = alien.tour_invasion + alien.duree_invasion;

            // Vérifier que l'alien est encore/bientôt là
            if(tour_actuel() + 3 < alien.tour_invasion) continue; // Pas encore là
            if(alien.capture_en_cours == NB_TOURS_CAPTURE) continue; // Déjà capturé
            if(tour_depart < tour_actuel()) continue; // Déjà parti

            Path p = quickest_path(my_position, alien.pos);
            int turns_to_target = (p.cost+7)/8;
            int turns_to_alien = std::max(turns_to_target, alien.tour_invasion - tour_actuel());
            std::cout << p.cost << "|";

            if(turns_to_alien == 0)
            {
                // Vérifier que la capture est toujours possible
                if(tour_depart - tour_actuel() < NB_TOURS_CAPTURE - alien.capture_en_cours) continue;
            }
            else
            {
                // Vérifier que la capture est possible après le déplacement
                if(tour_depart - tour_actuel() - turns_to_target < NB_TOURS_CAPTURE) continue;
            }

            float score = alien.points_capture / (turns_to_alien+1);
            if(score > best_score)
            {
                std::swap(best_moves, p.path);
                best_score = score;
                best_alien = alien.pos;
            }
        }

        std::cout << " -> (" << best_alien.ligne << "," << best_alien.colonne << ")" << std::endl;
        for(Move m: best_moves)
        {
            if(perform_move(agent, m) != OK) break;
        }
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}

