#include "Card.h"

bool Card::beats_in_suit_of(const Card& lead) const
{
    return (suit == lead.suit) && (value > lead.value);
}
/*
Card& Card::operator= (const Card& rhs)
{
    if (this != &rhs)
        Card(rhs);

    return *this;
}
*/
