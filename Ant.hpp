#ifndef ANT_HPP
#define ANT_HPP

#include <SFML/Graphics.hpp>
#include <cstring>
#include "Food.hpp"
#include "Wall.hpp"

#define DNA_SIZE 700
#define ANT_WIDTH 5
#define ANT_HEIGHT 40
#define MAX_FITNESS 400


class Ant: public sf::RectangleShape{

    public:
       Ant();
       void AntInit();
       void moveAnt(int i);
       void getDNA(int *p){memcpy(p,this->DNA,DNA_SIZE*sizeof(int));};
       void setDNA(int *_DNA);
       void computeFitness(Food food);
       void checkCollision(Food food, std::vector<Wall> walls);
       float getFitness(){return this->fitness;};
       void reset();

    private:
        int hitWall;
        float fitness;
        
        float endPosX;
        float endPosY;
        int DNA[DNA_SIZE]; 

};

#endif