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
    const float X_OFFSET = 205.841;
    const float X_COL = 263.24;
    const int X_SIZE = 224;
    const int Y_SIZE = 312;

    const int Y_CLUB = 1093;
    const int Y_DIAMOND = 2149;
    const int Y_SPADE = 1797;
    const int Y_HEART = 1445;

    const std::string FILENAME = "resources/Color_52_Faces_v.2.0.png";
    sf::Image file;
    file.loadFromFile(FILENAME);

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

            // y coordinate
            switch (suit)
            {
            case CLUBS:
                y = Y_CLUB;
                break;
            case DIAMONDS:
                y = Y_DIAMOND;
                break;
            case SPADES:
                y = Y_SPADE;
                break;
            case HEARTS:
                y = Y_HEART;
                break;
            default:
                ; // wha...  !?
            }

            // x coordinate
            if (value == 14)  // ace
            {
                x = X_OFFSET + X_COL;
            }
            else  // 2-13
            {
                x = X_OFFSET + (X_COL * value);
            }

            // load
            if (! card_textures[suit][value].loadFromImage(file, sf::IntRect(x, y, X_SIZE, Y_SIZE)))
            {
                *cards_finished = -1;  // error
                return;
            }

            card_sprites[suit][value].setTexture(card_textures[suit][value]);
            card_sprites[suit][value].setScale(2 * float(window.getSize().x) / Deck::HIGH / X_SIZE,
                                               2 * float(window.getSize().x) / Deck::HIGH / X_SIZE);

            ++*cards_finished;
        }
    }

    make_arrow();
}

void Gui::load_images_alt(int* cards_finished)
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
                // wha...  !?
                x = 0;
                y = 0;
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

    make_arrow();
}

void Gui::make_arrow()
{
    const int arrow_size = 40;

    arrow_texture.create(arrow_size, arrow_size);

    sf::RectangleShape rectangle(sf::Vector2f(arrow_size / 2, arrow_size / 2));
    rectangle.setFillColor(button_color);
    rectangle.setPosition(arrow_size / 2, arrow_size / 4);

    sf::ConvexShape triangle;
    triangle.setPointCount(3);
    triangle.setPoint(0, sf::Vector2f(arrow_size / 2, 0));
    triangle.setPoint(1, sf::Vector2f(0, arrow_size / 2));
    triangle.setPoint(2, sf::Vector2f(arrow_size / 2, arrow_size));
    triangle.setFillColor(button_color);

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
    loading_message.setOrigin(loading_message.getLocalBounds().left, loading_message.getLocalBounds().top);  // needed only because the origin of text is weird
    loading_message.setColor(text_color);
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
    // TODO: remove time test after lots of testing
    sf::Clock clock;  // time test
    *to_play = game.hand.dynamic_play_ai(game.get_passing_direction());
    sf::Time time = clock.getElapsedTime();  // time test
    std::cout << "  time to decide: " << time.asSeconds() << std::endl;
}

void Gui::pause_wait_for_click(const float& seconds, const bool& allow_click_to_skip)
{
    // show screen for 1 second, or until user clicks if allow_click_to_skip
    sf::Clock clock;
    bool user_clicked = false;

    while (window.isOpen() && (! user_clicked) && (clock.getElapsedTime().asSeconds() < seconds))
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonReleased && allow_click_to_skip)
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

void Gui::show_hand_scores()
{
    float bottom_score_y;

    // text
    sf::Text score_of_each_player;
    score_of_each_player.setFont(font);
    score_of_each_player.setCharacterSize(window.getSize().y / 25);
    score_of_each_player.setOrigin(score_of_each_player.getLocalBounds().left, score_of_each_player.getLocalBounds().top);  // needed only because the origin of text is weird
    score_of_each_player.setColor(text_color);  // TODO: text color magic numbers

    for (int i = 0; i < PLAYER_COUNT; ++i)
    {
        score_of_each_player.setString("game: " + std::to_string(game.get_score(i)) + "\nhand: " + std::to_string(game.hand.get_score(i)));

        bottom_score_y = hand_y_position -
                         card_sprites[0][2].getGlobalBounds().height / 5 -
                         score_of_each_player.getGlobalBounds().height -
                         PADDING * 2;

        switch (i)
        {
        case 0:  // bottom
            score_of_each_player.setPosition((window.getSize().x - score_of_each_player.getGlobalBounds().width) / 2 -
                                             card_sprites[0][2].getGlobalBounds().width,
                                             bottom_score_y);
            break;
        case 1:  // left
            score_of_each_player.setPosition(PADDING, (bottom_score_y + PADDING) / 4);
            break;
        case 2:  // top
            score_of_each_player.setPosition((window.getSize().x - score_of_each_player.getGlobalBounds().width) / 2 +
                                             card_sprites[0][2].getGlobalBounds().width,
                                             PADDING);
            break;
        case 3:  // right
            score_of_each_player.setPosition(window.getSize().x - PADDING -
                                             score_of_each_player.getGlobalBounds().width,
                                             (bottom_score_y + PADDING) * 3 / 4);
        }

        screen_texture.draw(score_of_each_player);
    }
}

