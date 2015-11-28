#include "Text_UI.h"

#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <cstdlib>  // rand, size_t, strtol (stoi isn't working on my compiler, nor exception handling)
#include <algorithm>  // find

void Text_UI::show_game_scores() const
{
    std::cout << "  Game Scores:  " << game.get_score(0) << "  "
                                    << game.get_score(1) << "  "
                                    << game.get_score(2) << "  "
                                    << game.get_score(3) << std::endl;
}

void Text_UI::show_hand_scores() const
{
    std::cout << "  Hand Scores:  " << game.hand.get_score(0) << "  "
                                    << game.hand.get_score(1) << "  "
                                    << game.hand.get_score(2) << "  "
                                    << game.hand.get_score(3) << std::endl;
}

void Text_UI::show_hand(const Deck& hand) const
{
    int number = 1;
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
    {
        if (number < 10)
            std::cout << ' ';
        std::cout << number << "  " << card_str(*itr) << std::endl;
        ++number;
    }
}

std::string Text_UI::card_str(const Card& card) const
{
    std::string to_return;
    switch (card.get_value())
    {
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        to_return += char(card.get_value() + '0');  // I don't know proper casting
        break;
    case 10:
        to_return = "10";
        break;
    case 11:
        to_return = "Jack";
        break;
    case 12:
        to_return = "Queen";
        break;
    case 13:
        to_return = "King";
        break;
    case 14:
        to_return = "Ace";
        break;
    default:
        to_return = "ERROR: CARD HAS BAD VALUE!";
    }
    to_return += " of ";
    switch (card.get_suit())
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
        to_return += "ERROR: CARD HAS BAD SUIT!";
    }

    return to_return;
}

std::string Text_UI::direction_str(const int& how_many_players_to_the_left) const
{
    switch (how_many_players_to_the_left)
    {
    case 1:
        return "left";
    case 2:
        return "across";
    case 3:
        return "right";
    default:
        return "ERROR: BAD DIRECTION SENT TO direction_str!";
    }
}

Card Text_UI::choose_card(const std::vector<Card>& hand_vector) const
{
    std::string choice;

    std::getline(std::cin, choice);
    int choice_index = strtol(choice.c_str(), nullptr, 10) - 1;
    if (size_t(choice_index) >= hand_vector.size() || choice_index < 0)  // invalid choice
        return Card();  // 0 value
    else  // valid choice
        return hand_vector.at(choice_index);
}

std::vector<Card> Text_UI::input_passing_choices(const Deck& hand) const
{
    std::vector<Card> to_return;
    Card card_choice;

    std::vector<Card> hand_vector;
    // copy hand to vector
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
        hand_vector.push_back(*itr);

    std::cout << "pick 3 to pass " << direction_str(game.get_passing_direction()) << std::endl;
    while (to_return.size() < 3)
    {
        std::cout << "choice " << to_return.size() + 1 << "? ";

        card_choice = choose_card(hand_vector);

        if (card_choice.get_value() &&  // not 0 value
            std::find(to_return.begin(), to_return.end(), card_choice) == to_return.end())
        {
            to_return.push_back(card_choice);
        }
    }

    return to_return;
}

Card Text_UI::input_play_choice(const Deck& hand) const
{
    Card to_return;

    std::vector<Card> valid_choices;
    game.hand.find_valid_choices(valid_choices);

    size_t current_valid_choice = 0;
    std::vector<int> indices_of_valid_choices;

    // put indices of valid choices in the right spot
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
    {
        if ((current_valid_choice < valid_choices.size()) &&  // valid choice left to find
            (*itr == valid_choices[current_valid_choice]))  // found here
        {
            indices_of_valid_choices.push_back(current_valid_choice);
            ++current_valid_choice;
        }
        else  // not here, or none left to find
            indices_of_valid_choices.push_back(-1);  // flag for not valid / illegal play
    }

    std::cout << "card to play? ";
    while (! to_return.get_value())
    {
        std::string choice;

        std::getline(std::cin, choice);
        int choice_index = strtol(choice.c_str(), nullptr, 10) - 1;
        if (size_t(choice_index) >= indices_of_valid_choices.size() || choice_index < 0)  // invalid menu option
            to_return = Card();  // 0 value
        else  // one of the menu options
        {
            if (indices_of_valid_choices[choice_index] == -1)  // illegal play
            {
                to_return = Card();  // 0 value
                std::cout << "That play is not allowed.\n";
            }
            else  // legal play
                to_return = valid_choices[indices_of_valid_choices[choice_index]];
        }
    }

    return to_return;
}

void Text_UI::play()
{
    game.game_reset();
    while (game.get_winners().empty())
    {
        game.hand.reset_hand();
        game.hand.deal_hands();

        show_game_scores();

        // passing
        if (game.get_passing_direction())
        {
            for (int player_passing = 0; player_passing < PLAYER_COUNT; ++player_passing)
            {
                if (! game.hand.is_human(player_passing))
                {
                    std::vector<Card> to_pass = game.hand.get_hands()[player_passing].pick_random(3);
                    game.hand.pass(player_passing, (player_passing+game.get_passing_direction())%PLAYER_COUNT, to_pass);
                }
                else  // is human
                {
                    show_hand(game.hand.get_hands()[player_passing]);
                    game.hand.pass(player_passing,
                                   (player_passing+game.get_passing_direction())%PLAYER_COUNT,
                                   input_passing_choices(game.hand.get_hands()[player_passing]));
                }
            }
            game.hand.receive_passed_cards();
        }
        // passing done

        // play
        for (int tricks = 13; tricks > 0; --tricks)
        {
            game.hand.reset_trick();
            while (game.hand.turns_left_in_trick())
            {
                if (! game.hand.is_human(game.hand.get_whose_turn()))
                {
                    std::vector<Card> valid_choices;
                    Card to_play;

                    game.hand.find_valid_choices(valid_choices);

                    // AI here
                    to_play = valid_choices[rand() % valid_choices.size()];

                    std::cout << "player " << game.hand.get_whose_turn() + 1 << " plays " << card_str(to_play) << std::endl;
                    game.hand.play_card(to_play);
                }
                else  // is human
                {
                    show_hand(game.hand.get_hands()[game.hand.get_whose_turn()]);
                    Card to_play = input_play_choice(game.hand.get_hands()[game.hand.get_whose_turn()]);
                    std::cout << "player " << game.hand.get_whose_turn() + 1 << " plays " << card_str(to_play) << std::endl;
                    game.hand.play_card(to_play);
                }
            }
            game.hand.end_trick();
            show_hand_scores();
            std::cout << std::endl;
            game.change_passing();
        }
        game.hand.end_hand();
        game.end_hand();
    }
    show_game_scores();
}
