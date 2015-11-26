#include "Deck.h"

#include <stdexcept>
#include <iostream>  // for test

// whether ace is high is defined here (1-13 or 2-14)
const int Deck::LOW = 2;  // must be > 0 or constructor will break
const int Deck::HIGH = 14;

void Deck::iterator::check_between_sets()
{
    while (current_set != sort_order[SUIT_COUNT-1] && real_iterator == cards[sort_order[current_set]].end())
    {
        ++current_set;
        real_iterator = cards[sort_order[current_set]].begin();
    }
}

Card& Deck::iterator::operator* ()
{
    return *real_iterator;
}

Card* Deck::iterator::operator-> ()
{
    return real_iterator;
}

Deck::iterator Deck::begin()
{
    iterator itr_to_return;
    itr_to_return.current_set = 0;
    itr_to_return.real_iterator = cards[sort_order[current_set]].begin();
    itr_to_return.check_between_sets();
}

Deck::Deck(const bool& create_full /*= false*/)
{
    card_count = 0;

    int stopping_value;
    if (! create_full)
        stopping_value = LOW - 1;
    else
    {
        stopping_value = HIGH;
    }
    for (int suit = 0; suit < SUIT_COUNT; ++suit)
    {
        // suit_dividers[suit].first = cards.size();
        for (int value = LOW; value <= stopping_value; ++value)  // this loop does not run if create_full is false
        {
            cards[suit].insert(Card(value, Suit(suit)));
            ++card_count;
        }
        // suit_dividers[suit].second = cards.size();
        sort_order[suit] = Suit(suit);  // default sort order
    }
}

void Deck::change_sort(const std::vector<Suit>& suits_in_order)
{
    // make sure all suits are there
    bool all_found = false;
    if (suits_in_order.size() == SUIT_COUNT)
    {
        all_found = true;
        bool found[SUIT_COUNT];
        for (int i = 0; i < SUIT_COUNT; ++i)
            found[i] = false;
        for (int i = 0; i < SUIT_COUNT; ++i)
            found[suits_in_order[i]] = true;
        for (int i = 0; i < SUIT_COUNT; ++i)
            if (! found[i])
            {
                all_found = false;
                break;
            }
    }
    if (! all_found)
        throw std::invalid_argument("invalid suit order");

    /*
    std::vector<Card> new_cards;
    std::pair<unsigned int, unsigned int> new_suit_dividers[SUIT_COUNT];

    for (auto itr = suits_in_order.begin(); itr != suits_in_order.end(); ++itr)
    {
        new_suit_dividers[*itr].first = new_cards.size();
        for (unsigned int from_index = suit_dividers[*itr].first; from_index < suit_dividers[*itr].second; ++from_index)
        {
            new_cards.push_back(cards[from_index]);
        }
        new_suit_dividers[*itr].second = new_cards.size();
    }

    // update all data
    cards = new_cards;
    */
    for (int i = 0; i < SUIT_COUNT; ++i)
    {
        // suit_dividers[i] = new_suit_dividers[i];
        sort_order[i] = suits_in_order[i];
    }
}

void Deck::pop(const Card& card_to_remove)
{
    if (cards[card_to_remove.get_suit()].erase(card_to_remove))
        --card_count;
}

Card Deck::deal_one()
{

}