void Gui::show_passed_cards()
{
    screen_texture.clear(bg_color);

    // find indices of passed cards
    std::unordered_set<int> indices_of_higher_cards;
    size_t index = 0;
    for (auto itr = game.hand.get_hands()[0].begin(); itr != game.hand.get_hands()[0].end(); ++itr)
    {
        for (auto passed_itr = game.hand.get_passed_cards_to_player(0).begin();
             passed_itr != game.hand.get_passed_cards_to_player(0).end();
             ++passed_itr)
        {
            if (*itr == *passed_itr)
            {
                indices_of_higher_cards.insert(index);
                break;
            }
        }

        ++index;
    }
    // assert indices_of_higher_cards.size() == 3

    // show_hand has to be first in this function because other functions depend on variables set by show_hand
    show_hand(game.hand.get_hands()[0], indices_of_higher_cards);

    sf::Text tip;
    tip.setFont(font);
    tip.setString("These cards were passed to you.");
    tip.setCharacterSize(window.getSize().x / 53);
    tip.setOrigin(tip.getLocalBounds().left, tip.getLocalBounds().top);  // needed only because the origin of text is weird
    tip.setColor(text_color);
    tip.setPosition(window.getSize().x -
                        tip.getGlobalBounds().width -
                        PADDING,
                    hand_y_position -
                        card_sprites[0][2].getGlobalBounds().height / 5 -
                        tip.getGlobalBounds().height -
                        PADDING);

    show_hand_scores();
    screen_texture.draw(tip);

    screen_texture.display();

    pause_wait_for_click(4);
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

    hand_sprites.clear();

    hand_y_position = window.getSize().y - card_sprites[0][2].getGlobalBounds().height - PADDING;

    width_of_card_space = card_sprites[0][2].getGlobalBounds().width * 21 / 20;  // without overlapping

    hand_x_position = window.getSize().x / 2 - (width_of_card_space * hand.size()) / 2;
    if (hand_x_position < PADDING)
    {
        hand_x_position = PADDING;
        width_of_card_space = (window.getSize().x - (2 * PADDING) -
                               card_sprites[0][2].getGlobalBounds().width) / (hand.size() - 1);
    }

    int x_position = hand_x_position;  // each card

    int index = 0;
    for (auto itr = hand.begin(); itr != hand.end(); ++itr)
    {
        if (indices_of_higher_cards.find(index) == indices_of_higher_cards.end())
            draw_card(*itr, x_position, hand_y_position);
        else
            draw_card(*itr, x_position, hand_y_position - card_sprites[0][2].getGlobalBounds().height / 5);
        hand_sprites.push_back(&(card_sprites[itr->get_suit()][itr->get_value()]));
        x_position += width_of_card_space;

        // std::cout << "drawing a card at " << x_position << ' ' << hand_y_position << std::endl;

        ++index;
    }
}

void Gui::pass_screen_draw(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards)
{
    screen_texture.clear(bg_color);

    // show_hand has to be first in this function because other functions depend on variables set by show_hand
    show_hand(hand, indices_of_higher_cards);

    sf::Text tip;
    tip.setFont(font);
    tip.setString("Choose three cards to pass to another player.");
    tip.setCharacterSize(window.getSize().x / 53);
    tip.setOrigin(tip.getLocalBounds().left, tip.getLocalBounds().top);  // needed only because the origin of text is weird
    tip.setColor(text_color);
    tip.setPosition(window.getSize().x -
                        tip.getGlobalBounds().width -
                        PADDING,
                    hand_y_position -
                        card_sprites[0][2].getGlobalBounds().height / 5 -
                        tip.getGlobalBounds().height -
                        PADDING);

    show_hand_scores();
    screen_texture.draw(tip);
    draw_direction();

    screen_texture.display();

    std::cout << indices_of_higher_cards.size() << std::endl;
}

