#ifndef GAME_HAND_H_INCLUDED
#define GAME_HAND_H_INCLUDED

#include <vector>
#include <unordered_set>

#include "Deck.h"
#include "Card.h"

const int PLAYER_COUNT = 4;
const Card STARTING_CARD(2, CLUBS);

class Game_Hand
{
private:
    std::vector<Deck> hands;
    std::vector<Deck> unknown_cards_for_player;  // for AI
    bool player_is_human[PLAYER_COUNT];

    int scores[PLAYER_COUNT];

    std::vector<std::vector<Card> > passed_cards_to_player;  // first index is player passed to

    // trick
    std::vector<Card> played_cards;
    int played_card_count;
    int trick_leader;

    int whose_turn;

    int points_for(const Card& card);

public:
    Game_Hand();

    // getters
    const int& get_whose_turn() const { return whose_turn; }
    const std::vector<Deck>& get_hands() const { return hands; }
    const int& get_score(const int& player) const { return scores[player]; }
    const bool& is_human(const int& player) const { return player_is_human[player]; }
    bool turns_left_in_trick() const { return played_card_count < PLAYER_COUNT; }

    // each hand, user interface calls these in this order
    void reset_hand();
    void deal_hands();
    void pass(const int& from_player, const int& to_player, const std::vector<Card>& passed_cards);
    void receive_passed_cards();
    void reset_trick();
    void play_card(const Card& card);
    void end_trick();  // TODO: returns who won the trick? (for UI)
    void end_hand();  // shoot the moon points
};

#endif // GAME_HAND_H_INCLUDED
