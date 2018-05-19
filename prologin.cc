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
    std::vector<Strategy*> alien_strategy(aliens.size(), nullptr);
    Strategy* agent_strategy[NB_AGENTS] = { nullptr, nullptr, nullptr, nullptr };
    int fixed_strategies = 0;
    while(fixed_strategies != NB_AGENTS)
    {
        for(int agent = 0; agent < NB_AGENTS; agent++)
        {
            if(agent_strategy[agent]) continue;

            position my_position = position_agent(moi(), agent);

            Strategy* best_strategy = nullptr;
            double best_score = +0;
            int best_alien = -1;

            for(unsigned int a = 0; a < aliens.size(); a++)
            {
                alien_info alien = aliens[a];

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

                if(alien_strategy[a] && strategy->score <= alien_strategy[a]->score) continue;

                if(strategy->score > best_score)
                {
                    best_score = strategy->score;
                    delete best_strategy;
                    best_strategy = strategy;
                    best_alien = a;
                }
            }

            if(best_strategy)
            {
                if(alien_strategy[best_alien])
                {
                    fixed_strategies--;
                    agent_strategy[alien_strategy[best_alien]->agent] = nullptr;
                    delete alien_strategy[best_alien];
                }

                alien_strategy[best_alien] = best_strategy;
                agent_strategy[agent] = best_strategy;
            }
            else
            {
                agent_strategy[agent] = new Idle(agent);
            }
            fixed_strategies++;
        }
    }

    for(int agent = 0 ; agent < NB_AGENTS ; agent++)
    {
        agent_strategy[agent]->apply();
        delete agent_strategy[agent];
    }

    end = std::clock();
    std::cout << "elapsed time: " << double(end - begin)/CLOCKS_PER_SEC << std::endl;
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}

