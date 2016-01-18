#include "Game_Hand.h"

#include <set>
#include <vector>
#include <iostream>  // just for debugging  TODO: remove include
#include <algorithm>  // std::find

#include "Card.h"
#include "Deck.h"

int Game_Hand::points_for(const Card& card) const
{
    // TODO: magic numbers, scoring options
    // note: this came out as one of the top functions in a code profiling, so don't make it too complex
    if (card.get_suit() == HEARTS)
        return 1;
    if (card.get_value() == 12 && card.get_suit() == SPADES)
        return 13;
    return 0;
}

void Game_Hand::speculate_hands(const int& player_speculating, const int& passing_direction)
{
    /** replace hands with player's guess about the other players' hands
        (so the computer doesn't cheat)
        to be called in a simulation */
    std::vector<Deck> speculated_hands(PLAYER_COUNT);
    std::vector<int> space_remaining_in(PLAYER_COUNT);

    // variables used for distributing cards:
    std::vector<std::unordered_set<int> > hands_that_allow_suit(SUIT_COUNT);  // index is suit  // TODO
    std::unordered_set<int> non_full_hands;
    std::vector<int> allow_suit_and_non_full;  // intersection of non-full-hands with hands that allow this suit

    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        space_remaining_in[i] = hands[i].size();
    }

    // I know my own hand
    space_remaining_in[player_speculating] = 0;

    // cards I passed to someone
    // (this section should do nothing ( space remaining -= 0 ) when called for passing ai)
    const int WHOM_I_PASSED_TO = (player_speculating + passing_direction) % PLAYER_COUNT;
    space_remaining_in[WHOM_I_PASSED_TO] -= passed_cards_to_player[WHOM_I_PASSED_TO].size();

    // prepare to distribute unknown cards
    // which hands are not full
    for (int player = 0; player < PLAYER_COUNT; ++player)
    {
        if (space_remaining_in[player] > 0)
            non_full_hands.insert(player);
    }

    // which hands can hold which suits (exclude player_speculating, hand that is not speculated)
    for (int suit = 0; suit < SUIT_COUNT; ++suit)
    {
        for (int player = 0; player < PLAYER_COUNT; ++player)
        {
            if (player_seen_void_in_suits[player].count(suit) == 0)
            {
                if (player != player_speculating)  // (exclude player_speculating, hand that is not speculated)
                {
                    hands_that_allow_suit[suit].insert(player);
                }
            }
            /*
            else  // player is void in suit TODO: this else is just for testing, remove
            {
                if (player != player_speculating)  // (exclude player_speculating, hand that is not speculated)
                {
                    std::cout << "    player " << player << " has no ";
                    switch (suit)
                    {
                    case CLUBS:
                        std::cout << "clubs\n";
                        break;
                    case DIAMONDS:
                        std::cout << "diamonds\n";
                        break;
                    case SPADES:
                        std::cout << "spades\n";
                        break;
                    case HEARTS:
                        std::cout << "hearts\n";
                    }
                }
            }
            */
        }  // players
    }  // suits

    // distribute unknown cards
    /*
    Algorithm for distributing unknown cards

    For each card in the unknown cards: ( *this_card_itr "this card" )

        If this card fits and is allowed in a number of hands > 0:
            Choose one of those hands at random and put this card in that hand.

        else (this card is not allowed in any hand that it fits in):
            Make a list of suits that can go in hands that are not full. ( suits_allowed_in_non_full )
            Make a list of cards of those suits that are in hands that this card is allowed in. ( possible_cards_to_swap )
            If that 2nd list is length 0 (There are no cards of suits from the 1st list in the hand that allows the suit of this card):
                ASSERT: (I don't have a formal proof for this, but I think it is true.)
                        There is a 3rd hand that is not involved in the previously used hands in this algorithm.
                        One hand is full and is the only one that allows this card.
                        A second hand is the only one not full and doesn't allow this card or any card in the first hand.
                        The 3rd hand is full and must allow some suit that is in the first hand and must have some suit that is allowed in the second hand.
                Choose, at random, a card from the 3rd hand cards of suits that fit in the 2nd hand, and move that card to the 2nd hand.
                Choose, at random, a card from the 1st hand cards of suits that fit in the 3rd hand, and move that card to the 3rd hand.
                Put "this card" (finally) in the 1st hand.
            else:
                Choose a card randomly from that list, and move it to a random hand that is not full.
                Put "this card" in a hand that allows it.
    Next card.
    */
    for (auto this_card_itr = unknown_cards_for_player[player_speculating].begin();
         this_card_itr != unknown_cards_for_player[player_speculating].end();
         ++this_card_itr)
    {
        // intersection of non_full and allow suit
        allow_suit_and_non_full.clear();
        for (auto allow_itr = hands_that_allow_suit[this_card_itr->get_suit()].begin();
             allow_itr != hands_that_allow_suit[this_card_itr->get_suit()].end();
             ++allow_itr)
        {
            if (non_full_hands.count(*allow_itr))
                allow_suit_and_non_full.push_back(*allow_itr);
        }

        if (allow_suit_and_non_full.size() > 0)
        {
            int which_player_to_give_this_card_to = allow_suit_and_non_full[rand() % allow_suit_and_non_full.size()];
            speculated_hands[which_player_to_give_this_card_to].insert(*this_card_itr);
            if (--space_remaining_in[which_player_to_give_this_card_to] == 0)  // decrement space remaining and see if it's full
                non_full_hands.erase(which_player_to_give_this_card_to);  // now full
        }
        else  // there are no hands left that allow this suit and have space for this card
        {
            // so we have to do some swapping
            // find which suits are allowed in non-full hands
            std::unordered_set<int> suits_allowed_in_non_full;
            for (auto non_full_itr = non_full_hands.begin(); non_full_itr != non_full_hands.end(); ++non_full_itr)
            {
                for (int suit = 0; suit < SUIT_COUNT; ++suit)
                {
                    if (hands_that_allow_suit[suit].count(*non_full_itr))
                        suits_allowed_in_non_full.insert(suit);
                }
            }
            // find possible cards to swap (out of those suits)
            std::vector<Card> possible_cards_to_swap;
            std::vector<int> which_hand_to_pull_that_card_from;
            // go through the hands that "this card" is allowed in
            for (auto allow_itr = hands_that_allow_suit[this_card_itr->get_suit()].begin();
                 allow_itr != hands_that_allow_suit[this_card_itr->get_suit()].end();
                 ++allow_itr)
            {
                // go through the cards in this hand
                for (auto this_card_target_hand_itr = speculated_hands[*allow_itr].begin();
                     this_card_target_hand_itr != speculated_hands[*allow_itr].end();
                     ++this_card_target_hand_itr)
                {
                    // can this card go in a non-full hand?
                    if (suits_allowed_in_non_full.count(this_card_target_hand_itr->get_suit()))
                    {
                        possible_cards_to_swap.push_back(*this_card_target_hand_itr);
                        which_hand_to_pull_that_card_from.push_back(*allow_itr);
                    }
                }
            }

            if (possible_cards_to_swap.size() > 0)
            {
                // do the swap
                int card_index_to_swap = rand() % possible_cards_to_swap.size();
                // intersection of non_full and allow swap suit
                std::vector<int> allow_swap_suit_and_non_full;
                for (auto allow_itr = hands_that_allow_suit[possible_cards_to_swap[card_index_to_swap].get_suit()].begin();
                     allow_itr != hands_that_allow_suit[possible_cards_to_swap[card_index_to_swap].get_suit()].end();
                     ++allow_itr)
                {
                    if (non_full_hands.count(*allow_itr))
                        allow_swap_suit_and_non_full.push_back(*allow_itr);
                }

                // assert allow_swap_suit_and_non_full.size() > 0  // TODO: remove (or set up debugging)
                if (allow_swap_suit_and_non_full.size() == 0)
                    std::cout << "ERROR: assertion fail, single swap, no place to put swap card\n";

                int which_player_to_give_this_card_to = allow_swap_suit_and_non_full[rand() % allow_swap_suit_and_non_full.size()];
                speculated_hands[which_player_to_give_this_card_to].insert(possible_cards_to_swap[card_index_to_swap]);
                speculated_hands[which_hand_to_pull_that_card_from[card_index_to_swap]].erase(possible_cards_to_swap[card_index_to_swap]);
                // and this card in the space that was made for it
                speculated_hands[which_hand_to_pull_that_card_from[card_index_to_swap]].insert(*this_card_itr);
                // only the size of the non-full hand changed
                if (--space_remaining_in[which_player_to_give_this_card_to] == 0)  // decrement space remaining and see if it's full
                    non_full_hands.erase(which_player_to_give_this_card_to);  // now full
            }
            else  // no single swap possible, double swap is needed
            {
                // std::cout << "entering double swap scenario\n";
                // assert: double swap is possible
                /* questionable assertion:
                    There is a 3rd hand that is not involved in the previously used hands in this algorithm.
                    One hand is full and is the only one that allows *this_card_itr
                    A second hand is the only one not full and doesn't allow this card or any card in the first hand.
                    The 3rd hand is full and must allow some suit that is in the first hand
                                         and must have some suit that is allowed in the second hand. */
                int full_and_allows_this_card = *(hands_that_allow_suit[this_card_itr->get_suit()].begin());  // "1st hand"
                int non_full_hand = *(non_full_hands.begin());  // "2nd hand" suits_allowed_in_non_full
                // which hand is third hand?
                int third_hand = 0;
                while ((third_hand == full_and_allows_this_card) ||  // first hand
                       (third_hand == player_speculating) ||  // the hand we're not speculating
                       (third_hand == non_full_hand))  // second hand
                {
                    ++third_hand;
                }

                // debugging assertions  // TODO: remove
                if (hands_that_allow_suit[this_card_itr->get_suit()].size() != 1)
                    std::cout << "ERROR: big assertion was wrong, hands that allow this suit != 1\n";  // !
                if (non_full_hands.size() != 1)
                    std::cout << "ERROR: big assertion was wrong, not exactly 1 non-full hand\n";

                // go through third hand and find cards allowed in non-full hand
                std::vector<Card> second_level_swap_possibilities;
                for (auto third_hand_itr = speculated_hands[third_hand].begin();
                     third_hand_itr != speculated_hands[third_hand].end();
                     ++third_hand_itr)
                {
                    if (suits_allowed_in_non_full.count(third_hand_itr->get_suit()))
                        second_level_swap_possibilities.push_back(*third_hand_itr);
                }

                // another of the debugging assertions  // TODO: remove
                if (second_level_swap_possibilities.size() == 0)
                    std::cout << "ERROR: big assertion was wrong, no cards to move from 3rd to 2nd\n";

                // move one of those cards to the non-full hand
                Card second_level_swap = second_level_swap_possibilities[rand() % second_level_swap_possibilities.size()];
                speculated_hands[non_full_hand].insert(second_level_swap);
                speculated_hands[third_hand].erase(second_level_swap);
                // change space remaining
                if (--space_remaining_in[non_full_hand] == 0)  // decrement space remaining and see if it's full
                    non_full_hands.erase(non_full_hand);  // now full (we're on the last card)

                // which suits are allowed in the third hand?
                std::unordered_set<int> suits_allowed_in_third_hand;
                for (int suit = 0; suit < SUIT_COUNT; ++suit)
                {
                    if (player_seen_void_in_suits[third_hand].count(suit) == 0)  // then this suit is allowed
                        suits_allowed_in_third_hand.insert(suit);
                }

                // find cards that can be moved from first hand to third hand
                // possible_cards_to_swap is already declared from way before and is empty
                for (auto first_hand_itr = speculated_hands[full_and_allows_this_card].begin();
                     first_hand_itr != speculated_hands[full_and_allows_this_card].end();
                     ++first_hand_itr)
                {
                    if (suits_allowed_in_third_hand.count(first_hand_itr->get_suit()))
                        possible_cards_to_swap.push_back(*first_hand_itr);
                }

                // another of the debugging assertions  // TODO: remove
                if (possible_cards_to_swap.size() == 0)
                    std::cout << "ERROR: big assertion was wrong, no cards to move from 1st to 3rd\n";

                // move one of those cards to the third hand
                Card first_level_swap = possible_cards_to_swap[rand() % possible_cards_to_swap.size()];
                speculated_hands[third_hand].insert(first_level_swap);
                speculated_hands[full_and_allows_this_card].erase(first_level_swap);
                // so now it's not full and has space for this card
                // (it will be full right after inserting this card, so don't change space remaining)

                // Now, finally, we can get rid of this card
                speculated_hands[full_and_allows_this_card].insert(*this_card_itr);
            }  // done with double swap
        }  // done with any swapping
    }  // next card

    // put in the cards that I already knew
    speculated_hands[player_speculating] = hands[player_speculating];
    for (auto itr = passed_cards_to_player[WHOM_I_PASSED_TO].begin();
         itr != passed_cards_to_player[WHOM_I_PASSED_TO].end();
         ++itr)
    {
        speculated_hands[WHOM_I_PASSED_TO].insert(*itr);
    }

    // replace real hands with speculated hands
    hands = speculated_hands;
}

