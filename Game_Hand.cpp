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
