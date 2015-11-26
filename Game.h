#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "Game_Hand.h"

class Game
{
private:
    Game_Hand hand;
public:
    void play_test()  // test
    {
        hand.deal_hands();
        for (int player_passing = 0; player_passing < PLAYER_COUNT; ++player_passing)
        {
            std::vector<Card> to_pass = hand.get_hands()[hand.get_whose_turn()].pick_random(3);
            hand.pass(player_passing, (player_passing+1)%PLAYER_COUNT, to_pass);
        }
        hand.receive_passed_cards();
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
        }
    }
};

#endif // GAME_H_INCLUDED
