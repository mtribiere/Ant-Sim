#ifndef WALL_HPP
#define WALL_HPP

#include <SFML/Graphics.hpp>

class Wall: public sf::RectangleShape{

    public:         
        Wall(sf::Vector2f position, sf::Vector2f size);
        sf::Vector2f _getPosition(){return this->getPosition();};
        sf::Vector2f _getSize(){return this->getSize();};

};

#endif