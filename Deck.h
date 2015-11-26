#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <vector> // ?
#include <utility>  // pair ?
#include <set>

#include "Card.h"

// a group of cards (can be a deck or a hand)
class Deck
{
private:
    std::set<Card> cards[SUIT_COUNT];  // set for each suit
    Suit sort_order[SUIT_COUNT];

    int card_count;
    // std::pair<unsigned int, unsigned int> suit_dividers[SUIT_COUNT];  // all of this suit in [first, second) (original/default order is index)
public:
    class iterator
    {
    private:
        friend class Deck;

        Deck* parent;
        int current_set;
        std::set<Card>::iterator inside_iterator;

        void check_between_sets();
    public:
        iterator(Deck* parent_) : parent(parent_) {}
        const Card& operator* ();
        const Card* operator-> ();
        iterator& operator++ ();  // prefix increment

        bool operator== (const iterator& rhs) { return inside_iterator == rhs.inside_iterator; }
        bool operator!= (const iterator& rhs) { return ! operator== (rhs); }
    };

    friend class iterator;

    iterator begin();
    iterator end();

    static const int LOW;  // whether ace is high is defined in these (1-13 or 2-14)
    static const int HIGH;

    Deck(const bool& create_full = false);

    /** changes order of suits
        @throws std::invalid_argument if all suits are not given */
    void change_sort(const std::vector<Suit>& suits_in_order);

    void pop(const Card& card_to_remove);

    /** random */
    Card deal_one();
};

#endif // DECK_H_INCLUDED
