#ifndef CARD_H_INCLUDED
#define CARD_H_INCLUDED

enum Suit { CLUBS, DIAMONDS, SPADES, HEARTS, SUIT_COUNT };

class Card
{
public:
    const int value;
    const Suit suit;

    // constructors
    Card() : value(0), suit(SUIT_COUNT) {}
    Card(int value_, Suit suit_) : value(value_), suit(suit_) {}

    bool operator== (const Card& rhs) const { return value == rhs.value && suit == rhs.suit; }
    bool operator!= (const Card& rhs) const { return ! operator== (rhs); }

    /** same suit and value > other.value */
    // to make this class work with other games, could add a trump parameter (default to no trump)
    bool beats_in_suit_of(const Card& lead) const;

    // TODO: str() string representation of card? (would only be for testing and debugging)
};

#endif // CARD_H_INCLUDED