Game_Hand::Game_Hand() :
    hands(PLAYER_COUNT),
    passed_cards_to_player(PLAYER_COUNT),
    played_cards(PLAYER_COUNT),
    points_played_this_trick(false),
    shoot_moon_possible(true),
    this_is_simulation(false),
    unknown_cards_for_player(PLAYER_COUNT, true)
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        player_is_human[i] = !i;  // player 0 human, rest not
    }
}

const std::vector<Card>& Game_Hand::get_played_cards()
{
    // std::cout << "get_played_cards: whose_turn = " << whose_turn << std::endl;
    // put null cards where cards haven't been played
    int player = whose_turn;
    for (int handled = played_card_count; handled < PLAYER_COUNT; ++handled)
    {
        // std::cout << "putting null at " << player << std::endl;
        played_cards[player] = Card();
        // std::cout << "value there: " << played_cards[player].get_value() << std::endl;
        player = (player + 1) % PLAYER_COUNT;
    }

    return played_cards;
}

void Game_Hand::reset_hand()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        hands[i].clear();
        unknown_cards_for_player[i].fill();
        scores[i] = 0;
        passed_cards_to_player[i].clear();
        player_seen_void_in_suits[i].clear();
    }

    pass_count = 0;
    hearts_broken = false;
    shoot_moon_possible = true;
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
    //std::cout << "in pass function vector size " << passed_cards.size() << std::endl;
    //std::cout << "passed_cards_to_player[to_player].size() " << passed_cards_to_player[to_player].size() << std::endl;
    //std::cout << "hands[from_player].size() after passing in pass function " << hands[from_player].size() << std::endl;
    for (auto itr = passed_cards.begin(); itr != passed_cards.end(); ++itr)
    {
        passed_cards_to_player[to_player].push_back(*itr);
        hands[from_player].erase(*itr);
    }
    //std::cout << "passed_cards_to_player[to_player].size() " << passed_cards_to_player[to_player].size() << std::endl;
    //std::cout << "hands[from_player].size() after passing in pass function " << hands[from_player].size() << std::endl;
    ++pass_count;
}

