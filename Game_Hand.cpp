#include "Game_Hand.h"

#include <set>
#include <vector>

#include "Deck.h"

int Game_Hand::points_for(const Card& card)
{
    // TODO: magic numbers, scoring options
    if (card.get_suit() == HEARTS)
        return 1;
    if (card.get_value() == 12 && card.get_suit() == SPADES)
        return 13;
    return 0;
}

Game_Hand::Game_Hand() :
    hands(PLAYER_COUNT),
    unknown_cards_for_player(PLAYER_COUNT, true),
    passed_cards_to_player(PLAYER_COUNT),
    played_cards(PLAYER_COUNT)
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        player_is_human[i] = !i;  // player 0 human, rest not
    }
}

void Game_Hand::reset_hand()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        hands[i].clear();
        unknown_cards_for_player[i].fill();
        scores[i] = 0;
        passed_cards_to_player[i].clear();
        hearts_broken = false;
    }
}

void Game_Hand::deal_hands()
{
    Deck deck(true);

    Card dealt;
    while (! deck.is_empty())
    {
        for (int i = 0; i < PLAYER_COUNT; ++i)
        {
            dealt = deck.deal_one();  // TODO: maybe can get rid of this if insert returns an iterator
            hands[i].insert(dealt);
            unknown_cards_for_player[i].erase(dealt);

            if (dealt == STARTING_CARD)
                whose_turn = i;
        }
    }
}

void Game_Hand::pass(const int& from_player, const int& to_player, const std::vector<Card>& passed_cards)
{
    for (auto itr = passed_cards.begin(); itr != passed_cards.end(); ++itr)
    {
        passed_cards_to_player[to_player].push_back(*itr);
        hands[from_player].erase(*itr);
    }
}

void Game_Hand::receive_passed_cards()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        for (auto itr = passed_cards_to_player[i].begin(); itr != passed_cards_to_player[i].end(); ++itr)
        {
            hands[i].insert(*itr);
            // TODO: remove from unknown cards

            if (*itr == STARTING_CARD)
                whose_turn = i;
        }
    }
}

void Game_Hand::reset_trick()
{
    played_card_count = 0;
    trick_leader = whose_turn;
}

void Game_Hand::play_card(const Card& card)
{
    // TODO: remove from unknown cards of all players

    hands[whose_turn].erase(card);
    played_cards[whose_turn] = card;
    ++played_card_count;

    if (card.beats_in_suit_of(played_cards[trick_leader]))
        trick_leader = whose_turn;

    if (card.get_suit() == HEARTS)
        hearts_broken = true;  // TODO: alternate rule, queen of spades breaks hearts

    whose_turn = (whose_turn + 1) % PLAYER_COUNT;
}

void Game_Hand::end_trick()
{
    for (auto card_itr = played_cards.begin(); card_itr != played_cards.end(); ++card_itr)
    {
        scores[trick_leader] += points_for(*card_itr);
    }
    whose_turn = trick_leader;

    // TODO: history of tricks?
}

void Game_Hand::end_hand()
{
    // TODO: shoot the moon points
}

