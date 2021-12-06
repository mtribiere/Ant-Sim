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
#include <pthread.h>
#include <signal.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define ANTS_COUNT 100

#include "Ant.hpp"
#include "Food.hpp"
#include "Wall.hpp"

void signal_callback_handler(int signum);
void sortAnts(Ant *ants);
void *AntComputeThread(void *args);


struct AntArgument{
    int ID;
    Ant *ants;
    Ant *bestAnts;
    Food *food;
    std::vector<Wall> *walls;

    int state;
};

int globalExitFlag = 0;


int main(int argc, char **argv)
{   
    //Parse arguments 
    if(argc > 1)
        srand (atoi(argv[1])); 
    else
        srand(time(NULL));


    signal(SIGINT, signal_callback_handler);
    
    //Init Food
    Food food;

    //Init wall
    std::vector<Wall> walls;
    walls.push_back(Wall(sf::Vector2f(200,300),sf::Vector2f(400,50)));
    walls.push_back(Wall(sf::Vector2f(0,500),sf::Vector2f(300,50)));
    walls.push_back(Wall(sf::Vector2f(800-300,500),sf::Vector2f(300,50)));

    //Init Ants
    Ant *ants = (Ant*) malloc(sizeof(Ant)*ANTS_COUNT);
    for(int i = 0;i<ANTS_COUNT;i++) ants[i] = Ant(); 

    //Init Ants threads and arguments
    int state = 0; //0: Simulation | 1: Reproduction
    pthread_t *antsThread = (pthread_t *) malloc(sizeof(pthread_t) * ANTS_COUNT);
    struct AntArgument *antsArgument = (struct AntArgument *) malloc(sizeof(struct AntArgument) * ANTS_COUNT);
    for(int i = 0;i<ANTS_COUNT;i++){

        antsArgument[i].ID = i;
        antsArgument[i].ants = ants;
        antsArgument[i].food = &food;
        antsArgument[i].walls = &walls;
        antsArgument[i].state = 0;

    }

    int generation = 0;

    while (!globalExitFlag)
    {

        std::cout << "Generation " << generation << " -> ";

        //Run Ants
        double tSimStart = omp_get_wtime();

        int rc;
        for(int i = 0;i<ANTS_COUNT;i++){
            antsArgument[i].state = 0;
            rc = pthread_create(&antsThread[i], NULL, AntComputeThread, (void *)&antsArgument[i]);
      
            if (rc) {
                std::cout << "Error: Unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        //Join threads
        for(int i = 0;i<ANTS_COUNT;i++)
            pthread_join(antsThread[i],NULL);
        
        double tSimStop = omp_get_wtime() - tSimStart;

        
        ////////////////////////////////////////// Genetic part 
        double tstart = omp_get_wtime();

        //Sort Ants
        sortAnts(ants);
        
        //How many found food
        int total = 0;
        for(int i = 0;i<ANTS_COUNT;i++){
            if(ants[i].getFitness() == 0)
                total++;
        }
        std::cout << total << " | Best: " << ants[0].getFitness();

        //Generate new generation DNA
        Ant *bestAnts = (Ant*) malloc(sizeof(Ant)*10);
        memcpy(bestAnts,ants,sizeof(Ant)*10);
        
        for(int i = 0;i<ANTS_COUNT;i++){

            antsArgument[i].bestAnts = bestAnts;
            antsArgument[i].state = 1;

            rc = pthread_create(&antsThread[i], NULL, AntComputeThread, (void *)&antsArgument[i]);
            
      
            if (rc) {
                std::cout << "Error: Unable to create thread," << rc << std::endl;
                exit(-1);
            }
        }

        //Join threads
        for(int i = 0;i<ANTS_COUNT;i++)
            pthread_join(antsThread[i],NULL);



        free(bestAnts);

        //Reset new generation
        for(int antID = 0;antID<ANTS_COUNT;antID++){
            ants[antID].reset();
        }

        generation++;

        std::cout << " | Simulation time: " << tSimStop << " | Genetic time : " << omp_get_wtime()-tstart << std::endl;

    }

    free(ants);
    free(antsThread);
    free(antsArgument);
    return 0;
}



void sortAnts(Ant *ants){

    int bestAnts[10] = {-1};
    bestAnts[0] = 0;
    
    //For 10 best Ants
    for(int i = 0;i<10;i++){

        //Find the next lowest
        if(i == 0){

            //Simple lowest finding
            for(int antID = 0;antID < ANTS_COUNT;antID++){
            
                if(ants[antID].getFitness() <= ants[bestAnts[i]].getFitness()){
                    bestAnts[i] = antID;
                }
           
            }

        }else{ //Please help

            //Find init value (id not in the array)
            for(int j = 0;j<ANTS_COUNT;j++){
                int inArray = 0;
                for(int k = 0;k<10;k++){
                    if(bestAnts[k] == j)
                        inArray = 1;
                }

                if(!inArray){
                    bestAnts[i] = j;
                    break;
                }
            }

            //Look for every ant
            for(int antID = 0;antID < ANTS_COUNT;antID++){
                
                //If this ant not in the array
                int inArray = 0;
                for(int j = 0;j<10;j++)
                    if(bestAnts[j] == antID)
                        inArray = 1;

                if(!inArray){

                    //If the ant as a lower fitness    
                    if((ants[antID].getFitness() <= ants[bestAnts[i]].getFitness()) && (antID != bestAnts[i-1])){
                        bestAnts[i] = antID;
                    }

                }

            }

        }

    }

 

    Ant tmpAnts[10];
    for(int i = 0;i<10;i++){
        tmpAnts[i] = ants[bestAnts[i]];
    }

    for(int i = 0;i<10;i++){
        ants[i] = tmpAnts[i];
    }
}

void *AntComputeThread(void *args){    
    //Get all arguments out
    struct AntArgument *antArg = (struct AntArgument *) args; 
    int antID = antArg->ID;
    

    //Simulation
    if((antArg->state) == 0){

        //Compute the simulation
        for(int i = 0;i<DNA_SIZE;i++){
            (antArg->ants)[antID].moveAnt(i);
            (antArg->ants)[antID].checkCollision(*(antArg->food),*(antArg->walls));
        }

        //Compute fitness
        (antArg->ants)[antID].computeFitness(*(antArg->food));
    
    }else{ //Reproduction

            //Choose 2 parents
            int p1[DNA_SIZE];
            int p2[DNA_SIZE];
            
            (antArg->bestAnts)[rand()%10].getDNA(p1);
            (antArg->bestAnts)[rand()%10].getDNA(p2);

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

            (antArg->ants)[antID].setDNA(newDNA);

    }
    return NULL;
}

// Define the function to be called when ctrl-c (SIGINT) is sent to process
void signal_callback_handler(int signum) {
    std::cout << "[I] Received CTRL-C signal, exiting next generation" << std::endl;
    globalExitFlag = 1;
}