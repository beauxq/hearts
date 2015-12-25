#include "Gui.h"

#include <iostream>  // TODO: get rid of this later
#include <string>
#include <vector>
#include <cstdlib>  // rand, size_t, strtol (stoi isn't working on my compiler, nor exception handling)
#include <algorithm>  // find
#include <thread>

#include "Card.h"

bool Gui::window_processes()
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            return false;
    }

    window.clear();
    screen_sprite.setTexture(screen_texture.getTexture());
    window.draw(screen_sprite);
    window.display();

    return true;
}

void Gui::load()
{
    int cards_finished = 0;
    //load_images(&cards_finished);
    std::thread load_thread(&Gui::load_images, this, &cards_finished);
    load_screen(&cards_finished);

    load_thread.join();
}

void Gui::load_images(int* cards_finished)
{
    // these numbers from data files (positions and sizes of cards)
    const int X_OFFSET = 8;
    const int Y_OFFSET = 6;
    const int X_COL = 328;
    const int Y_ROW = 492;
    const int X_SIZE = 272;
    const int Y_SIZE = 422;
    const int X_JACK = 330;
    const int X_QUEEN = 665;
    const int X_KING = 996;
    const int Y_FACE = 996;

    const std::string FILENAMES[SUIT_COUNT] = {"resources/Club.jpg",
                                               "resources/Diamond.jpg",
                                               "resources/Spade.jpg",
                                               "resources/Heart.jpg"};

    // rect parameters
    int x, y;

    for (int suit = 0; suit < SUIT_COUNT; ++suit)
    {
        for (int value = Deck::LOW; value <= Deck::HIGH; ++value)
        {
            // don't bother to finish loading if the user closes the window
            if (! window.isOpen())
            {
                return;
            }

            switch (value)
            {
            case 14:
                x = X_OFFSET;
                y = Y_OFFSET;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
                x = (value - 1) % 5 * X_COL + X_OFFSET;
                y = (value - 1) / 5 * Y_ROW + Y_OFFSET;
                break;
            case 11:
                x = X_JACK;
                y = Y_FACE;
                break;
            case 12:
                x = X_QUEEN;
                y = Y_FACE;
                break;
            case 13:
                x = X_KING;
                y = Y_FACE;
                break;
            default:
                ; // wha...  !?
            }
            std::cout << "am i getting here?" << std::endl;
            if (! card_textures[suit][value].loadFromFile(FILENAMES[suit], sf::IntRect(x, y, X_SIZE, Y_SIZE)))
            {
                *cards_finished = -1;  // error
                return;
            }

            card_sprites[suit][value].setTexture(card_textures[suit][value]);
            card_sprites[suit][value].setScale(float(window.getSize().x) / Deck::HIGH / X_SIZE,
                                               float(window.getSize().x) / Deck::HIGH / X_SIZE);

            ++*cards_finished;
        }
    }
}

void Gui::load_screen(int* cards_finished)
{
    // text
    sf::Text loading_message;
    loading_message.setFont(font);
    loading_message.setString("loading images...");
    loading_message.setCharacterSize(48);
    loading_message.setColor(sf::Color(80,160,0));
    loading_message.setPosition((window.getSize().x - loading_message.getLocalBounds().width) / 4,
                                window.getSize().y / 4);

    // progress bar background
    sf::RectangleShape progress_bar_bg(sf::Vector2f(loading_message.getLocalBounds().width,
                                                    loading_message.getLocalBounds().height / 2));
    progress_bar_bg.setPosition(loading_message.getGlobalBounds().left,
                                loading_message.getGlobalBounds().top + loading_message.getGlobalBounds().height * 1.1);
    progress_bar_bg.setFillColor(sf::Color::Black);

    // progress bar
    sf::RectangleShape progress_bar(sf::Vector2f(loading_message.getLocalBounds().width,
                                                 loading_message.getLocalBounds().height / 2));
    progress_bar.setPosition(loading_message.getGlobalBounds().left,
                             loading_message.getGlobalBounds().top + loading_message.getGlobalBounds().height * 1.1);
    progress_bar.setFillColor(sf::Color(80,160,0));

    while (window.isOpen())
    {
        if (! window_processes())
            window.close();

        progress_bar.setScale(float(*cards_finished) / 52, 1);
        screen_texture.clear(bg_color);  // TODO: background color here
        screen_texture.draw(loading_message);
        screen_texture.draw(progress_bar_bg);
        screen_texture.draw(progress_bar);
        screen_texture.display();

        if (*cards_finished == 52 || *cards_finished == -1)
            break;
    }

    // TODO: file open error (*cards_finished == -1)
}

void Gui::show_game_scores() const
{
    std::cout << "  Game Scores:  " << game.get_score(0) << "  "
                                    << game.get_score(1) << "  "
                                    << game.get_score(2) << "  "
                                    << game.get_score(3) << std::endl;
}

