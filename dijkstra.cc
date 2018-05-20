#include "dijkstra.hh"
#include <queue>

struct DijkNode
{
    int dist;
    position pos;
    position prev;
};

bool operator<(const DijkNode& a, const DijkNode& b)
{
    return a.dist > b.dist;
}

Dijkstra::Dijkstra(position start, int action_limit, int adversaire)
{
    adv = adversaire;
    from = start;

    position prev[TAILLE_BANQUISE][TAILLE_BANQUISE];
    for(int i = 0 ; i < TAILLE_BANQUISE ; i++)
    {
        for(int j = 0 ; j < TAILLE_BANQUISE ; j++) prev[i][j] = INVALID_POS;
    }

    std::priority_queue<DijkNode> queue;
    queue.push(DijkNode{0, start, start});

    while(!queue.empty())
    {
        DijkNode node = queue.top();
        queue.pop();

        // Pas de planification sur trop longtemps / pas toujours utile
        if(node.dist > action_limit) break;

        if(prev[node.pos.ligne][node.pos.colonne] != INVALID_POS) continue;
        prev[node.pos.ligne][node.pos.colonne] = node.prev;

        for(direction dir : DIR)
        {
            position neigh = node.pos + dir_to_vec(dir);
            if(is_empty(neigh))
            {
                queue.push(DijkNode{node.dist + COUT_DEPLACEMENT, neigh, node.pos});
            }
            else if(agent_sur_case(neigh) == adversaire)
            {
                if(can_push_toward(neigh, find_dir(node.pos, neigh)))
                {
                    queue.push(DijkNode{node.dist + COUT_DEPLACEMENT + COUT_POUSSER, neigh, node.pos});
                }
            }

            position glide_pos = glide_dest(node.pos, dir);
            if(glide_pos != node.pos)
            {
                queue.push(DijkNode{node.dist + COUT_GLISSADE, glide_pos, node.pos});
            }
        }
    }
}

#include <iostream>

/// Return the move sequence between two points that is the quickest in terms of action points.
Path Dijkstra::quickest_path(position to)
{
    if(from == to) return Path{0, std::deque<Move>()};
    if(!is_empty(to)) return Path{1000000000, std::deque<Move>()};
    if(prev[to.ligne][to.colonne] == INVALID_POS)
        return Path{1000000000, std::deque<Move>()};

    int cost = 0;

    // Reconstruct path from prev array
    std::deque<Move> path;
    while(to != from)
    {
        position step = prev[to.ligne][to.colonne];
        direction dir;
        if(to.ligne == step.ligne)
        {
            if(to.colonne < step.colonne) dir = OUEST;
            else dir = EST;
        }
        else
        {
            if(to.ligne < step.ligne) dir = NORD;
            else dir = SUD;
        }

        if(agent_sur_case(to) == adv)
        {
            cost += COUT_POUSSER;
            path.push_front(Move{ACTION_POUSSER, dir});
        }

        if(dist(to, step) > 1)
        {
            cost += COUT_GLISSADE;
            path.push_front(Move{ACTION_GLISSER, dir});
        }
        else
        {
            cost += COUT_DEPLACEMENT;
            path.push_front(Move{ACTION_DEPLACER, dir});
        }

        to = step;
    }

    return Path{cost, path};
}

std::vector<Dijkstra> my_dijkstra;
std::vector<Dijkstra> opp_dijkstra;

void update_static_tabs()
{
    my_dijkstra.clear();
    my_dijkstra.reserve(4);
    for(int i = 0; i < NB_AGENTS ; i++)
    {
        Dijkstra dijk(position_agent(moi(), i));
        my_dijkstra.push_back(dijk);
    }

    my_dijkstra.clear();
    opp_dijkstra.reserve(4);
    for(int i = 0; i < NB_AGENTS ; i++)
    {
        Dijkstra dijk(position_agent(adversaire(), i), NB_POINTS_ACTION, moi());
        opp_dijkstra.push_back(dijk);
    }
}
