#include "strategy.hh"
#include <iostream>
#include <algorithm>
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

    double best_score = 0;
    std::vector<Strategy*> best_strategy_set(4, nullptr);
    std::vector<int> agents = {0, 1, 2, 3};
    while(true) // Permutation enumeration
    {
        int nb_cancel = 0;
        double current_score = 0;
        std::vector<bool> managed_aliens(aliens.size(), false);
        std::vector<Strategy*> agent_strategy;
        agent_strategy.reserve(4);

        for(int agent : agents)
        {
            position my_position = position_agent(moi(), agent);

            Strategy* best_strategy = nullptr;
            double best_score = +0;
            int best_alien = -1;

            for(unsigned int a = 0 ; a < aliens.size() ; a++)
            {
                if(managed_aliens[a]) continue;

                Strategy* strategy = new Idle(agent);
                alien_info alien = aliens[a];
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
                    best_alien = a;
                }
                else delete strategy;
            }

            managed_aliens[best_alien] = true;
            agent_strategy.push_back(best_strategy);
            nb_cancel += best_strategy->apply();
            current_score += best_score;
        }

        if(current_score > best_score)
        {
            best_score = current_score;
            for(int a = 0 ; a < NB_AGENTS ; a++) delete best_strategy_set[a];
            std::swap(best_strategy_set, agent_strategy);
        }

        for(int c = 0; c < nb_cancel ; c++) annuler();

        if(!next_permutation(agents.begin(), agents.end())) break;
    }

    for(int agent = 0 ; agent < NB_AGENTS ; agent++)
    {
        best_strategy_set[agent]->apply();
        delete best_strategy_set[agent];
    }

    end = std::clock();
    std::cout << "elapsed time: " << double(end - begin)/CLOCKS_PER_SEC << std::endl;
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}

