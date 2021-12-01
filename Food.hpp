#ifndef FOOD_HPP
#define FOOD_HPP

#include <SFML/Graphics.hpp>

#define FOOD_SIZE 20

class Food: public sf::CircleShape{

    public:
        Food();
        sf::Vector2f getPoS(){return this->getPosition();};
};

#endif