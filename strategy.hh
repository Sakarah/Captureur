#ifndef STRATEGY_HH
#define STRATEGY_HH

#include "map.hh"

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

class StayOnAlien : public Strategy
{
public:
    StayOnAlien(int agent_id);
    void apply() override;
private:
    bool push;
    direction push_dir;
};

class ElimThreat : public Strategy
{
public:
    ElimThreat(int agent_id, Threat threat);
    void apply();
private:
    std::deque<Move> moves;
    bool push;
    direction push_dir;
};

class Idle : public Strategy
{
public:
    Idle(int agent_id);
    void apply() override;
};

#endif // STRATEGY_HH
