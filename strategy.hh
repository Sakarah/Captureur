#ifndef STRATEGY_HH
#define STRATEGY_HH

#include "map.hh"

void self_defend(int agent_id);

class Strategy
{
public:
    virtual ~Strategy() = default;
    int agent;
    double score;
    virtual void apply() = 0;
};

class GoToAlien : public Strategy
{
public:
    GoToAlien(int agent_id, const alien_info& alien);
    void apply() override;
private:
    std::deque<Move> moves;
};

class PushEnemy : public Strategy
{
public:
    PushEnemy(int agent_id, const alien_info& alien);
    void apply() override;
private:
    std::deque<Move> moves;
    direction push_dir;
};

#endif // STRATEGY_HH
