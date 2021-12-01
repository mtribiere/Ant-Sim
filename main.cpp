/*    __   __ _  ____    ____  __  _  _  _  _  __     __  ____  __  ____ 
//   / _\ (  ( \(_  _)  / ___)(  )( \/ )/ )( \(  )   / _\(_  _)/  \(  _ \
//  /    \/    /  )(    \___ \ )( / \/ \) \/ (/ (_/\/    \ )( (  O ))   /
//  \_/\_/\_)__) (__)   (____/(__)\_)(_/\____/\____/\_/\_/(__) \__/(__\_)
*/

#include <chrono>
#include <thread>
#include <time.h>
#include <vector>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <cstring>
#include <omp.h> 

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define ANTS_COUNT 10000

#include "Ant.hpp"
#include "Food.hpp"
#include "Wall.hpp"

void sortAnts(Ant *ants);

// Good Seeds : 
// 50: Very demonstrative
// 175: Very quick convergance
// 124: Blocking
// 

int main(int argc, char **argv)
{   
    //Parse arguments 
    if(argc > 1)
        srand (atoi(argv[1])); 
    else
        srand(time(NULL));

    //Init UI
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Ant simulator");
    window.setPosition(sf::Vector2i(200,200));

    //Init Ants
    Ant *ants = (Ant*) malloc(sizeof(Ant)*ANTS_COUNT);
    for(int i = 0;i<ANTS_COUNT;i++) ants[i] = Ant(); 
    
    //Init Food
    Food food;

    //Init wall
    std::vector<Wall> walls;
    walls.push_back(Wall(sf::Vector2f(200,300),sf::Vector2f(400,50)));
    walls.push_back(Wall(sf::Vector2f(0,500),sf::Vector2f(300,50)));
    walls.push_back(Wall(sf::Vector2f(800-300,500),sf::Vector2f(300,50)));

    int step = 0;
    int generation = 0;

    while (window.isOpen())
    {

        std::cout << "Generation " << generation << " -> ";
        
        //Compute an iteration
        while(step<DNA_SIZE && window.isOpen()){
            
            ////////////////////////////////////////// Simulation part         
            for(int antID=0;antID<ANTS_COUNT;antID++){
                ants[antID].moveAnt(step);
                ants[antID].checkCollision(food,walls);
            }

            ////////////////////////////////////////// Drawing part 
            //Draw the ants
            
            window.clear();

            for(int antID=0;antID<ANTS_COUNT;antID++){
                window.draw((Ant)ants[antID]);
            }

            //Draw food
            window.draw(food);
            
            //Draw walls
            for(int i = 0;i< (int) walls.size();i++){
                window.draw(walls[i]);
            }

            //Display everything
            window.display();
            

            //Wait
            std::this_thread::sleep_for (std::chrono::milliseconds(1));

            step++;

            //Search for an exit event
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close(); 
            }
        }

        if(!window.isOpen())
            break;

        ////////////////////////////////////////// Genetic part 
        double tstart = omp_get_wtime();

        //Compute the fitness
        for(int antID = 0;antID<ANTS_COUNT;antID++){
            ants[antID].computeFitness(food);
        }

        //Sort Ants
        sortAnts(ants);

        //How many found food
        int total = 0;
        for(int i = 0;i<ANTS_COUNT;i++){
            if(ants[i].getFitness() == 0)
                total++;
        }
        std::cout << total;

        //Generate new generation DNA
        Ant *antsTmp = (Ant*) malloc(sizeof(Ant)*ANTS_COUNT);
        for(int i = 0;i<ANTS_COUNT;i++) antsTmp[i] = Ant(); 
    
        for(int i = 0;i<ANTS_COUNT;i++){
            
            //Choose 2 parents
            int p1[DNA_SIZE];
            int p2[DNA_SIZE];
            
            ants[rand()%10].getDNA(p1);
            ants[rand()%10].getDNA(p2);

            //Choose a DNA merge point
            int mergePoint = rand()%DNA_SIZE;

            //Mix DNA
            int newDNA[DNA_SIZE];
            
            for(int j = 0;j<mergePoint;j++)
                newDNA[j] = p1[j];
            
            for(int j = mergePoint;j<DNA_SIZE;j++)
                newDNA[j] = p2[j];
            
            //Mutation
            if((rand()%ANTS_COUNT/2) == 0){
                if(rand()%2)
                    for(int j = rand()%DNA_SIZE;j<DNA_SIZE;j++)
                            newDNA[j] = (rand()%21 - 10);
                else
                    for(int j = rand()%DNA_SIZE;j>=0;j--)
                            newDNA[j] = (rand()%21 - 10);
                    
            }

            //Set DNA
            antsTmp[i].setDNA(newDNA);

        }

        for(int i = 0;i<ANTS_COUNT;i++){
            ants[i] = antsTmp[i];
        }

        free(antsTmp);

        //Reset new generation
        step = 0;
        for(int antID = 0;antID<ANTS_COUNT;antID++){
            ants[antID].reset();
        }

        generation++;

        std::cout << " | Average compute time : " << omp_get_wtime()-tstart << std::endl;

    }

    free(ants);
    return 0;
}



void sortAnts(Ant *ants){
    int i,j;
    for(i = 0;i<ANTS_COUNT-1;i++){
        for(j = 0;j<ANTS_COUNT-1;j++){
            
            if(ants[j].getFitness() > ants[j+1].getFitness()){
                
                Ant tmp = ants[j];
                ants[j] = ants[j+1];
                ants[j+1] = tmp;

            }

        }
    }
}
