#include "strategy.hh"
#include <iostream>
#include <chrono>

/**
 * Captureur - IA Prologin 2018
 * Captureur est une IA qui essaye d'allier différentes stratégies à répartir sur les 4 manchots.
 * Les manchots essayent tous de prendre un alien ou ennemi objectif pendant leur tour
 * afin de maximiser le score qu'ils espèrent être capable d'obtenir.
 * Si un manchot estime pouvoir remporter plus de points qu'un autre en prenant un objectif déjà attribué,
 * le manchot qui l'avait précédemment devra changer d'objectif.
 *
 * Chaque alien vaut son nombre de points multiplié par un plus le temps qui a déjà été passé pour essayer de l'obtenir.
 * Les aliens difficilement accessibles ou qui arrivent après font gagner moins de points.
 * Toutes ces fonctions heuristiques cherchent à être homogènes et à ne pas introduire de constantes arbitraires.
 *
 * De plus pousser un ennemi (resp. aider un allié) est aussi modélisé par une estimation du nombre de points perdus (resp. non perdus).
 *
 * Pour connaître les emplacements accessibles, et y aller au plus vite on utilise un Dijkstra.
 *
 * On appelle stratégie le choix par un manchot d'effectuer l'une des quatres actions suivantes
 * - Rester sur sa case
 * - Pousser un ennemi
 * - Aller chercher un alien
 * - Aider un allié
 *
 * Si aucune action n'est utile, il se contente de se rapprocher d'un allié quelconque.
 *
 * Dans les idées ont été testées, il y avait d'autres stratégies de défense moins efficaces en pratiques,
 * et une gestion de la visibilité dans le Dijkstra.
 *
 * Avec plus de temps j'aurais aimé être capable de demander une aide au déplacement à un allié,
 * de former des structures défensives plus stables ou d'avoir une meilleure gestion de l'attribution des tâches.
 */


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

    std::vector<Strategy*> alien_strategy(aliens.size()+4, nullptr);
    Strategy* agent_strategy[NB_AGENTS] = { nullptr, nullptr, nullptr, nullptr };
    int fixed_strategies = 0;
    while(fixed_strategies != NB_AGENTS)
    {
        for(int agent = 0; agent < NB_AGENTS; agent++)
        {
            if(agent_strategy[agent]) continue;

            position my_position = position_agent(moi(), agent);

            Strategy* best_strategy = nullptr;
            double best_score = 1e-300; // Avoid stupid actions
            int best_alien = -1;

            for(unsigned int a = 0; a < aliens.size()+NB_AGENTS; a++)
            {
                Strategy* strategy = nullptr;

                if(a < aliens.size())
                {
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
                }
                else
                {
                    strategy = new ElimThreat(agent, a-aliens.size());
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

                if(best_alien < (int)aliens.size())
                {
                    std::cout << agent << " => l" << aliens[best_alien].pos.ligne
                              << " c" << aliens[best_alien].pos.colonne << std::endl;
                }
                else
                {
                    position adv_pos = position_agent(adversaire(), best_alien-aliens.size());
                    std::cout << agent << " |> l" << adv_pos.ligne
                              << " c" << adv_pos.colonne << std::endl;
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

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";
}

/// Fonction appelée à la fin de la partie.
void partie_fin()
{
    // Rien à libérer
}