void Game_Hand::receive_passed_cards()
{
    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        for (auto itr = passed_cards_to_player[i].begin(); itr != passed_cards_to_player[i].end(); ++itr)
        {
            hands[i].insert(*itr);
            unknown_cards_for_player[i].erase(*itr);  // remove from unknown cards

            if (*itr == STARTING_CARD)
                whose_turn = i;
        }
    }
}

void Game_Hand::reset_trick()
{
    played_card_count = 0;
    trick_leader = whose_turn;
    points_played_this_trick = false;
}

void Game_Hand::play_card(const Card& card)
{
    hands[whose_turn].erase(card);
    played_cards[whose_turn] = card;
    ++played_card_count;
    if (! points_played_this_trick)
        points_played_this_trick = points_for(card);

    if (! this_is_simulation)
    {
        // remove from unknown cards of all players
        for (int player = 0; player < PLAYER_COUNT; ++player)
        {
            unknown_cards_for_player[player].erase(card);
        }
        // remove from passed cards (AI players remembering what cards they passed)
        auto position = std::find(passed_cards_to_player[whose_turn].begin(),
                                  passed_cards_to_player[whose_turn].end(),
                                  card);
        if (position != passed_cards_to_player[whose_turn].end())
            passed_cards_to_player[whose_turn].erase(position);

        // is this player showing that they have none of a suit?
        if (card.get_suit() != played_cards[trick_leader].get_suit())
            player_seen_void_in_suits[whose_turn].insert(played_cards[trick_leader].get_suit());

        // is this player showing that they only have hearts left?
        if ((card.get_suit() == HEARTS) &&
            (played_card_count == 1) &&
            (! hearts_broken))
        {
            player_seen_void_in_suits[whose_turn].insert(CLUBS);
            player_seen_void_in_suits[whose_turn].insert(DIAMONDS);
            player_seen_void_in_suits[whose_turn].insert(SPADES);
        }
    }

    if (card.beats_in_suit_of(played_cards[trick_leader]))
        trick_leader = whose_turn;

    // see if it's still possible to shoot the moon
    // TODO: can this be more efficient? this function is high in the profiler
    // maybe only check each trick if it's still possible to shoot the moon?
    if (shoot_moon_possible)
    {
        // find out if still possible
        if (points_played_this_trick)
        {
            int player_with_non_zero_score = -1;
            for (int player = 0; player < PLAYER_COUNT; ++player)
            {
                if (scores[player])
                {
                    player_with_non_zero_score = player;
                    break;
                }
            }

            if (player_with_non_zero_score > -1)
            {
                // see whether this player has played yet this trick
                bool this_player_played = false;
                int going_through_turns = (whose_turn + 5 - played_card_count) % PLAYER_COUNT;  // first turn this trick
                for (int turn_count = played_card_count; turn_count > 0; --turn_count)
                {
                    if (going_through_turns == player_with_non_zero_score)
                    {
                        this_player_played = true;
                        break;
                    }
                    going_through_turns = (going_through_turns + 1) % PLAYER_COUNT;
                }

                if (this_player_played)
                {
                    // this player can't take the trick?
                    if (player_with_non_zero_score != trick_leader)
                    {
                        shoot_moon_possible = false;

                        // TODO: remove - for testing
                        if (! this_is_simulation)
                            std::cout << "this is the point when it becomes impossible for anyone to shoot the moon\n";
                    }
                    // he is leading, still possible to shoot the moon
                }
                // else still possible to shoot the moon
            }
            // else still possible to shoot the moon
        }
        // else still possible to shoot the moon
    }

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

int Game_Hand::end_hand()
{
    /** @returns who shot the moon, -1 if no one shot the moon */
    // shoot the moon points
    for (int win_player = 0; win_player < PLAYER_COUNT; ++win_player)  // check if player got 26
    {
        if (scores[win_player] == 26)  // this player shot the moon
        {
            scores[win_player] = 0;
            for (int other_player = 0; other_player < PLAYER_COUNT; ++other_player)
            {
                if (other_player != win_player)
                    scores[other_player] = 26;
            }
            return win_player;
        }
        else if (scores[win_player] > 0)  // 1 to 25 points
            return -1;  // no one shot the moon
    }
    std::cerr << "invalid scores at the end of a hand\n";
    return -2;
}

void Game_Hand::find_valid_choices(std::vector<Card>& valid_choices) const
{
    /*
    Much of this code is copied to find_valid_choice_rule.
    Keep the two functions in sync.

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
                for (auto itr = hands[whose_turn].begin(); itr != hands[whose_turn].end(); ++itr)  // play any non-points
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

void Game_Hand::find_valid_choice_rule(std::string& rule) const
{
    /*
    Much of this code is copied from find_valid_choices.
    Keep the two functions in sync.

    It is put in two different places for efficiency.
    find_valid_choices is called millions of times per game,
    so I don't want to put more in it than has to be in it.
    */
    const std::string FOLLOW_SUIT_RULE("You must play the same suit that was led.");  // because it's used more than once

    if (hands[whose_turn].size() == 13)  // first trick
    {
        if (played_card_count == 0)  // first player
        {
            rule = "The two of clubs must be played first.";  // two of clubs
            return;  // nothing else
        }
        // not first player
        if (hands[whose_turn].count(CLUBS))  // must match lead suit (clubs)
        {
            rule = FOLLOW_SUIT_RULE;
        }
        else  // don't have any to match lead suit
        {
            if (hands[whose_turn].contains_non_points())  // have non-points
            {
                rule = "Points are not allowed on the first trick.";
            }
            else  // no non-points (no match for lead suit)
            {
                // play anything
            }
        }
    }
    else  // not first trick
    {
        if (played_card_count == 0)  // first player
        {
            if (hearts_broken)  // hearts broken
            {
                // play anything
            }
            else  // hearts not broken
            {
                if (hands[whose_turn].count(HEARTS) == hands[whose_turn].size())  // only hearts in hand
                {
                    // play anything
                }
                else  // non-hearts in hand, hearts not allowed
                {
                    // anything from 3 other suits
                    rule = "Hearts may not be led until they have been played.";
                }
            }
        }
        else  // not first player (and not first trick)
        {
            if (hands[whose_turn].count(played_cards[trick_leader].get_suit()))  // must match lead suit
            {
                rule = FOLLOW_SUIT_RULE;
            }
            else  // don't have matching suit
            {
                // play anything
            }
        }
    }
}

Card Game_Hand::static_play_ai() const
{
    /** this will not work with alternative scoring rules involving other cards (jack of diamonds)
        it could crash */

    std::vector<Card> valid_choices;
    find_valid_choices(valid_choices);

    if (played_card_count > 0)  // I'm not leading the trick
    {
        if (valid_choices[0].get_suit() == played_cards[trick_leader].get_suit())  // I have to follow suit
        {
            if (valid_choices[0].get_value() < played_cards[trick_leader].get_value())  // I can play under, avoid taking it
            {
                // find highest card I can avoid taking it with
                for (int i = valid_choices.size() - 1; i >= 0; --i)
                {
                    if (valid_choices[i].get_value() < played_cards[trick_leader].get_value())  // this is highest card I can avoid taking it with
                    {
                        // Q of Spades instead of K of Spades
                        if (valid_choices[i].get_value() == 13 &&  // King
                            valid_choices[i].get_suit() == SPADES &&  // of Spades
                            i > 0 &&  // I have a lower Spade
                            valid_choices[i-1].get_value() == 12)  // and it's the Queen
                        {
                            return valid_choices[i-1];  // play Queen of Spades
                        }
                        else  // not king and queen of spades
                        {
                            return valid_choices[i];  // highest card I can avoid taking it with
                        }
                    }
                }
            }
            else  // I can't play under
            {
                if (played_card_count < 3)  // someone else will play after me
                {
                    if (played_cards[trick_leader].get_suit() == SPADES)  // spades
                    {
                        if (valid_choices[0].get_value() == 12 &&  // queen
                            valid_choices.size() > 1)  // and I have something else  // TODO: is this code ever run with size == 1?
                        {
                            return valid_choices[1];
                        }
                        else  // not queen or I don't have anything else
                        {
                            return valid_choices[0];
                        }
                    }
                    else  // not spades
                    {
                        return valid_choices[0];  // lowest card
                    }
                }
                else  // no one else plays after me
                {
                    if (played_cards[trick_leader].get_suit() == SPADES)  // spades
                    {
                        if (valid_choices[valid_choices.size() - 1].get_value() == 12 &&  // queen
                            valid_choices.size() > 1)  // and I have something else  // TODO: is this code ever run with size == 1?
                        {
                            return valid_choices[valid_choices.size() - 2];  // highest except queen
                        }
                        else  // not queen or I don't have anything else
                        {
                            return valid_choices[valid_choices.size() - 1];  // highest
                        }
                    }
                    else  // not spades
                    {
                        return valid_choices[valid_choices.size() - 1];  // highest card
                    }
                }
            }
        }
        else  // don't have to follow suit (and not leading)
        {
            // GET RID OF THE QUEEN!!
            // (play lowest of high spades)
            std::vector<Card> high_spades;  // ordered from highest to lowest
            for (int i = valid_choices.size() - 1; i >= 0; --i)
            {
                if (valid_choices[i].get_suit() != SPADES)
                    continue;
                if (valid_choices[i].get_value() > 11)  // higher than Jack
                    high_spades.push_back(valid_choices[i]);
                else  // spade, lower than queen
                    break;
            }
            if (high_spades.size() > 0)  // I have high spades
            {
                return high_spades[high_spades.size() - 1];  // play lowest high spade
            }
            else  // I don't have any high spades
            {
                // play the highest card in the suit of the highest lowest card of its suit
                // (yes, you understood that)
                // algorithm:
                // look at the lowest card in each suit
                // which one of those is the highest?
                // what's the suit of that card?
                // play the highest card in that suit

                Suit suit_currently_looking_through = valid_choices[0].get_suit();
                Suit suit_of_the_highest_lowest_card_of_its_suit = valid_choices[0].get_suit();
                int lowest_value_in_that_suit = valid_choices[0].get_value();

                for (unsigned int i = 1; i < valid_choices.size(); ++i)
                {
                    if (valid_choices[i].get_suit() != suit_currently_looking_through)  // moved into a new suit
                    {
                        suit_currently_looking_through = valid_choices[i].get_suit();

                        if (valid_choices[i].get_value() /* lowest value in this suit */ >
                            lowest_value_in_that_suit)  // found one higher
                        {
                            suit_of_the_highest_lowest_card_of_its_suit = suit_currently_looking_through;
                            lowest_value_in_that_suit = valid_choices[i].get_value();
                        }
                    }
                }
                // now we know the suit of the highest lowest card of its suit
                // since we don't have any high spades, any card of this suit should be a valid choice
                // so we can pull it out of the hand (instead of the valid_choices)
                auto set_itr = hands[whose_turn].suit_rbegin(suit_of_the_highest_lowest_card_of_its_suit);
                return *set_itr;
            }
        }
    }
    else  // I lead
    {
        bool found_non_high_spade = false;
        for (unsigned int i = 0; i < valid_choices.size(); ++i)
        {
            if (valid_choices[i].get_value() < 12 || valid_choices[i].get_suit() != SPADES)  // found non "high spade"
            {
                found_non_high_spade = true;
                break;
            }
        }
        if (! found_non_high_spade)  // only high spades available
            return valid_choices[0];  // play lowest high spade
        // random, but not high spade
        int size_after_high_spades_swapped_out = valid_choices.size();
        int random_choice;
        while (true)
        {
            random_choice = rand() % size_after_high_spades_swapped_out;
            if (valid_choices[random_choice].get_value() > 11 && valid_choices[random_choice].get_suit() == SPADES)  // high spade
            {
                // swap high spade into last spot
                --size_after_high_spades_swapped_out;
                std::swap(valid_choices[random_choice], valid_choices[size_after_high_spades_swapped_out]);
                continue;
            }
            else  // not high spade
                return valid_choices[random_choice];
        }

    }
    return Card();  // just to avoid warning message about control reaching end of non-void function

    /* this strategy in Python:
        if must_choose_trick_suit:  # I'm not leading the trick
            # see if I can avoid taking it
            can_avoid_taking_it = False
            for index in valid_choices:
                if self.players_hands[self.turn][index] < self.played_cards[self.taker()]:
                    can_avoid_taking_it = True
                    try:  # might be first one can avoid taking it with
                        if self.players_hands[self.turn][can_avoid_taking_it_with].value != 12 or \
                                self.players_hands[self.turn][can_avoid_taking_it_with].suit != SUITS[2]:
                            # don't change to king of spades if it's queen of spades
                            can_avoid_taking_it_with = index
                    except NameError:  # first one
                        can_avoid_taking_it_with = index  # after for loop, will be the highest because sorted
            if can_avoid_taking_it:
                answer = can_avoid_taking_it_with
            else:  # can't avoid taking it, unless...
                if self.played_cards.count(None) > 1:  # someone else will play after me
                    if self.trick_suit != SUITS[2]:  # not spades
                        answer = valid_choices[0]  # lowest
                    else:  # is spades
                        if self.players_hands[self.turn][valid_choices[0]].value == 12 and \
                                self.players_hands[self.turn][valid_choices[0]].suit == SUITS[2]:  # Q of S
                            answer = valid_choices[1]  # second lowest
                        else:  # lowest is not Queen of Spades
                            answer = valid_choices[0]  # play lowest
                else:  # can't avoid taking it, no one else plays after me
                    if self.trick_suit != SUITS[2]:  # not spades
                        answer = valid_choices[-1]  # highest
                    else:  # is spades
                        if self.players_hands[self.turn][valid_choices[-1]].value == 12 and \
                                self.players_hands[self.turn][valid_choices[-1]].suit == SUITS[2]:  # Q of S
                            answer = valid_choices[-2]  # second highest
                        else:  # highest is not Queen of Spades
                            answer = valid_choices[-1]
        else:  # I don't have to follow suit
            if self.played_cards.count(None) == 4:  # I'm leading
                only_high_spades_available = True
                valid_choices_minus_high_spades = []
                for index in valid_choices:
                    if self.players_hands[self.turn][index].suit != SUITS[2] or \
                            self.players_hands[self.turn][index].value not in [12, 13, 1]:
                        only_high_spades_available = False
                        valid_choices_minus_high_spades.append(index)
                if only_high_spades_available:
                    answer = valid_choices[0]  # lowest high spade
                else:  # valid choices are not all high spades  todo: maybe something better here?
                    answer = random.choice(valid_choices_minus_high_spades)
            else:  # not leading, don't have to follow suit, not taking it
                # GET RID OF THE QUEEN!!!
                i_have_high_spades = False
                high_spades_choices = []
                for index in valid_choices:
                    if self.players_hands[self.turn][index].suit == SUITS[2] and \
                            self.players_hands[self.turn][index].value in [12, 13, 1]:
                        i_have_high_spades = True
                        high_spades_choices.append(index)
                if i_have_high_spades:
                    answer = high_spades_choices[0]  # lowest of the high spades
                else:  # I don't have high spades
                    suit_where_my_lowest_card_is_the_highest = self.players_hands[self.turn][valid_choices[0]].suit
                    lowest_in_suit = valid_choices[0]
                    highest_in_suit = valid_choices[0]
                    current_suit_going_through = suit_where_my_lowest_card_is_the_highest
                    # todo: check this part  v
                    for index in valid_choices:
                        if self.players_hands[self.turn][index].suit != current_suit_going_through:
                            current_suit_going_through = self.players_hands[self.turn][index].suit
                            # this will be the lowest card in this new suit:
                            if not (self.players_hands[self.turn][index] <  # not < in place of >= because overloading
                                    self.players_hands[self.turn][lowest_in_suit]):
                                suit_where_my_lowest_card_is_the_highest = self.players_hands[self.turn][index].suit
                                lowest_in_suit = index
                                highest_in_suit = index
                        else:  # still in same suit
                            if current_suit_going_through == suit_where_my_lowest_card_is_the_highest:
                                if self.players_hands[self.turn][index] > \
                                        self.players_hands[self.turn][highest_in_suit]:
                                    highest_in_suit = index
                    answer = highest_in_suit

        return answer
    */
}

Card Game_Hand::dynamic_play_ai(const int& passing_direction) const
{
    std::vector<Card> valid_choices;
    find_valid_choices(valid_choices);

    // only one choice?
    if (valid_choices.size() == 1)
        return valid_choices[0];

    std::vector<int> score_for_each_valid_choice(valid_choices.size(), 0);

    int loop_count = (AI_LEVEL / valid_choices.size()) / hands[whose_turn].size();  // how many times we go through the valid choices

    for (int i = loop_count; i > 0; --i)
    {
        for (size_t choice_index = 0; choice_index < valid_choices.size(); ++choice_index)  // index for both vectors (valid choices and scores for each)
        {
            Game_Hand simulation = *this;

            simulation.this_is_simulation = true;
            simulation.player_is_human[0] = false;

            // tests to make sure arrays are copied correctly
            // std::cout << "player_is_human[0] " << player_is_human[0] << std::endl;
            // std::cout << "simulation.player_is_human[0] " << simulation.player_is_human[0] << std::endl;

            simulation.speculate_hands(whose_turn, passing_direction);

            // play the card we're checking now
            simulation.play_card(valid_choices[choice_index]);  // changes simulation.whose_turn to someone else

            do
            {
                // play the rest of the current trick
                while (simulation.turns_left_in_trick())
                {
                    simulation.play_card(simulation.simulation_play_ai());
                }
                simulation.end_trick();

                if (simulation.get_hands()[0].size() > 0)  // more tricks to play
                {
                    simulation.reset_trick();
                }
            } while (simulation.hands[0].size() > 0);  // until there are no more tricks left to play
            simulation.end_hand();

            score_for_each_valid_choice[choice_index] += simulation.scores[whose_turn];
        }  // end valid choice loop
    }  // end loop through valid choices multiple times according to AI_LEVEL

    // simple min function
    size_t index_of_min = 0;

    // TODO: remove these cout only after much testing and fine tuning of the AI
    std::cout << valid_choices[0].str() << "  " << (float)score_for_each_valid_choice[0] / loop_count << std::endl;
    for (size_t index = 1; index < valid_choices.size(); ++index)
    {
        if (score_for_each_valid_choice[index] < score_for_each_valid_choice[index_of_min])
        {
            index_of_min = index;
        }
        std::cout << valid_choices[index].str() << "  " << (float)score_for_each_valid_choice[index] / loop_count << std::endl;
    }

    return valid_choices[index_of_min];
}

Card Game_Hand::simulation_play_ai() const
{
    int random_if_less_than;  // play choice will be random if a rand()%10 < this

    // TODO: I don't know a good way to tune these numbers
    if (possible_to_shoot_moon())
        random_if_less_than = 4;  // higher probability of playing a random card
    else  // not possible to shoot the moon
        random_if_less_than = 2;  // lower probability of playing a random card

    if ((rand() % 10) < random_if_less_than)
    {
        std::vector<Card> valid_choices;
        find_valid_choices(valid_choices);

        return valid_choices[rand() % valid_choices.size()];
    }
    else  // static ai
    {
        return static_play_ai();
    }
}

std::vector<Card> Game_Hand::pass_ai(const int& from_player, const int& passing_direction) const
{
    const size_t THIRTEEN_CHOOSE_THREE = 286;

    std::vector<Card> cards_to_pass;  // = hands[from_player].pick_random(3);

    std::vector< std::pair< std::vector<size_t>, int > > sim_scores(THIRTEEN_CHOOSE_THREE);
    // sim_scores[index 0-285].first[index 0-2]  = indices of choices
    //                        .second            = score

    // copy hand to vector of cards for faster random access
    // (Deck random access is slow)
    std::vector<Card> hand;
    for (auto itr = hands[from_player].begin(); itr != hands[from_player].end(); ++itr)
        hand.push_back(*itr);

    size_t sim_scores_index = 0;
    size_t y, z;

    for (size_t x = 0; x < 11; ++x)
    {
        for (y = x + 1; y < 12; ++y)
        {
            for (z = y + 1; z < 13; ++z)
            {
                std::vector<Card> cards_passed_for_these_simulations;
                cards_passed_for_these_simulations.push_back(hand[x]);
                cards_passed_for_these_simulations.push_back(hand[y]);
                cards_passed_for_these_simulations.push_back(hand[z]);

                std::vector<size_t> hand_indices = {x, y, z};
                sim_scores[sim_scores_index] = std::pair< std::vector<size_t>, int > (hand_indices, 0);

                for (int sim_count = AI_LEVEL / 1000; sim_count > 0; --sim_count)
                {
                    Game_Hand simulation = *this;

                    simulation.this_is_simulation = true;
                    simulation.player_is_human[0] = false;

                    // test
                    /*
                    std::cout << "from_player " << from_player << " before speculate\n";
                    std::cout << simulation.hands[0].size() << ' '
                              << simulation.hands[1].size() << ' '
                              << simulation.hands[2].size() << ' '
                              << simulation.hands[3].size() << std::endl; */

                    simulation.speculate_hands(from_player, passing_direction);

                    // test
                    /*
                    std::cout << "from_player " << from_player << " after speculate\n";
                    std::cout << simulation.hands[0].size() << ' '
                              << simulation.hands[1].size() << ' '
                              << simulation.hands[2].size() << ' '
                              << simulation.hands[3].size() << std::endl; */


                    // passing
                    for (int player_to_pass = 0; player_to_pass < PLAYER_COUNT; ++player_to_pass)
                    {
                        if (player_to_pass == from_player)
                        {
                            simulation.pass(from_player,
                                            (from_player + passing_direction) % PLAYER_COUNT,
                                            cards_passed_for_these_simulations);
                        }
                        else  // not the player who's simulating
                        {
                            std::vector<Card> this_player_pass = simulation.hands[player_to_pass].pick_random(3);
                            //std::cout << "random player " << player_to_pass << " vector size " << this_player_pass.size() << std::endl;
                            simulation.pass(player_to_pass,
                                            (player_to_pass + passing_direction) % PLAYER_COUNT,
                                            this_player_pass);
                        }
                    }

                    // test
                    /*
                    std::cout << "from_player " << from_player << " after passing\n";
                    std::cout << simulation.hands[0].size() << ' '
                              << simulation.hands[1].size() << ' '
                              << simulation.hands[2].size() << ' '
                              << simulation.hands[3].size() << std::endl; */


                    simulation.receive_passed_cards();

                    // test
                    /*
                    std::cout << "from_player " << from_player << " after receiving\n";
                    std::cout << simulation.hands[0].size() << ' '
                              << simulation.hands[1].size() << ' '
                              << simulation.hands[2].size() << ' '
                              << simulation.hands[3].size() << std::endl; */


                    // playing
                    for (int tricks = 13; tricks > 0; --tricks)
                    {
                        simulation.reset_trick();
                        while (simulation.turns_left_in_trick())
                        {
                            simulation.play_card(simulation.simulation_play_ai());
                        }
                        simulation.end_trick();
                    }
                    simulation.end_hand();
                    // done playing

                    sim_scores[sim_scores_index].second += simulation.scores[from_player];
                }

                // for testing AI
                /*
                std::cout << "score " << (float)sim_scores[sim_scores_index].second / (AI_LEVEL / 1000) << "  "
                          << hand[x].str()
                          << hand[y].str()
                          << hand[z].str() << std::endl;
                */

                ++sim_scores_index;
            }
        }
    }

    // std::cout << sim_scores_index << std::endl;

    // min of scores
    size_t best_pass_index = 0;

    for (sim_scores_index = 1; sim_scores_index < THIRTEEN_CHOOSE_THREE; ++sim_scores_index)
    {
        if (sim_scores[sim_scores_index].second < sim_scores[best_pass_index].second)
            best_pass_index = sim_scores_index;
    }

    // found best pass
    cards_to_pass.push_back(hand[sim_scores[best_pass_index].first[0]]);
    cards_to_pass.push_back(hand[sim_scores[best_pass_index].first[1]]);
    cards_to_pass.push_back(hand[sim_scores[best_pass_index].first[2]]);

    // for AI testing
    std::cout << "player " << from_player << " had:\n";
    for (auto hand_itr = hand.begin(); hand_itr != hand.end(); ++hand_itr)
    {
        std::cout << hand_itr->str() << std::endl;
    }
    std::cout << "and chose to pass:\n";
    std::cout << cards_to_pass[0].str() << ' ';
    std::cout << cards_to_pass[1].str() << ' ';
    std::cout << cards_to_pass[2].str() << std::endl << std::endl;

    return cards_to_pass;
}