void Gui::show_hand_scores() const
{
    std::cout << "  Hand Scores:  " << game.hand.get_score(0) << "  "
                                    << game.hand.get_score(1) << "  "
                                    << game.hand.get_score(2) << "  "
                                    << game.hand.get_score(3) << std::endl;
}

void Gui::show_hand(const Deck& hand)
{
    int y_position = window.getSize().y - card_sprites[0][2].getGlobalBounds().height * 3 / 2;
    int width_of_card_space = card_sprites[0][2].getGlobalBounds().width * 21 / 20;
    int x_position = window.getSize().x / 2 - (width_of_card_space * hand.size()) / 2;

    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
    {
        draw_card(*itr, x_position, y_position);
        x_position += width_of_card_space;

        std::cout << "drawing a card at " << x_position << ' ' << y_position << std::endl;
    }
}

void Gui::draw_card(const Card& card, float x, float y)
{
    card_sprites[card.get_suit()][card.get_value()].setPosition(x, y);
    screen_texture.draw(card_sprites[card.get_suit()][card.get_value()]);
}

void Gui::draw_direction(const int& how_many_players_to_the_left) const
{
    switch (how_many_players_to_the_left)
    {
    case 1:
        return;  // "left";
    case 2:
        return;  // "across";
    case 3:
        return;  // "right";
    default:
        return;  // "ERROR: BAD DIRECTION SENT TO direction_str!";
    }
}

Card Gui::choose_card(const std::vector<Card>& hand_vector) const
{
    std::string choice;

    std::getline(std::cin, choice);
    int choice_index = strtol(choice.c_str(), nullptr, 10) - 1;
    if (size_t(choice_index) >= hand_vector.size() || choice_index < 0)  // invalid choice
        return Card();  // 0 value
    else  // valid choice
        return hand_vector.at(choice_index);
}

std::vector<Card> Gui::input_passing_choices(const Deck& hand) const
{
    std::vector<Card> to_return;
    Card card_choice;

    std::vector<Card> hand_vector;
    // copy hand to vector
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
        hand_vector.push_back(*itr);

    std::cout << "pick 3 to pass " << game.get_passing_direction() << " to the left\n";
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

Card Gui::input_play_choice(const Deck& hand) const
{
    Card to_return;

    std::vector<Card> valid_choices;
    game.hand.find_valid_choices(valid_choices);
    // this might seem inefficient because find_valid_choices iterates through the hand,
    // then we iterate through the hand again to match indices with the vector
    // but find_valid_choices is called a lot more than this UI function
    // so we don't want find_valid_choices to do any more than it has to

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
    while (! to_return.get_value())  // loop until valid input
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

void Gui::ai_pass(int player_passing, std::vector<Card>* cards_to_pass) const
{
    *cards_to_pass = game.hand.get_hands()[player_passing].pick_random(3);
}

void Gui::pass()
{
    std::vector<std::vector<Card> > cards_to_pass(PLAYER_COUNT);  // first index is from player

    // start AI threads
    std::vector<std::thread> threads;
    for (int player_passing = PLAYER_COUNT - 1; player_passing >= 0; --player_passing)
    {
        if (! game.hand.is_human(player_passing))  // not human
        {
            threads.push_back(std::thread(&Gui::ai_pass, this, player_passing, &(cards_to_pass[player_passing])));
        }
        else  // human
        {
            screen_texture.clear(bg_color);
            show_game_scores();
            show_hand_scores();
            show_hand(game.hand.get_hands()[player_passing]);
            screen_texture.display();

            while (window.isOpen())
            {
                if (! window_processes())
                    window.close();

            }
        }
    }

    // join threads
    for (auto itr = threads.begin(); itr != threads.end(); ++itr)
        itr->join();
}

void Gui::play()
{
    load();  // load images into memory
    game.game_reset();
    game.hand.reset_hand();
    game.hand.deal_hands();

    while (window.isOpen())
    {
        // find out what point in the game we're at
        if (game.hand.get_pass_count() < PLAYER_COUNT)  // passing needs to be done
        {
            // passing
            if (! game.get_passing_direction())  // if keeper
            {
                game.hand.set_pass_count();  // tell it we've already passed
            }
            else  // not keeper, need to pass
            {
                pass();
            }
        }
        else  // passing is done
        {
            // playing
        }
    }


    /* Text_UI
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
                    to_play = game.hand.static_play_ai();
                    // to_play = valid_choices[rand() % valid_choices.size()];

                    std::cout << "player " << game.hand.get_whose_turn() + 1 << " plays " << to_play.get_value() << ' ' << to_play.get_suit() << std::endl;
                    game.hand.play_card(to_play);
                }
                else  // is human
                {
                    show_hand(game.hand.get_hands()[game.hand.get_whose_turn()]);
                    Card to_play = input_play_choice(game.hand.get_hands()[game.hand.get_whose_turn()]);
                    std::cout << "player " << game.hand.get_whose_turn() + 1 << " plays " << to_play.get_value() << ' ' << to_play.get_suit() << std::endl;
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
    */
}