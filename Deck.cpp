#include "Deck.h"

#include <stdexcept>
#include <iostream>  // for test
#include <cstdlib>
#include <algorithm>  // find

// whether ace is high is defined here (1-13 or 2-14)
const int Deck::LOW = 2;
const int Deck::HIGH = 14;

const int Deck::VALUE_COUNT = HIGH + 1 - LOW;

Deck::iterator Deck::begin() const
{
    iterator itr_to_return(this);
    itr_to_return.current_set = 0;
    itr_to_return.inside_iterator = cards[sort_order[itr_to_return.current_set]].begin();
    itr_to_return.check_between_sets();

    return itr_to_return;
}

Deck::iterator Deck::end() const
{
    iterator itr_to_return(this);
    itr_to_return.current_set = SUIT_COUNT - 1;
    itr_to_return.inside_iterator = cards[sort_order[itr_to_return.current_set]].end();

    return itr_to_return;
}

Deck::Deck(const bool& create_full /*= false*/)
{
    card_count = 0;

    if (create_full)
        fill();
    for (int suit = 0; suit < SUIT_COUNT; ++suit)
    {
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

    for (int i = 0; i < SUIT_COUNT; ++i)
    {
        sort_order[i] = suits_in_order[i];
    }
}

void Deck::erase(const Card& card_to_remove)
{
    if (cards[card_to_remove.get_suit()].erase(card_to_remove))  // TODO: does this remove the right card (after order change)?
    {
        --card_count;
    }
}

Card Deck::deal_one()
{
    auto deck_itr = begin();
    int choice = rand() % card_count;  // used as the iterating variable to advance iterator
    while (choice > 0)
    {
        ++deck_itr;
        --choice;
    }

    Card to_return = *deck_itr;
    erase(to_return);

    return to_return;
}

std::vector<Card> Deck::pick_random(int n) const
{
    // std::cout << "entered pick_random to pick " << n << std::endl;
    std::vector<Card> to_return;

    iterator deck_itr(this);
    int choice;

    for (; n > 0; --n)
    {
        // std::cout << "enter loop needing " << n << " picks\n";
        deck_itr = begin();
        choice = rand() % (card_count - to_return.size());
        while (choice > 0)
        {
            // std::cout << "need to advance deck_itr " << choice << std::endl;
            ++deck_itr;
            bool this_might_be_in_vector = true;
            while (this_might_be_in_vector)
            {
                // std::cout << "haven't found it not in vector\n";
                std::vector<Card>::iterator to_ret_itr = std::find(to_return.begin(), to_return.end(), *deck_itr);
                if (to_ret_itr == to_return.end())  // got through the vector without finding it
                    this_might_be_in_vector = false;
                else
                    ++deck_itr;
            }
            --choice;
        }

        to_return.push_back(*deck_itr);
    }

    return to_return;
}

void Deck::insert(const Card& card)
{
    if (cards[card.get_suit()].insert(card).second)  // second is bool whether inserted
        ++card_count;
}

void Deck::clear()
{
    for (int i = 0; i < SUIT_COUNT; ++i)
        cards[i].clear();
    card_count = 0;
}

void Deck::fill()
{
    for (int suit = 0; suit < SUIT_COUNT; ++suit)
    {
        for (int value = LOW; value <= HIGH; ++value)
        {
            if (cards[suit].insert(Card(value, Suit(suit))).second)  // sets might be not empty
                ++card_count;
        }
    }
}

const Card& Deck::at(int index) const
{
    auto itr = begin();
    while (index > 0)
    {
        ++itr;
        --index;
    }

    return *itr;
}

bool Deck::contains_non_points() const
{
    return (cards[CLUBS].size() ||
            cards[DIAMONDS].size() ||
            (cards[SPADES].size() > 1) ||
            (cards[SPADES].size() /* exactly 1 spade */ && (cards[SPADES].begin()->get_value() != 12)));
}