void Game_Hand::find_valid_choices(std::vector<Card>& valid_choices)
{
    /*
    O(n) for a function that will be called millions of times per game
    efficiency is important
    I made this chart trying to work out the best organization

    key:
    lead - I am first player of trick
    first - first trick of the hand
    hb - hearts already broken

    l - whether I have suit matching lead suit matters
    h - whether I have non-hearts matters
    p - whether I have non-points matters
    - - nothing matters

    2c - two of clubs

                            3-dimensional table
                lead y	lead no
        first y	-(2c)	l p
        hb n	h	    l
        hb y	-	    l

                            list
                                        T(n) (if going through cards to see what matters, then going through to get valid cards)
        lead y first y hb y     -		0 (impossible)
        lead y first y hb n     -   	1 (2c)
        lead y first n hb y     -       n
        lead y first n hb n     h		2n
        lead n first y hb y     -		0 (impossible)
        lead n first y hb n     l p		2n	if l then p done (l can stop first itr, p can't)
        lead n first n hb y     l       2n
        lead n first n hb n     l		2n
                                        (but not iterating twice because have sets of suits)

    lead n   happens 3 times as often as  lead y
    first n  happens thousands of times more often than  first y
    hb is probably about equal y and n
    */

    // bool found_non_points = false;
    // bool found_non_hearts = false;
    // bool found_lead_suit = false;
    // bool points_allowed;
    // bool hearts_allowed;
    // bool non_lead_suit_allowed;

    // this might look long and messy, but I think it's the most efficient
    // here we go...
    if (hands[whose_turn].size() == 13)  // first trick
    {
        if (played_card_count == 0)  // first player
        {
            valid_choices.push_back(STARTING_CARD);  // two of clubs
            return;  // nothing else
        }
        // not first player
        if (hands[whose_turn].count(CLUBS))  // must match lead suit (clubs)
        {
            for (auto itr = hands[whose_turn].suit_begin(CLUBS); itr != hands[whose_turn].suit_end(CLUBS); ++itr)
            {
                valid_choices.push_back(*itr);
            }
        }
        else  // don't have any to match lead suit
        {
            if (hands[whose_turn].contains_non_points())  // have non-points
            {
                for (auto itr = hands[whose_turn].begin(); itr != hands[whose_turn].end(); ++itr)  // play anything (non-points)
                {
                    if (! points_for(*itr))
                    {
                        valid_choices.push_back(*itr);
                    }
                }
            }
            else  // no non-points (no match for lead suit)
            {
                for (auto itr = hands[whose_turn].begin(); itr != hands[whose_turn].end(); ++itr)  // play anything
                {
                    valid_choices.push_back(*itr);
                }
            }
        }
    }
    else  // not first trick
    {
        if (played_card_count == 0)  // first player
        {
            if (hearts_broken)  // hearts broken
            {
                for (auto itr = hands[whose_turn].begin(); itr != hands[whose_turn].end(); ++itr)  // play anything
                {
                    valid_choices.push_back(*itr);
                }
            }
            else  // hearts not broken
            {
                if (hands[whose_turn].count(HEARTS) == hands[whose_turn].size())  // only hearts in hand
                {
                    for (auto itr = hands[whose_turn].suit_begin(HEARTS); itr != hands[whose_turn].suit_end(HEARTS); ++itr)  // play anything (hearts)
                    {
                        valid_choices.push_back(*itr);
                    }
                }
                else  // non-hearts in hand, hearts not allowed
                {
                    // anything from 3 other suits
                    for (auto itr = hands[whose_turn].suit_begin(CLUBS); itr != hands[whose_turn].suit_end(CLUBS); ++itr)
                    {
                        valid_choices.push_back(*itr);
                    }
                    for (auto itr = hands[whose_turn].suit_begin(DIAMONDS); itr != hands[whose_turn].suit_end(DIAMONDS); ++itr)
                    {
                        valid_choices.push_back(*itr);
                    }
                    for (auto itr = hands[whose_turn].suit_begin(SPADES); itr != hands[whose_turn].suit_end(SPADES); ++itr)
                    {
                        valid_choices.push_back(*itr);
                    }
                    // TODO: this is the only time in this function that we get something from more than one suit
                    // without iterating over the entire hand
                    // it may be useful to have the vector in the same order as iterating over the entire hand
                    // so this may need to change to iterate over the entire hand (since the order of suits can change)
                }
            }
        }
        else  // not first player (and not first trick)
        {
            if (hands[whose_turn].count(played_cards[trick_leader].get_suit()))  // must match lead suit
            {
                for (auto itr  = hands[whose_turn].suit_begin(played_cards[trick_leader].get_suit());
                          itr != hands[whose_turn].suit_end  (played_cards[trick_leader].get_suit());
                        ++itr)
                {
                    valid_choices.push_back(*itr);
                }
            }
            else  // don't have matching suit
            {
                for (auto itr = hands[whose_turn].begin(); itr != hands[whose_turn].end(); ++itr)  // play anything
                {
                    valid_choices.push_back(*itr);
                }
            }
        }
    }
}
