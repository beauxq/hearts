#include "Gui.h"

#include <iostream>  // TODO: get rid of this later
#include <string>
#include <vector>
#include <cstdlib>  // rand, size_t, strtol (stoi isn't working on my compiler, nor exception handling)
#include <algorithm>  // find
#include <thread>
#include <unordered_set>

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
    // load_images(&cards_finished);  // to load without threading, comment out thread lines
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
            // std::cout << "am i getting here?" << std::endl;  // test line
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

    const int arrow_size = 40;

    arrow_texture.create(arrow_size, arrow_size);

    sf::RectangleShape rectangle(sf::Vector2f(arrow_size / 2, arrow_size / 2));
    rectangle.setFillColor(sf::Color(255, 127, 0));
    rectangle.setPosition(arrow_size / 2, arrow_size / 4);

    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, sf::Vector2f(arrow_size / 2, 0));
    triangle.setPoint(1, sf::Vector2f(0, arrow_size / 2));
    triangle.setPoint(2, sf::Vector2f(arrow_size / 2, arrow_size));
    triangle.setFillColor(sf::Color(255, 127, 0));

    arrow_texture.clear(bg_color);

    arrow_texture.draw(rectangle);
    arrow_texture.draw(triangle);

    arrow_texture.display();

    arrow_sprite.setTexture(arrow_texture.getTexture());
    arrow_sprite.setOrigin(arrow_size / 2, arrow_size / 2);
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

void Gui::play_ai_wrapper(Card* to_play)
{
    // AI here
    *to_play = game.hand.static_play_ai();
}

void Gui::pause_wait_for_click(const float& seconds)
{
    // show screen for 1 second or until user clicks
    sf::Clock clock;
    bool user_clicked = false;

    while (window.isOpen() && (! user_clicked) && (clock.getElapsedTime().asSeconds() < seconds))
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonReleased)
                user_clicked = true;
            // else if other events (not mouse button release or close)
        }

        window.clear();
        screen_sprite.setTexture(screen_texture.getTexture());
        window.draw(screen_sprite);
        window.display();
    }
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

void Gui::show_played_cards()
{
    std::vector<Card> cards = game.hand.get_played_cards();

    sf::Vector2f base_location;  // bottom middle card
    base_location.x = window.getSize().x / 2 - card_sprites[0][2].getGlobalBounds().width / 2;
    base_location.y = window.getSize().y / 2 - card_sprites[0][2].getGlobalBounds().height / 2;

    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        if (cards[i].get_value() > 0)
        {
            switch (i)
            {
            case 0:
                draw_card(cards[i], base_location.x, base_location.y);
                break;
            case 1:
                draw_card(cards[i],
                          base_location.x - card_sprites[0][2].getGlobalBounds().width * 21 / 20,
                          base_location.y - card_sprites[0][2].getGlobalBounds().height *21 / 40);
                break;
            case 2:
                draw_card(cards[i],
                          base_location.x,
                          base_location.y - card_sprites[0][2].getGlobalBounds().height * 21 / 20);
                break;
            case 3:
                draw_card(cards[i],
                          base_location.x + card_sprites[0][2].getGlobalBounds().width * 21 / 20,
                          base_location.y - card_sprites[0][2].getGlobalBounds().height *21 / 40);
            }
        }
    }
}

void Gui::show_hand(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards)
{
    /** second parameter is which cards to display higher than others (for passing) */

    hand_y_position = window.getSize().y - card_sprites[0][2].getGlobalBounds().height * 3 / 2;
    width_of_card_space = card_sprites[0][2].getGlobalBounds().width * 21 / 20;
    hand_x_position = window.getSize().x / 2 - (width_of_card_space * hand.size()) / 2;

    int x_position = hand_x_position;  // each card

    int index = 0;
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
    {
        if (indices_of_higher_cards.find(index) == indices_of_higher_cards.end())
            draw_card(*itr, x_position, hand_y_position);
        else
            draw_card(*itr, x_position, hand_y_position - card_sprites[0][2].getGlobalBounds().height / 5);
        x_position += width_of_card_space;

        // std::cout << "drawing a card at " << x_position << ' ' << hand_y_position << std::endl;

        ++index;
    }
}

void Gui::pass_screen_draw(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards)
{
    screen_texture.clear(bg_color);
    show_game_scores();
    show_hand_scores();
    show_hand(hand, indices_of_higher_cards);
    draw_direction();
    screen_texture.display();

    std::cout << indices_of_higher_cards.size() << std::endl;
}

void Gui::turn_screen_draw()
{
    screen_texture.clear(bg_color);
    show_game_scores();
    show_hand_scores();
    show_hand(game.hand.get_hands()[0]);  // show player 0 human cards
    show_played_cards();
    screen_texture.display();
}

