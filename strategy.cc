#include "strategy.hh"
#include <iostream>

GoToAlien::GoToAlien(int agent_id, const alien_info& alien)
{
    agent = agent_id;
    score = -0;
    position my_position = position_agent(moi(), agent);

    int tour_depart = alien.tour_invasion + alien.duree_invasion;
    tour_depart = std::min(tour_depart, 100);

    // Vérifier que l'alien est encore/bientôt là
    if(tour_actuel() + 5 < alien.tour_invasion) return; // Pas encore là
    if(alien.capture_en_cours == NB_TOURS_CAPTURE) return; // Déjà capturé
    if(tour_depart < tour_actuel()) return; // Déjà parti

    Path p = quickest_path(my_position, alien.pos, MOI);
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
    tour_depart = std::min(tour_depart, 100);

    // Vérifier que l'alien est là
    if(tour_actuel() < alien.tour_invasion) return; // Pas encore là
    if(alien.capture_en_cours == NB_TOURS_CAPTURE) return; // Déjà capturé

    int turns_before_capture = NB_TOURS_CAPTURE - alien.capture_en_cours;
    // Vérifier que la capture par l'ennemi est toujours possible
    if(tour_depart - tour_actuel() < turns_before_capture) return;

    for(direction dir : DIR)
    {
        if(!can_push_toward(alien.pos, dir, 0)) continue;
        position attack_pos = alien.pos + dir_to_vec(opposite(dir));
        if(!is_empty(attack_pos, 0) && my_position != attack_pos) continue;

        Path p = quickest_path(my_position, attack_pos, MOI);
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

    score = alien_def_score(alien);

    for(direction dir : DIR)
    {
        position pushed = my_position + dir_to_vec(dir);
        if(agent_sur_case(pushed) != adversaire()) continue;
        if(can_push_toward(pushed, dir, MOI))
        {
            int enemy_cost = std::min(COUT_GLISSADE, dist(pushed, glide_dest(pushed, dir, MOI)));
            double dir_score = alien_def_score(alien) * (1 + (enemy_cost / NB_POINTS_ACTION));
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

ElimThreat::ElimThreat(int agent_id, int opp_id)
{
    push = false;
    agent = agent_id;
    score = -0;

    AttackInfo atk = best_opponent_attack(opp_id, MOI|ADV);
    atk.score /= count_threats(atk.target, MOI|ADV);

    position my_position = position_agent(moi(), agent);
    position adv_pos = position_agent(adversaire(), opp_id);
    position ally_pos = atk.target;

    if(alien_sur_case(adv_pos)) return;
    std::vector<position> def_positions;

    direction dir = find_dir(ally_pos, adv_pos);
    if(dir != INVALIDE)
    {
        position dir_vec = dir_to_vec(dir);

        for(position def_pos = ally_pos+dir_vec; def_pos != adv_pos; def_pos = def_pos + dir_vec)
        {
            if(type_case(def_pos) != LIBRE) break;
            def_positions.push_back(def_pos);
        }
    }

    def_positions.push_back(atk.attack_pos);

    direction atk_axis = find_dir(atk.attack_pos, atk.target);
    def_positions.push_back(atk.target + dir_to_vec(atk_axis));

    if(atk.target + dir_to_vec(atk_axis) == atk.attack_pos) std::cout << "WTF???";

    for(position def_pos : def_positions)
    {
        Path p = quickest_path(my_position, def_pos, NB_POINTS_ACTION);
        if(p.cost > NB_POINTS_ACTION) continue;

        // TODO : Better move emulation
        int cancel_count = 0;
        for(Move m : p.path)
        {
            if(perform_move(agent, m) != OK) break;
            cancel_count++;
        }
        AttackInfo other_atk = best_opponent_attack(opp_id, ADV);
        for(int c = 0; c < cancel_count; c++) annuler();

        double cur_score = atk.score - other_atk.score;
        if(cur_score > score)
        {
            score = cur_score;
            std::swap(p.path, moves);
        }
    }

    // Pousse si possible
    for(direction pdir : DIR)
    {
        if(!can_push_toward(adv_pos, pdir, MOI)) continue;
        position attack_pos = adv_pos + dir_to_vec(opposite(pdir));
        if(!is_empty(attack_pos, MOI) && my_position != attack_pos) continue;

        bool is_just_near = dist(adv_pos, ally_pos) == 1;
        int min_action_left = NB_POINTS_ACTION - COUT_POUSSER - (is_just_near ? 1 : 0);
        Path push_path = quickest_path(my_position, attack_pos, min_action_left);
        if(push_path.cost <= min_action_left)
        {
            // TODO : Better move emulation here too
            int cancel_count = 0;
            for(Move m : push_path.path)
            {
                if(perform_move(agent, m) != OK) goto end_move;
                cancel_count++;
            }
            if(pousser(agent, pdir) != OK) goto end_move;
            cancel_count++;
            if(deplacer(agent, pdir) != OK) goto end_move;
            cancel_count++;
            end_move:
            AttackInfo other_atk = best_opponent_attack(opp_id, ADV);
            for(int c = 0; c < cancel_count; c++) annuler();

            double cur_score = atk.score - other_atk.score;
            if(cur_score >= score)
            {
                score = cur_score;
                push = true;
                push_dir = pdir;
                std::swap(push_path.path, moves);
            }
        }
    }
}

void ElimThreat::apply()
{
    for(Move m: moves)
    {
        if(perform_move(agent, m) != OK) return;
    }
    if(push)
    {
        if(pousser(agent, push_dir) != OK) return;
        deplacer(agent, push_dir); // Prend la place si possible
    }
}

Idle::Idle(int agent_id)
{
    agent = agent_id;
    score = 0;
}

void Idle::apply()
{
    std::cout << agent << " => idle..." << std::endl;

    // Se rapprocher de quelqu'un de notre équipe
    position my_pos = position_agent(moi(), agent);
    std::deque<Move> moves;
    int dist = 25*25*25;
    for(int i = 0 ; i < NB_AGENTS ; i++)
    {
        if(i == agent) continue;
        position ally_pos = position_agent(moi(), i);
        for(direction dir : DIR)
        {
            Path p = quickest_path(my_pos, ally_pos+dir_to_vec(dir), 0);
            if(p.cost < dist)
            {
                dist = p.cost;
                std::swap(moves, p.path);
            }
        }
    }
    for(Move m: moves)
    {
        perform_move(agent, m);
    }
}
