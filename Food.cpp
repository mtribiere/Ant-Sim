#include "Food.hpp"

Food::Food(){
    this->setRadius(FOOD_SIZE);
    this->setFillColor(sf::Color::Green);
    
    this->setPosition(40,7*800/8);
}