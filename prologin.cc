#include "strategy.hh"
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
    std::clock_t begin, end;
    begin = std::clock();

    std::vector<alien_info> aliens = liste_aliens();
    for(int agent = 0; agent < NB_AGENTS; agent++)
    {
        bool end_turn = false;
        while(!end_turn)
        {
            position my_position = position_agent(moi(), agent);

            Strategy* best_strategy = nullptr;
            double best_score = +0;
            for(alien_info alien : aliens)
            {
                Strategy* strategy = nullptr;
                if(my_position == alien.pos)
                {
                    strategy = new StayOnAlien(agent);
                }
                else if(agent_sur_case(alien.pos) == adversaire())
                {
                    strategy = new PushEnemy(agent, alien);
                }
                else
                {
                    strategy = new GoToAlien(agent, alien);
                }

                if(strategy->score > best_score)
                {
                    best_score = strategy->score;
                    delete best_strategy;
                    best_strategy = strategy;
                }
            }

            if(best_strategy)
            {
                best_strategy->apply();
                end_turn = best_strategy->end_of_turn;
                delete best_strategy;
            }
            else
            {
                end_turn = true;
            }

            if(points_action_agent(agent) == 0) end_turn = true;
        }
    }

    end = std::clock();
    std::cout << "elapsed time: " << double(end - begin)/CLOCKS_PER_SEC << std::endl;
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}

