#ifndef TEXT_UI_H_INCLUDED
#define TEXT_UI_H_INCLUDED

#include <string>
#include <vector>

#include "Card.h"
#include "Deck.h"
#include "Game.h"

class Text_UI
{
private:
    Game game;

    void show_scores() const;
    void show_hand(const Deck& hand) const;

    std::string card_str(const Card& card) const;
    std::string direction_str(const int& how_many_players_to_the_left) const;

    /** this function called in other input functions
        @returns default constructed card for failure */
    Card choose_card(const std::vector<Card>& hand_vector) const;
    std::vector<Card> input_passing_choices(const Deck& hand) const;
    Card input_play_choice(const Deck& hand) const;
public:
    void play();
};

#endif // TEXT_UI_H_INCLUDED
