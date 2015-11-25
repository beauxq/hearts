#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <vector>
#include <utility>  // pair

#include "Card.h"

// a group of cards (can be a deck or a hand)
class Deck
{
private:
    std::vector<Card> cards;
    Suit sort_order[SUIT_COUNT];
    std::pair<unsigned int, unsigned int> suit_dividers[SUIT_COUNT];  // all of this suit in [first, second) (original/default order is index)
public:
    static const int LOW;  // whether ace is high is defined in these (1-13 or 2-14)
    static const int HIGH;

    Deck(const bool& create_full = false);

    /** changes order of suits
        @throws std::invalid_argument if all suits are not given */
    void change_sort(const std::vector<Suit>& suits_in_order);
};

#endif // DECK_H_INCLUDED
