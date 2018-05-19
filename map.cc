#include "map.hh"
#include <queue>

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

/// Return the move sequence between two points that is the quickest in terms of action points.
Path quickest_path(position from, position to)
{
    if(!is_empty(to)) return Path{1000000000, std::deque<Move>()};

    position prev[TAILLE_BANQUISE][TAILLE_BANQUISE];
    for(int i = 0 ; i < TAILLE_BANQUISE ; i++)
    {
        for(int j = 0 ; j < TAILLE_BANQUISE ; j++) prev[i][j] = INVALID_POS;
    }

    std::priority_queue<DijkNode> queue;
    queue.push(DijkNode{0, from, from});

    while(!queue.empty())
    {
        DijkNode node = queue.top();
        queue.pop();

        // Pas de planification sur plus de 3 tours
        if(node.dist > 3*NB_POINTS_ACTION) break;

        if(prev[node.pos.ligne][node.pos.colonne] != INVALID_POS) continue;
        prev[node.pos.ligne][node.pos.colonne] = node.prev;

        if(node.pos == to)
        {
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
                if(dist(to, step) > 1) path.push_front(Move{ACTION_GLISSER, dir});
                else path.push_front(Move{ACTION_DEPLACER, dir});

                to = step;
            }
            return Path{node.dist, path};
        }

        for(direction dir : DIR)
        {
            position neigh = node.pos + dir_to_vec(dir);
            if(is_empty(neigh))
            {
                queue.push(DijkNode{node.dist + COUT_DEPLACEMENT, neigh, node.pos});
            }

            position glide_pos = glide_dest(node.pos, dir);
            if(glide_pos != node.pos)
            {
                queue.push(DijkNode{node.dist + COUT_GLISSADE, glide_pos, node.pos});
            }
        }
    }

    return Path{1000000000, std::deque<Move>()};
}

direction opposite(direction dir)
{
    if(dir == NORD) return SUD;
    else if(dir == SUD) return NORD;
    else if(dir == OUEST) return EST;
    else return OUEST;
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