void Gui::turn_screen_draw(const std::string& rule)
{
    sf::Text rule_text;
    rule_text.setFont(font);
    rule_text.setString(rule);
    rule_text.setCharacterSize(window.getSize().x / 53);
    rule_text.setOrigin(rule_text.getLocalBounds().left, rule_text.getLocalBounds().top);  // needed only because the origin of text is weird
    rule_text.setColor(text_color);
    rule_text.setPosition(window.getSize().x -
                            rule_text.getGlobalBounds().width -
                            PADDING,
                          hand_y_position -
                            card_sprites[0][2].getGlobalBounds().height / 5 -
                            rule_text.getGlobalBounds().height -
                            PADDING);

    screen_texture.clear(bg_color);
    show_hand_scores();
    show_hand(game.hand.get_hands()[0]);  // show player 0 human cards
    screen_texture.draw(rule_text);
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

void Gui::ai_pass(int player_passing, std::vector<Card>* cards_to_pass) const
{
    std::cout << "thread has started for player " << player_passing << std::endl;
    *cards_to_pass = game.hand.pass_ai(player_passing, game.get_passing_direction());
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
            //ai_pass(player_passing, &(cards_to_pass[player_passing]));
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
                        // I don't care which button it is  // TODO: change to only left mouse button?
                        // check vertical position
                        // card click
                        if (event.mouseButton.y >= (hand_y_position - card_sprites[0][2].getGlobalBounds().height / 5) &&
                            event.mouseButton.y < (hand_y_position + card_sprites[0][2].getGlobalBounds().height))
                        {
                            int which_card_index = -1;

                            for (int index = hand_sprites.size() - 1; index >= 0; --index)  // backwards because of overlapping
                            {
                                if (hand_sprites[index]->getGlobalBounds().contains(event.mouseButton.x,
                                                                                    event.mouseButton.y))
                                {
                                    which_card_index = index;
                                    break;
                                }
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
                            std::cout << "clicked on arrow with 3 choices\n";
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

        show_passed_cards();
    }
}

void Gui::computer_turn()
{
    // std::vector<Card> valid_choices;
    Card to_play;

    // game.hand.find_valid_choices(valid_choices);

    if (window.isOpen())  // if window is closed, don't show plays or do any extra waiting
    {
        // start ai thread
        std::thread ai_thread(&Gui::play_ai_wrapper, this, &to_play);
        //play_ai_wrapper(&to_play);  // test without thread

        turn_screen_draw();
        pause_wait_for_click(1);

        ai_thread.join();
    }
    else  // window closed
    {
        play_ai_wrapper(&to_play);
    }
    std::cout << "           player " << game.hand.get_whose_turn() << " plays " << to_play.str() << std::endl;
    game.hand.play_card(to_play);
}

void Gui::human_turn()
{
    Card to_play;

    std::vector<Card> valid_choices;
    std::string rule;
    game.hand.find_valid_choices(valid_choices);
    game.hand.find_valid_choice_rule(rule);
    // this might seem inefficient because find_valid_choices iterates through the hand,
    // then we iterate through the hand again to match indices with the vector
    // but find_valid_choices is called a lot more than this UI function
    // so we don't want find_valid_choices to do any more than it has to

    size_t current_valid_choice = 0;
    std::vector<int> indices_of_valid_choices;

    // TODO: find_valid_choices needs to be changed to make sure the vector returned
    // is in the same order as iterating through the hand

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

                    for (int index = hand_sprites.size() - 1; index >= 0; --index)  // backwards because of overlapping
                    {
                        if (hand_sprites[index]->getGlobalBounds().contains(event.mouseButton.x,
                                                                            event.mouseButton.y))
                        {
                            which_card_index = index;
                            break;
                        }
                    }

                    if (which_card_index >= 0)
                    {
                        if (indices_of_valid_choices[which_card_index] == -1)  // illegal play
                        {
                            to_play = Card();  // 0 value
                            std::cout << rule << std::endl;
                            turn_screen_draw(rule);
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

    if (window.isOpen())
        game.hand.play_card(to_play);
}

void Gui::win_screen()
{
    screen_texture.clear(bg_color);

    // show_hand has to be first in this function because other functions depend on variables set by show_hand
    show_hand(game.hand.get_hands()[0]);  // just so we have the variables set that need to be set

    // win message
    std::string win_message;
    sf::Text win_text;
    win_text.setFont(font);
    if (game.get_winners().size() > 1)
        win_message = "winners:";
    else  // only 1 winner
        win_message = "winner:";
    for (auto itr = game.get_winners().begin(); itr != game.get_winners().end(); ++itr)
    {
        win_message += '\n' + player_names[*itr];
    }
    win_text.setString(win_message);
    win_text.setCharacterSize(window.getSize().y / 18);
    win_text.setOrigin(win_text.getLocalBounds().left, win_text.getLocalBounds().top);  // needed only because the origin of text is weird
    win_text.setColor(text_color);
    win_text.setPosition((window.getSize().x - win_text.getGlobalBounds().width) / 2,
                    window.getSize().y / 7);

    // buttons
    sf::Text quit_text;
    sf::Text again_text;
    quit_text.setFont(font);
    again_text.setFont(font);
    quit_text.setString("quit");
    again_text.setString("play again");
    quit_text.setCharacterSize(window.getSize().y / 25);
    again_text.setCharacterSize(window.getSize().y / 25);
    quit_text.setOrigin(quit_text.getLocalBounds().left, quit_text.getLocalBounds().top);  // needed only because the origin of text is weird
    again_text.setOrigin(again_text.getLocalBounds().left, again_text.getLocalBounds().top);  // needed only because the origin of text is weird
    quit_text.setColor(text_color);
    again_text.setColor(text_color);
    sf::RectangleShape quit_button;
    sf::RectangleShape again_button;
    quit_button.setSize(sf::Vector2f(again_text.getGlobalBounds().width + 2 * PADDING,
                                     again_text.getGlobalBounds().height + 2 * PADDING));  // same size button for both
    again_button.setSize(sf::Vector2f(again_text.getGlobalBounds().width + 2 * PADDING,
                                      again_text.getGlobalBounds().height + 2 * PADDING));
    quit_button.setFillColor(button_color);
    again_button.setFillColor(button_color);
    quit_button.setPosition(window.getSize().x / 2 - PADDING - quit_button.getGlobalBounds().width,
                            win_text.getGlobalBounds().top + win_text.getGlobalBounds().height + PADDING);
    again_button.setPosition(window.getSize().x / 2 + PADDING,
                             win_text.getGlobalBounds().top + win_text.getGlobalBounds().height + PADDING);
    quit_text.setPosition(quit_button.getGlobalBounds().left + (quit_button.getGlobalBounds().width - quit_text.getGlobalBounds().width) / 2 + PADDING,
                          win_text.getGlobalBounds().top + win_text.getGlobalBounds().height + 2 * PADDING);
    again_text.setPosition(window.getSize().x / 2 + 2 * PADDING,
                           win_text.getGlobalBounds().top + win_text.getGlobalBounds().height + 2 * PADDING);

    screen_texture.draw(quit_button);
    screen_texture.draw(again_button);
    screen_texture.draw(quit_text);
    screen_texture.draw(again_text);

    show_hand_scores();
    screen_texture.draw(win_text);

    screen_texture.display();

    bool user_clicked_again = false;

    while (window.isOpen() && (! user_clicked_again))
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (quit_button.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    window.close();
                else if (again_button.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y))
                    user_clicked_again = true;
            }
            // else if other events (not mouse button release or close)
        }

        window.clear();
        screen_sprite.setTexture(screen_texture.getTexture());
        window.draw(screen_sprite);
        window.display();
    }
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
                pause_wait_for_click(1, false);

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
                        // show winner
                        std::cout << "winner: " << game.get_winners()[0] << std::endl;  // TODO: remove cout test
                        win_screen();

                        game.game_reset();
                        game.hand.reset_hand();
                        game.hand.deal_hands();
                    }
                }
            }  // section that is only done if window is still open
        }  // tricks section starting with human playing
    }
    // window closed
    std::cout << "out of window open while loop\n";
    // TODO: Save game
    // at this point, either:
    // human needs to play a card: save game
    // there are points in the game score and passing is not done: save game
    // there are no points in the game score and passing is not done: don't save game
    // nah, save it no matter what
}
