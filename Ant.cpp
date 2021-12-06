#include "Ant.hpp"
#include <math.h>       /* sin */
#include <stdio.h>
#include <iostream>
#include <cstring>

#define PI 3.14159265

Ant::Ant(){

    //Init DNA
    for(int i = 0;i<DNA_SIZE;i++)
        this->DNA[i] = rand()%21 - 10;
    
    this->hitWall = 0;
    this->fitness = -1;
    
    //Init Graphics
    this->setPosition(sf::Vector2f(800/2,800/8));
    this->setRotation(90);
    this->setSize(sf::Vector2f(ANT_HEIGHT,ANT_WIDTH));
    this->setFillColor(sf::Color::White);


}

void Ant::moveAnt(int i){

    //If in a wall
    if(this->hitWall)
        return;

    //Move the Ant
    this->rotate(DNA[i]);
    sf::Vector2f v = sf::Vector2f(2*cos(this->getRotation() * PI / 180.0), 2*sin(this->getRotation() * PI / 180.0));
    this->move(v);

    //Compute end position
    this->endPosX = this->getPosition().x + (ANT_HEIGHT * cos(this->getRotation() * PI / 180.0));
    this->endPosY = this->getPosition().y + (ANT_HEIGHT * sin(this->getRotation() * PI / 180.0));

}

void Ant::checkCollision(Food food, std::vector<Wall> walls){

    //Bondaries
    if(this->getPosition().x < 0 || this->getPosition().y < 0 ||
        this->getPosition().x >= 800 || this->getPosition().y >= 800){
            this->hitWall = 1;
        }
            

    if(this->endPosX < 0 || this->endPosY < 0 ||
        this->endPosX >= 800 || this->endPosY >= 800)
            this->hitWall = 1;

    //Food
    if((this->getPosition().x >= food.getPosition().x) && (this->getPosition().x <= food.getPosition().x + FOOD_SIZE)
        && (this->getPosition().y >= food.getPosition().y) && (this->getPosition().y <= food.getPosition().y + FOOD_SIZE)){
            this->hitWall = 1;
            this->fitness = 0;
        }

    if((this->endPosX >= food.getPosition().x) && (this->endPosX <= food.getPosition().x + FOOD_SIZE)
        && (this->endPosY >= food.getPosition().y) && (this->endPosY <= food.getPosition().y + FOOD_SIZE)){
            this->hitWall = 1;
            this->fitness = 0;
        }

    
    //Walls
    for(int i = 0;i< (int) walls.size();i++){

        
         if((this->getPosition().x >= walls[i]._getPosition().x) && (this->getPosition().x <= walls[i]._getPosition().x + walls[i]._getSize().x)
        && (this->getPosition().y >= walls[i]._getPosition().y) && (this->getPosition().y <= walls[i]._getPosition().y + walls[i]._getSize().y)){
            this->hitWall = 1;
        }

         if((this->endPosX >= walls[i]._getPosition().x) && (this->endPosX <= walls[i]._getPosition().x + walls[i]._getSize().x)
        && (this->endPosY >= walls[i]._getPosition().y) && (this->endPosY <= walls[i]._getPosition().y + walls[i]._getSize().y)){
            this->hitWall = 1;
        }
        
    }

}

void Ant::setDNA(int* _DNA){
    memcpy(this->DNA,_DNA,DNA_SIZE*sizeof(int));
}

void Ant::computeFitness(Food food){


    if(fitness == 0)
        return;
    this->fitness = (abs(this->getPosition().x - food.getPosition().x) + abs(this->getPosition().y - food.getPosition().y));
    
}

void Ant::reset(){
    this->setPosition(800/2,800/8);
    this->setRotation(90);
    this->hitWall = 0;
    this->fitness = -1;
}