#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_set>
#include <string>

#include "Card.h"
#include "Deck.h"
#include "Game.h"

const int X_DEFAULT_RESOLUTION = 800;
const int Y_DEFAULT_RESOLUTION = 600;
const std::string WINDOW_NAME("Hearts");
const sf::Color DEFAULT_BG_COLOR(80, 0, 160);
const sf::Color DEFAULT_TEXT_COLOR(90,180,0);
const sf::Color DEFAULT_BUTTON_COLOR(255, 127, 0);
const std::vector<std::string> DEFAULT_PLAYER_NAMES = {"You", "left", "North", "3 o'clock"};
const int PADDING = 5;  // TODO: find all the places where this needs to replace magic numbers

class Gui
{
private:
    sf::RenderWindow window;
    sf::RenderTexture screen_texture;  // drawing is "off-screen"
    sf::Sprite screen_sprite;
    sf::Font font;
    sf::Color bg_color;
    sf::Color text_color;
    sf::Color button_color;

    std::vector<std::string> player_names;

    int hand_y_position;
    int width_of_card_space;
    int hand_x_position;

    bool window_processes();

    std::vector<std::vector<sf::Texture> > card_textures;  // first index is suit
    std::vector<std::vector<sf::Sprite> > card_sprites;  // second index is value
    std::vector<sf::Sprite *> hand_sprites;  // pointers to card_sprites

    sf::RenderTexture arrow_texture;
    sf::Sprite arrow_sprite;

    void altload();
    void load();
    void load_images(int* cards_finished);
    void load_images_alt(int* cards_finished);  // old images, not used currently
    void make_arrow();
    void load_screen(int* cards_finished);
    void play_ai_wrapper(Card* to_play);
    void pause_wait_for_click(const float& seconds, const bool& allow_click_to_skip = true);

    Game game;

    void show_game_scores() const;
    void show_hand_scores();
    void show_passed_cards();
    void show_played_cards();
    void show_hand(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards = std::unordered_set<int>());
    void pass_screen_draw(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards = std::unordered_set<int>());
    void turn_screen_draw(const std::string& rule = std::string());

    void draw_card(const Card& card, float x, float y);
    void draw_direction();

    void ai_pass(int player_passing, std::vector<Card>* cards_to_pass) const;

public:
    Gui(const int& x_resolution = X_DEFAULT_RESOLUTION, const int& y_resolution = Y_DEFAULT_RESOLUTION) :
        window(sf::VideoMode(x_resolution, y_resolution), WINDOW_NAME, sf::Style::Close),  // no resize allowed // TODO: figure out resizing
        bg_color(DEFAULT_BG_COLOR),
        text_color(DEFAULT_TEXT_COLOR),
        button_color(DEFAULT_BUTTON_COLOR),
        player_names(DEFAULT_PLAYER_NAMES),
        card_textures(SUIT_COUNT, std::vector<sf::Texture>(Deck::HIGH + 1)),
        card_sprites(SUIT_COUNT, std::vector<sf::Sprite>(Deck::HIGH + 1))
    {
        window.setVerticalSyncEnabled(true);
        screen_texture.create(x_resolution, y_resolution);
        screen_sprite.setTexture(screen_texture.getTexture());
        font.loadFromFile("resources/LiberationSans-Regular.ttf");
    }

    // TODO: move to private what doesn't need to be public
    void pass();

    void computer_turn();

    void human_turn();

    void win_screen();

    void play();

    void test()
    {
        load();

        while (window.isOpen())
        {
            if (! window_processes())
                window.close();
        }
    }
};

#endif // GUI_H_INCLUDED
