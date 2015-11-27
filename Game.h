#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <vector>
#include <iostream>  // for test

#include "Game_Hand.h"

class Game
{
private:
    static const int passing_directions[PLAYER_COUNT];

    int total_scores[PLAYER_COUNT];
    std::vector<int> winners;  // empty if game is not finished
    unsigned int passing_index;

public:
    Game_Hand hand;

    // Game() {}

    void game_reset();

    void end_hand();

    void change_passing() { passing_index = (passing_index + 1) % PLAYER_COUNT; }

    const std::vector<int>& get_winners() const { return winners; }
    int get_score(const int& player) const { return total_scores[player]; }
    int get_passing_direction() const { return passing_directions[passing_index]; }

    void play_test()  // test
    {
        std::cout << "begin\n";
        hand.deal_hands();
        std::cout << "dealt, now passing\n";
        for (int player_passing = 0; player_passing < PLAYER_COUNT; ++player_passing)
        {
            std::vector<Card> to_pass = hand.get_hands()[player_passing].pick_random(3);
            std::cout << "picked random\n";
            hand.pass(player_passing, (player_passing+get_passing_direction())%PLAYER_COUNT, to_pass);
        }
        std::cout << "passed, now receiving\n";
        hand.receive_passed_cards();
        std::cout << "received, now playing\n";
        for (int tricks = 13; tricks > 0; --tricks)
        {
            hand.reset_trick();
            for (int turns = 4; turns > 0; --turns)
            {
                Card to_play;
                auto hand_itr = hand.get_hands()[hand.get_whose_turn()].begin();
                to_play = *hand_itr;
                hand.play_card(to_play);
            }
            hand.end_trick();
            change_passing();
        }
        hand.end_hand();
    }
};

#endif // GAME_H_INCLUDED
