#include "Wall.hpp"

#include <SFML/Graphics.hpp>

Wall::Wall(sf::Vector2f position, sf::Vector2f size){

    this->setSize(size);
    this->setFillColor(sf::Color::Red);

    this->setPosition(position);
}