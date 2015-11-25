#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <vector>
#include <set>

#include "Card.h"

// a group of cards (can be a deck or a hand)
class Deck
{
private:
    std::vector<Card> cards;
    int suit_hash_values[SUIT_COUNT];  // if this ever changes, need to rehash
    std::set<int> used_indices;
public:
    static const int LOW;  // whether ace is high is defined in these (1-13 or 2-14)
    static const int HIGH;

    Deck(const bool& create_full = false);

};

#endif // DECK_H_INCLUDED
