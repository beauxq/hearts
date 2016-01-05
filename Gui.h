#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

#include <SFML/Graphics.hpp>

#include <vector>
#include <unordered_set>

#include "Card.h"
#include "Deck.h"
#include "Game.h"

const int X_DEFAULT_RESOLUTION = 800;
const int Y_DEFAULT_RESOLUTION = 600;
const std::string WINDOW_NAME("Hearts");
const sf::Color DEFAULT_BG_COLOR(80, 0, 160);

class Gui
{
private:
    sf::RenderWindow window;
    sf::RenderTexture screen_texture;  // drawing is "off-screen"
    sf::Sprite screen_sprite;
    sf::Font font;
    sf::Color bg_color;

    int hand_y_position;
    int width_of_card_space;
    int hand_x_position;

    bool window_processes();

    std::vector<std::vector<sf::Texture> > card_textures;  // first index is suit
    std::vector<std::vector<sf::Sprite> > card_sprites;  // second index is value

    sf::RenderTexture arrow_texture;
    sf::Sprite arrow_sprite;

    void altload();
    void load();
    void load_images(int* cards_finished);
    void load_screen(int* cards_finished);
    void play_ai_wrapper(Card* to_play);
    void pause_wait_for_click(const float& seconds);

    Game game;

    void show_game_scores() const;
    void show_hand_scores() const;
    void show_played_cards();
    void show_hand(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards = std::unordered_set<int>());
    void pass_screen_draw(const Deck& hand, const std::unordered_set<int>& indices_of_higher_cards = std::unordered_set<int>());
    void turn_screen_draw();

    void draw_card(const Card& card, float x, float y);
    void draw_direction();

    /** this function called in other input functions
        @returns default constructed card for failure */
    Card choose_card(const std::vector<Card>& hand_vector) const;
    std::vector<Card> input_passing_choices(const Deck& hand) const;
    Card input_play_choice(const Deck& hand) const;

    void ai_pass(int player_passing, std::vector<Card>* cards_to_pass) const;

public:
    Gui(const int& x_resolution = X_DEFAULT_RESOLUTION, const int& y_resolution = Y_DEFAULT_RESOLUTION) :
        card_textures(SUIT_COUNT, std::vector<sf::Texture>(Deck::HIGH + 1)),
        card_sprites(SUIT_COUNT, std::vector<sf::Sprite>(Deck::HIGH + 1)),
        window(sf::VideoMode(x_resolution, y_resolution), WINDOW_NAME, sf::Style::Close),  // no resize allowed
        bg_color(DEFAULT_BG_COLOR)
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
