#include "Game_Hand.h"

#include <set>
#include <vector>

#include "Deck.h"

int Game_Hand::points_for(const Card& card) const
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

void Game_Hand::find_valid_choices(std::vector<Card>& valid_choices) const
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

Card Game_Hand::static_play_ai()
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
                        return valid_choices[0];  // highest card
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

                for (int i = 1; i < valid_choices.size(); ++i)
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
        for (int i = 0; i < valid_choices.size(); ++i)
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
