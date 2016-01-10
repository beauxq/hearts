#ifndef CARD_H_INCLUDED
#define CARD_H_INCLUDED

#include <string>

enum Suit { CLUBS, DIAMONDS, SPADES, HEARTS, SUIT_COUNT };

// this class is essentially const
// because everything is either private or constructor or const
// (no setters)
class Card
{
private:
    int value;
    Suit suit;

public:
    // constructors
    Card() : value(0), suit(SUIT_COUNT) {}
    Card(int value_, Suit suit_) : value(value_), suit(suit_) {}

    // this breaks the rule against overloading operators when it's not obvious and intuitive
    // but I want to put this in an ordered container
    // (could build my own container, and it would probably be more efficient, but lazy... maybe later)
    bool operator<  (const Card& rhs) const { return value < rhs.value; }
    bool operator>= (const Card& rhs) const { return ! operator< (rhs); }
    bool operator>  (const Card& rhs) const { return rhs.operator< (*this); }
    bool operator<= (const Card& rhs) const { return ! rhs.operator< (*this); }

    bool operator== (const Card& rhs) const { return value == rhs.value && suit == rhs.suit; }
    bool operator!= (const Card& rhs) const { return ! operator== (rhs); }

    /** same suit and value > other.value */
    // to make this class work with other games, could add a trump parameter (default to no trump)
    bool beats_in_suit_of(const Card& lead) const;

    // getters
    const int& get_value() const { return value; }
    const Suit& get_suit() const { return suit; }

    /** string representation of card
        only for testing and debugging? */
    const std::string str() const;
};

#endif // CARD_H_INCLUDED
