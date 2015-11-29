#ifndef GAME_HAND_H_INCLUDED
#define GAME_HAND_H_INCLUDED

#include <vector>

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
    int pass_count;  // how many players have passed

    // trick
    std::vector<Card> played_cards;
    int played_card_count;
    int trick_leader;

    int whose_turn;
    bool hearts_broken;

    int points_for(const Card& card) const;

public:
    Game_Hand();

    // getters
    const int& get_whose_turn() const { return whose_turn; }
    const std::vector<Deck>& get_hands() const { return hands; }  // TODO: make player the parameter?
    const int& get_score(const int& player) const { return scores[player]; }
    const bool& is_human(const int& player) const { return player_is_human[player]; }
    bool turns_left_in_trick() const { return played_card_count < PLAYER_COUNT; }
    const bool& hearts_is_broken() { return hearts_broken; }
    const int& get_pass_count() { return pass_count; }

    // setter
    void set_pass_count() { pass_count = PLAYER_COUNT; }  // to be called on the keeper hand to say we've already passed

    // each hand, user interface calls these in this order
    void reset_hand();
    void deal_hands();
    void pass(const int& from_player, const int& to_player, const std::vector<Card>& passed_cards);
    void receive_passed_cards();  // TODO: return which cards (for UI)
    void reset_trick();
    void play_card(const Card& card);  // TODO: return hearts broken (for UI)
    void end_trick();  // TODO: returns who took the trick? (for UI)
    void end_hand();  // shoot the moon points

    // rules
    void find_valid_choices(std::vector<Card>& valid_choices) const;

    // AI
    Card static_play_ai();
    Card dynamic_play_ai();
    std::vector<Card> pass_ai(const int& from_player);
};

#endif // GAME_HAND_H_INCLUDED