void Gui::draw_card(const Card& card, float x, float y)
{
    card_sprites[card.get_suit()][card.get_value()].setPosition(x, y);
    screen_texture.draw(card_sprites[card.get_suit()][card.get_value()]);
}

void Gui::draw_direction()
{
    arrow_sprite.setRotation((game.get_passing_direction() - 1) * 90);
    arrow_sprite.setPosition(window.getSize().x / 2, window.getSize().y / 2);
    screen_texture.draw(arrow_sprite);
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

    std::vector<std::thread> threads;  // so AI can work while human is choosing
    for (int player_passing = PLAYER_COUNT - 1; player_passing >= 0; --player_passing)  // start from end to start AI threads first, before starting human input
    {
        if (! game.hand.is_human(player_passing))  // not human
        {
            threads.push_back(std::thread(&Gui::ai_pass, this, player_passing, &(cards_to_pass[player_passing])));
        }
        else  // human
        {
            std::unordered_set<int> indices_to_pass;

            pass_screen_draw(game.hand.get_hands()[player_passing]);

            while (window.isOpen() && (cards_to_pass[player_passing].size() < 3))
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        window.close();
                    else if (event.type == sf::Event::MouseButtonReleased)
                    {
                        // I don't care which button it is
                        // check vertical position
                        // card click
                        if (event.mouseButton.y >= (hand_y_position - card_sprites[0][2].getGlobalBounds().height / 5) &&
                            event.mouseButton.y < (hand_y_position + card_sprites[0][2].getGlobalBounds().height))
                        {
                            int which_card_index = -1;
                            int x_position_in_hand = event.mouseButton.x - hand_x_position;
                            if (x_position_in_hand > 0 &&
                                x_position_in_hand % width_of_card_space < card_sprites[0][2].getGlobalBounds().width)
                            {
                                which_card_index = x_position_in_hand / width_of_card_space;
                                if (which_card_index >= game.hand.get_hands()[player_passing].size())
                                    which_card_index = -1;
                            }
                            if (which_card_index >= 0)
                            {
                                // std::cout << "clicked on card " << which_card_index << std::endl;
                                // toggle card in/out of passed cards
                                if (indices_to_pass.size() > 2)
                                {
                                    // only remove
                                    indices_to_pass.erase(which_card_index);
                                }
                                else  // less than 3 items
                                {
                                    if (! indices_to_pass.erase(which_card_index))  // if didn't erase something
                                    {
                                        indices_to_pass.insert(which_card_index);
                                    }
                                }

                                /* testing
                                for (auto itr = indices_to_pass.begin(); itr != indices_to_pass.end(); ++itr)
                                    std::cout << *itr << ' ';
                                std::cout << std::endl;
                                */

                                pass_screen_draw(game.hand.get_hands()[player_passing], indices_to_pass);
                            }
                        }
                        // else click not in card y
                        else if (arrow_sprite.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y) &&
                                 indices_to_pass.size() == 3)
                        {
                            std::cout << "clicked on arrow\n";
                            // pass these cards
                            for (auto itr = indices_to_pass.begin(); itr != indices_to_pass.end(); ++itr)
                            {
                                cards_to_pass[player_passing].push_back(game.hand.get_hands()[player_passing].at(*itr));
                            }
                        }
                        // else other mouse clicks (not card or arrow)
                    }
                    // else if other events (not mouse button release or close)
                }

                window.clear();
                screen_sprite.setTexture(screen_texture.getTexture());
                window.draw(screen_sprite);
                window.display();

            }
            // continue window processes until all players have chosen cards to pass
            bool all_players_passed = false;

            while (! all_players_passed)
            {
                if (! window_processes())
                    window.close();

                // check 3 AI players
                all_players_passed = true;
                for (int i = 3; i > 0; --i)
                    if (cards_to_pass[i].size() < 3)
                        all_players_passed = false;

            }
        }  // end human
    }  // end all players

    // join threads
    for (auto itr = threads.begin(); itr != threads.end(); ++itr)
        itr->join();

    if (window.isOpen())  // skip this if user closed the window
    {
        // pass the chosen cards
        for (int player_passing = 0; player_passing < PLAYER_COUNT; ++player_passing)
        {
            game.hand.pass(player_passing,
                           (player_passing + game.get_passing_direction()) % PLAYER_COUNT,
                           cards_to_pass[player_passing]);
        }

        // pick up passed cards
        game.hand.receive_passed_cards();

        // TODO: show the passed cards for 2? seconds?
    }
}

void Gui::computer_turn()
{
    std::vector<Card> valid_choices;
    Card to_play;

    game.hand.find_valid_choices(valid_choices);

    if (! window.isOpen())  // if window is closed, don't show plays or do any extra waiting
    {
        // AI here
        to_play = game.hand.static_play_ai();
        // to_play = valid_choices[rand() % valid_choices.size()];
    }
    else  // window is open
    {
        // start ai thread
        std::thread ai_thread(&Gui::play_ai_wrapper, this, &to_play);

        turn_screen_draw();
        pause_wait_for_click(1);

        ai_thread.join();
    }
    std::cout << "player " << game.hand.get_whose_turn() + 1 << " plays " << to_play.get_value() << ' ' << to_play.get_suit() << std::endl;
    game.hand.play_card(to_play);
}

