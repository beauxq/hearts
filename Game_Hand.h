#ifndef GAME_HAND_H_INCLUDED
#define GAME_HAND_H_INCLUDED

#include <string>
#include <vector>
#include <unordered_set>

#include "Deck.h"
#include "Card.h"

const int PLAYER_COUNT = 4;
const Card STARTING_CARD(2, CLUBS);

const int AI_LEVEL = 50000;  // how smart the AI is ( ~ number of simulated tricks)
// this number should be tuned so that the time test in Gui::play_ai_wrapper is about .5 on a slow computer
// (with compiler optimized for speed)

class Game_Hand
{
private:
    std::vector<Deck> hands;

    bool player_is_human[PLAYER_COUNT];

    int scores[PLAYER_COUNT];

    std::vector<std::vector<Card> > passed_cards_to_player;  // first index is player passed to
    // passed_cards_to_player is also used by AI for players to remember what cards they passed, and to whom
    // (so cards are removed from this when they are played)
    int pass_count;  // how many players have passed

    // trick
    std::vector<Card> played_cards;
    int played_card_count;
    int trick_leader;

    int whose_turn;
    bool hearts_broken;

    int points_for(const Card& card) const;

    // for AI
    bool points_played_this_trick;
    bool shoot_moon_possible;
    bool this_is_simulation;
    std::vector<Deck> unknown_cards_for_player;  // cards that the player doesn't know the location of
    std::unordered_set<int> player_seen_void_in_suits[PLAYER_COUNT];  // index is player
    // when a player shows that they have none of a suit, that suit is added to their set here
    // int is Suit, but there is no hash function for enumeration Suit

    /** replace hands with player's guess about what the other players' hands are
        (so the computer doesn't cheat)
        to be called in a simulation */
    void speculate_hands(const int& player_speculating, const int& passing_direction);
public:
    Game_Hand();

    // getters
    const int& get_whose_turn() const { return whose_turn; }
    const std::vector<Deck>& get_hands() const { return hands; }  // TODO: make player the parameter?
    const int& get_score(const int& player) const { return scores[player]; }
    const bool& is_human(const int& player) const { return player_is_human[player]; }
    bool turns_left_in_trick() const { return played_card_count < PLAYER_COUNT; }
    const bool& hearts_is_broken() const { return hearts_broken; }
    const int& get_pass_count() const { return pass_count; }
    const std::vector<Card>& get_played_cards();  // first put null cards where cards haven't been played
    const bool& possible_to_shoot_moon() const { return shoot_moon_possible; }
    const std::vector<Card>& get_passed_cards_to_player(const size_t& player) const { return passed_cards_to_player[player]; }

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
    int end_hand();  // returns who shot the moon, -1 if no one shot the moon

    // rules
    void find_valid_choices(std::vector<Card>& valid_choices) const;
    void find_valid_choice_rule(std::string& rule) const;

    // AI
    Card static_play_ai() const;
    Card dynamic_play_ai(const int& passing_direction) const;
    Card simulation_play_ai() const;  // used in simulation (inside dynamic_play_ai), static or more random if it's possible to shoot the moon
    std::vector<Card> pass_ai(const int& from_player, const int& passing_direction) const;
};

#endif // GAME_HAND_H_INCLUDED
