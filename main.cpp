#include <SFML/Graphics.hpp>

#include <iostream>

#include "Deck.h"

void test()
{
    // test
    Deck deck(true);

    std::vector<Suit> new_sort = {HEARTS, DIAMONDS, SPADES, CLUBS};
    deck.change_sort(new_sort);

    // test iteration through deck
    for (auto itr = deck.begin(); itr != deck.end(); ++itr)
    {
        std::cout << itr->get_value() << ' ' << itr->get_suit() << std::endl;
    }

    Card pick = deck.deal_one();

    std::cout << "dealt card: " << pick.get_value() << ' ' << pick.get_suit() << std::endl;
    // rest of deck
    for (auto itr = deck.begin(); itr != deck.end(); ++itr)
    {
        std::cout << itr->get_value() << ' ' << itr->get_suit() << std::endl;
    }


    // test sfml
    sf::RenderWindow window(sf::VideoMode(200, 200), "SFML works!");
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        window.draw(shape);
        window.display();
    }
}

int main()
{
    test();

    return 0;
}
