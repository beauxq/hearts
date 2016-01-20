#include "Deck.h"

#include <iostream>  // TODO: remove test

void Deck::iterator::check_between_sets()
{
    // std::cout << "entering check between sets, current set: " << current_set
    //           << " is end? " << (inside_iterator == (parent->cards)[(parent->sort_order)[current_set]].end()) << std::endl;
    while ((current_set != (LAST_SUIT)) &&
           (inside_iterator == (parent->cards)[(parent->sort_order)[current_set]].end()))
    {
        ++current_set;
        inside_iterator = (parent->cards)[(parent->sort_order)[current_set]].begin();
        // std::cout << "card at inside iterator: " << inside_iterator->get_value() << ' ' << inside_iterator->get_suit() << std::endl;
    }
    // std::cout << "card at inside iterator: " << inside_iterator->get_value() << ' ' << inside_iterator->get_suit() << std::endl;
}

const Card& Deck::iterator::operator* ()
{
    return *inside_iterator;
}

const Card* Deck::iterator::operator-> ()
{
    return &(*inside_iterator);
}

Deck::iterator& Deck::iterator::operator++ ()
{
    ++inside_iterator;
    check_between_sets();
    return *this;
}
