#include "strategy.hh"
#include <iostream>

GoToAlien::GoToAlien(int agent_id, const alien_info& alien)
{
    agent = agent_id;
    score = -0;
    position my_position = position_agent(moi(), agent);

    int tour_depart = alien.tour_invasion + alien.duree_invasion;

    // Vérifier que l'alien est encore/bientôt là
    if(tour_actuel() + 3 < alien.tour_invasion) return; // Pas encore là
    if(alien.capture_en_cours == NB_TOURS_CAPTURE) return; // Déjà capturé
    if(tour_depart < tour_actuel()) return; // Déjà parti

    Path p = quickest_path(my_position, alien.pos);
    int turns_to_target = (p.cost+7)/8;
    int turns_to_alien = std::max(turns_to_target, alien.tour_invasion - tour_actuel());

    if(turns_to_alien == 0)
    {
        // Vérifier que la capture est toujours possible
        if(tour_depart - tour_actuel() < NB_TOURS_CAPTURE - alien.capture_en_cours) return;
    }
    else
    {
        // Vérifier que la capture est possible après le déplacement
        if(tour_depart - tour_actuel() - turns_to_target < NB_TOURS_CAPTURE) return;
    }

    score = alien.points_capture / (turns_to_alien+1);
    std::swap(moves, p.path);
}

void GoToAlien::apply()
{
    for(Move m: moves)
    {
        if(perform_move(agent, m) != OK) break;
    }
}

PushEnemy::PushEnemy(int agent_id, const alien_info& alien)
{
    agent = agent_id;
    score = -0;
    position my_position = position_agent(moi(), agent);

    int tour_depart = alien.tour_invasion + alien.duree_invasion;

    // Vérifier que l'alien est là
    if(tour_actuel() < alien.tour_invasion) return; // Pas encore là
    if(alien.capture_en_cours == NB_TOURS_CAPTURE) return; // Déjà capturé

    int turns_before_capture = NB_TOURS_CAPTURE - alien.capture_en_cours;
    // Vérifier que la capture par l'ennemi est toujours possible
    if(tour_depart - tour_actuel() < turns_before_capture) return;

    for(direction dir : DIR)
    {
        if(!can_push_toward(alien.pos, dir)) continue;
        position attack_pos = alien.pos + dir_to_vec(opposite(dir));
        if(!is_empty(attack_pos) && my_position != attack_pos) continue;

        Path p = quickest_path(my_position, attack_pos);
        int turns_to_target = (p.cost+COUT_POUSSER+7)/8;

        if(turns_to_target > turns_before_capture) continue;

        int dir_score = alien_score(alien) / (turns_to_target+1);
        if(dir_score > score)
        {
            score = dir_score;
            std::swap(moves, p.path);
            push_dir = dir;
        }
    }
}

void PushEnemy::apply()
{
    for(Move m: moves)
    {
        if(perform_move(agent, m) != OK) return;
    }
    if(pousser(agent, push_dir) != OK) return;
    if(deplacer(agent, push_dir) != OK) return;
}

StayOnAlien::StayOnAlien(int agent_id)
{
    push = false;
    agent = agent_id;

    position my_position = position_agent(moi(), agent);

    alien_info alien = info_alien(my_position);
    if(alien.tour_invasion == -1) return;

    score = alien_score(alien);

    for(direction dir : DIR)
    {
        position pushed = my_position + dir_to_vec(dir);
        if(agent_sur_case(pushed) != adversaire()) continue;
        if(can_push_toward(pushed, dir))
        {
            int enemy_cost = std::min(COUT_GLISSADE, dist(pushed, glide_dest(pushed, dir)));
            double dir_score = alien_score(alien) * (1 + (enemy_cost / NB_POINTS_ACTION));
            if(dir_score > score)
            {
                push = true;
                push_dir = dir;
                score = dir_score;
            }
            break;
        }
    }
}

void StayOnAlien::apply()
{
    if(push) pousser(agent, push_dir);
}

Idle::Idle(int agent_id)
{
    agent = agent_id;
    score = 0;
}

void Idle::apply()
{
    std::cout << "idle..." << std::endl;
}