void Gui::human_turn()
{

    //while (window_processes());


    // copy paste from input play choice text ui
    Card to_play;

    std::vector<Card> valid_choices;
    game.hand.find_valid_choices(valid_choices);
    // this might seem inefficient because find_valid_choices iterates through the hand,
    // then we iterate through the hand again to match indices with the vector
    // but find_valid_choices is called a lot more than this UI function
    // so we don't want find_valid_choices to do any more than it has to

    size_t current_valid_choice = 0;
    std::vector<int> indices_of_valid_choices;

    // put indices of valid choices in the right spot
    for (auto itr = game.hand.get_hands()[game.hand.get_whose_turn()].begin();
         itr != game.hand.get_hands()[game.hand.get_whose_turn()].end();
         ++itr)
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

    turn_screen_draw();

    while (window.isOpen() && (to_play.get_value() == 0))  // loop until valid input
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                // I don't care which button it is
                // check vertical position
                // card click
                if (event.mouseButton.y >= hand_y_position &&
                    event.mouseButton.y < (hand_y_position + card_sprites[0][2].getGlobalBounds().height))
                {
                    int which_card_index = -1;
                    int x_position_in_hand = event.mouseButton.x - hand_x_position;
                    if (x_position_in_hand > 0 &&
                        x_position_in_hand % width_of_card_space < card_sprites[0][2].getGlobalBounds().width)
                    {
                        which_card_index = x_position_in_hand / width_of_card_space;
                        if (which_card_index >= game.hand.get_hands()[game.hand.get_whose_turn()].size())
                            which_card_index = -1;
                    }
                    if (which_card_index >= 0)
                    {
                        if (indices_of_valid_choices[which_card_index] == -1)  // illegal play
                        {
                            to_play = Card();  // 0 value
                            std::cout << "That play is not allowed.\n";  // TODO: message on screen
                        }
                        else  // legal play
                            to_play = valid_choices[indices_of_valid_choices[which_card_index]];
                    }
                }
                // else click not in card y
            }
            // else if other events (not mouse button release or close)
        }

        window.clear();
        screen_sprite.setTexture(screen_texture.getTexture());
        window.draw(screen_sprite);
        window.display();
    }
    // window closed or valid card to play
    // TODO: window close save game

    game.hand.play_card(to_play);
}

void Gui::play()
{
    load();  // load images into memory

    // TODO: load saved game
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

            // passing is done, now play until a human's turn
            // (still part of the passing section until a human needs to play)
            if (window.isOpen())
            {
                game.hand.reset_trick();

                while (! game.hand.is_human(game.hand.get_whose_turn()))
                {
                    computer_turn();
                }
                // now is human turn
            }
        }
        else  // passing is done, human's turn to play
        {
            // assert game.hand.is_human(game.hand.get_whose_turn())
            // playing
            std::cout << "entered human playing section\n";
            human_turn();

            // if the window is open, play until the next human's turn
            if (window.isOpen())
            {
                // play the rest of the trick (computers' turns)
                while (game.hand.turns_left_in_trick())
                {
                    computer_turn();
                }
                // end trick
                turn_screen_draw();
                pause_wait_for_click(1);

                game.hand.end_trick();
                show_hand_scores();

                if (game.hand.get_hands()[0].size() > 0)  // more tricks to play
                {
                    game.hand.reset_trick();
                    while (! game.hand.is_human(game.hand.get_whose_turn()))
                    {
                        computer_turn();
                    }
                    // now is human turn
                }
                else  // no more tricks to play, end of hand
                {
                    game.hand.end_hand();
                    game.end_hand();
                    game.change_passing();

                    // is game over?
                    if (game.get_winners().empty())  // not game over
                    {
                        game.hand.reset_hand();
                        game.hand.deal_hands();
                    }
                    else  // there's a winner, game over
                    {
                        // show winner (don't wait for click before resetting game, so that if window closes, it is reset)
                        std::cout << "winner: " << game.get_winners()[0] << std::endl;  // TODO: show winning screen
                        game.game_reset();
                        game.hand.reset_hand();
                        game.hand.deal_hands();
                        // TODO: wait for click after resetting game
                    }
                }
            }  // section that is only done if window is still open
        }  // tricks section starting with human playing
    }
    // window closed
    std::cout << "out of window open while loop\n";
    // at this point, either:
    // human needs to play a card: save game
    // there are points in the game score and passing is not done: save game
    // there are no points in the game score and passing is not done: don't save game
}
