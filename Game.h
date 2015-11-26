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
            std::vector<Card> to_pass;
            for (int i = 3; i > 0; --i)
            {
                to_pass.push_back(hand.hands[player_passing].deal_one());
                hand.hands[player_passing].insert(to_pass[to_pass.size()-1]);  // put it back, because pass will remove it
            }
            hand.pass(player_passing, (player_passing+1)%PLAYER_COUNT, to_pass);
        }
        hand.receive_passed_cards();
        for (int tricks = 13; tricks > 0; --tricks)
        {
            hand.reset_trick();
            for (int turns = 4; turn > 0; --turns)
            {
                Card to_play;
                auto hand_itr = hand.hands[hand.whose_turn].begin();
                to_play = *hand_itr;
                hand.play_card(to_play);
            }
        }
    }
};

#endif // GAME_H_INCLUDED
