#include "Deck.h"

// whether ace is high is defined here (1-13 or 2-14)
const int Deck::LOW = 2;
const int Deck::HIGH = 14;

Deck::Deck(const bool& create_full /*= false*/)
{
    cards.reserve(((HIGH + 1) * SUIT_COUNT) + 1);


    // default suit ordering
    for (int i = 0; i < SUIT_COUNT; ++i)
        suit_hash_values[i] = i * (HIGH + 1);

}
