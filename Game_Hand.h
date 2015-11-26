#ifndef GAME_HAND_H_INCLUDED
#define GAME_HAND_H_INCLUDED

#include <vector>

#include "Deck.h"
#include "Card.h"

const int PLAYER_COUNT = 4;
const Card STARTING_CARD(2, CLUBS);

class Game_Hand
{
public:  // for test
    std::vector<Deck> hands;
private:
    std::vector<Deck> unknown_cards_for_player;  // for AI
    bool player_is_human[PLAYER_COUNT];

    int scores[PLAYER_COUNT];

    std::vector<std::vector<Card> > passed_cards_to_player;  // first index is player passed to

    // trick
    std::vector<Card> played_cards;
    int played_card_count;  // TODO: is this even used?
    int trick_leader;

    int whose_turn;

    int points_for(const Card& card);

public:
    Game_Hand();

    void reset_hand();
    void deal_hands();
    void pass(const int& from_player, const int& to_player, const std::vector<Card>& passed_cards);
    void receive_passed_cards();
    void reset_trick();
    void play_card(const Card& card);
    void end_trick();
};

#endif // GAME_HAND_H_INCLUDED
