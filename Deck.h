#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <vector> // ?
#include <utility>  // pair ?
#include <cstdlib>
#include <set>

#include "Card.h"

// a group of cards (can be a deck or a hand)
class Deck
{
private:
    std::set<Card> cards[SUIT_COUNT];  // set for each suit
    Suit sort_order[SUIT_COUNT];

    size_t card_count;
public:
    // TODO: this is actually const_iterator, no need for normal iterator yet (probably ever)
    class iterator
    {
    private:
        friend class Deck;

        const Deck* parent;
        int current_set;
        std::set<Card>::iterator inside_iterator;

        void check_between_sets();
    public:
        iterator(const Deck* parent_) : parent(parent_) {}
        const Card& operator* ();
        const Card* operator-> ();
        iterator& operator++ ();  // prefix increment

        bool operator== (const iterator& rhs) { return inside_iterator == rhs.inside_iterator; }
        bool operator!= (const iterator& rhs) { return ! operator== (rhs); }
    };

    friend class iterator;

    iterator begin() const;
    iterator end() const;

    std::set<Card>::iterator suit_begin(const Suit& suit) const { return cards[suit].begin(); }
    std::set<Card>::iterator suit_end(const Suit& suit) const { return cards[suit].end(); }
    std::set<Card>::reverse_iterator suit_rbegin(const Suit& suit) const { return cards[suit].rbegin(); }
    std::set<Card>::reverse_iterator suit_rend(const Suit& suit) const { return cards[suit].rend(); }

    static const int LOW;  // 2 (whether ace is high is defined in these: 1-13 or 2-14)
    static const int HIGH;  // 14

    static const int VALUE_COUNT;  // 13 (avoid magic numbers and avoid extra arithmetic)
    static const int LAST_SUIT;  // 3 (avoid magic numbers and avoid extra arithmetic)

    Deck(const bool& create_full = false);

    /** changes order of suits
        @throws std::invalid_argument if all suits are not given */
    void change_sort(const std::vector<Suit>& suits_in_order);

    void erase(const Card& card_to_remove);

    /** random */
    Card deal_one();

    /** pick n random cards */
    std::vector<Card> pick_random(int n) const;  // maybe only for test, maybe not

    void insert(const Card& card);

    bool is_empty() const { return card_count == 0; }

    const size_t& size() const { return card_count; }

    void clear();

    /** fill with 52 cards */
    void fill();

    /** @returns the number of cards of the specified suit */
    const size_t count(const Suit& suit) const { return cards[suit].size(); }

    const Card& at(int index) const;

    bool contains_non_points() const;  // TODO: ? this is messy / breaking oop (how many places will things have to change for alternate scoring rules?)
};

#endif // DECK_H_INCLUDED
