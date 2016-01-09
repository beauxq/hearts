#include "Card.h"

#include <string>

bool Card::beats_in_suit_of(const Card& lead) const
{
    return (suit == lead.suit) && (value > lead.value);
}

const std::string Card::str() const
{
    /** string representation of card
        both 1 and 14 are Ace */
    std::string to_return;

    switch (value)
    {
    case 1:
    case 14:
        to_return = "Ace of ";
        break;
    case 11:
        to_return = "Jack of ";
        break;
    case 12:
        to_return = "Queen of ";
        break;
    case 13:
        to_return = "King of ";
        break;
    default:
        to_return = std::to_string(value);
        to_return += " of ";
    }

    switch (suit)
    {
    case CLUBS:
        to_return += "Clubs";
        break;
    case DIAMONDS:
        to_return += "Diamonds";
        break;
    case SPADES:
        to_return += "Spades";
        break;
    case HEARTS:
        to_return += "Hearts";
        break;
    default:
        to_return += "UNKNOWN SUIT";
    }

    return to_return;
}
