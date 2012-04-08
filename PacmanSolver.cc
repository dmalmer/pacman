//Daniel Malmer
//5/1/09
//Pacman solved used genetic algorithms

int blockCount = 0;

#include <iostream>
#include "Moves.cc"
#include "PacmanGame.cc"

using namespace std;

PacmanGame* game;

int fitness(Moves* moves, bool display = false) {
    //play the game with the given moves 
    string expandedMoves = moves->expand();
    int fitnessNum = game->RunGame(expandedMoves, display);
    return fitnessNum;
}

int closeness(Moves* mv1, Moves* mv2) {
    //used in deterministic crowding to determine distance
    int s = 0;
    string m1 = mv1->expand();
    string m2 = mv2->expand();

    int smallLength = m1.length();
    //subtract the difference in the move sizes
    if (m2.length() < smallLength) {
	smallLength = m2.length();
	//s -= m1.length() - smallLength;
    } else {
	//s -= m2.length() - smallLength;
    }
  
    for (int i = 0; i < smallLength; i++) {
	//give more to sameness in the beginning
	if (m1.at(i) == m2.at(i)) {
	    //child will never be longer than MAX_MOVES
	    s += MAX_MOVES - i;
	}
    }

    return s;
}

vector<int> createFitnessVector(vector<Moves*> population, int size) {
    //vector of ints correlated with moves
    vector<int> fitnessVector;
    for (int i = 0; i < population.size(); i++) {
	fitnessVector.push_back(fitness(population[i]));
    }
    return fitnessVector;
}

void sort(vector<Moves*> &population, vector<int> &popFitness, int size) {
    //insertion sort
    //walk through array once
    for (int i = 1; i < size; i++) {
        Moves *tempMoveI = population[i];
	int tempFitnessI = popFitness[i];
        int j = i-1;
        //on each next item, place it where it goes in already sorted items
        while (j >= 0 && tempFitnessI < popFitness[j]) {
            //shift as you go
            Moves *tempMoveJ = population[j];
	    int tempFitnessJ = popFitness[j];
            population[j] = population[j+1];
	    popFitness[j] = popFitness[j+1];
            population[j+1] = tempMoveJ;
	    popFitness[j+1] = tempFitnessJ;
            j--;
        }
        population[j+1] = tempMoveI;
	popFitness[j+1] = tempFitnessI;
    }
}

Moves* chooseParent(vector<Moves*> population, vector<int> popFitness, int size) {
    //tournament
    //choose parent at random, only choosing out of top half
    int p1 = rand() % size/2;
    int p2 = p1;
    while (p2 == p1) {
        p2 = rand() % size/2;
    }

    //return the fitter of the two parents
    return (popFitness[(size/2)+p1] > popFitness[(size/2)+p2]) ? population[(size/2)+p1] : population[(size/2)+p2];
}

void mutate(Moves &parent, float rate) {
    //taken care of in moves
    parent.mutate(rate);
}

void crossover(Moves &parent1, Moves &parent2) {
    //taken care of in moves
    parent1.crossover(&parent2);
}

Moves* deterministicCrowding(int popSize, int maxIter, float mutateRate) {
    
    vector<Moves*> population;
    vector<int> popFitness;

    bool used[popSize];
    vector<int> pairs;

    //initial population
    for (int i = 0; i < popSize; i++) {
        Moves* moves = new Moves();
        moves->generateRandomMoves();
        population.push_back(moves);
    }

    //initial fitnesses
    popFitness = createFitnessVector(population, popSize);
   
    //sort the population by fitness
    sort(population, popFitness, popSize);

    int count = 0;

    while (count < maxIter) {
        count++;

	//output
	cout << endl << "GENERATION " << count << ":" << endl;
	for (int i = popSize-10; i < popSize; i++) {
	    string move = population[i]->expand(true);
	    cout << "Pacman " << i << ": Fitness = " << popFitness[i] << endl;//", Move = " << move << endl;
	}

	//-------------------------	
	// Deterministic Crowding:
	//-------------------------
	
	//create n/2 pairs
	for (int i = 0; i < popSize; i++) {
	    used[i] = false;
	}
	
	//set pairs
	for (int i = 0; i < popSize/2; i++) {
	    //p1 is first available element
	    int p1 = i;
	    while (used[p1]) {
		p1++;
	    }
	    used[p1] = true;
	    pairs.push_back(p1);

	    //p2 is a random element or closest to that random element 
	    int p2 = rand() % popSize;
	    while (used[p2]) {
		p2 = (p2 + 1) % popSize;
	    }
	    used[p2] = true;
	    pairs.push_back(p2);
	}

	while (!pairs.empty()) {
	    //get next pair
	    int p1 = pairs.back();
	    pairs.pop_back();
	    int p2 = pairs.back();
	    pairs.pop_back();

	    //create two children
	    Moves* parent1 = population[p1];
	    Moves* parent2 = population[p2];

	    Moves* child1 = new Moves(*parent1);
	    Moves* child2 = new Moves(*parent2);
	    
	    //cross the children
	    crossover(*child1, *child2);
	    
	    //mutate the children;
	    mutate(*child1, mutateRate);
	    mutate(*child2, mutateRate);
	    
	    //determine fitnesses of children
	    int c1Fit = fitness(child1);
	    int c2Fit = fitness(child2);
	    
	    //find closer parent-child pairs
	    if (closeness(parent1, child1) + closeness(parent2, child2) > closeness(parent1, child2) + closeness(parent2, child1)) {
		//hold tournament
		if (c1Fit > popFitness[p1]) {
		    delete population[p1];
		    population[p1] = child1;
		    popFitness[p1] = c1Fit;
		} else {
		    delete child1;
		}
		if (c2Fit > popFitness[p2]) {
		    delete population[p2];
		    population[p2] = child2;
		    popFitness[p2] = c2Fit;
		} else {
		    delete child2;
		}
	    } else {
		//hold tournament
		if (c2Fit > popFitness[p1]) {
		    delete population[p1];
		    population[p1] = child2;
		    popFitness[p1] = c2Fit;
		} else {
		    delete child2;
		}
		if (c1Fit > popFitness[p2]) {
		    delete population[p2];
		    population[p2] = child1;
		    popFitness[p2] = c1Fit;
		} else {
		    delete child1;
		}
	    }
	}
	//sort the population by fitness
	sort(population, popFitness, popSize);
    }

    //return most fit element
    return population[popSize-1];
 
}

Moves* geneticAlgorithm(int popSize, int maxIter, float crossoverRate, float mutateRate) {

    vector<Moves*> population;
    vector<int> popFitness;

    //initial population
    for (int i = 0; i < popSize; i++) {
        Moves* moves = new Moves();
        moves->generateRandomMoves();
        population.push_back(moves);
    }
    //initial fitnesses
    popFitness = createFitnessVector(population, popSize);

    int count = 0;

    while (count < maxIter) {
        count++;

        //update fitness vector
        for (int i = 0; i < popSize/2; i++) {
            popFitness[i] = fitness(population[i]);
        }

        //sort the population by fitness
        sort(population, popFitness, popSize);

	//output
	cout << endl << "GENERATION " << count << ":" << endl;
	for (int i = popSize-10; i < popSize; i++) {
	    string move = population[i]->expand(true);
	    cout << "Pacman " << i << ": Fitness = " << popFitness[i] << endl;//", Move = " << move << endl;
	}

	//crossover or mutate the bottom half (will just write over it)
        for (int i = 0; i < popSize/2; i++) {
            //choose to crossover or mutate the remaining population the parents
            //random double between 0 and 1
            double randX = ((double) rand() + 1.0) / ((double) RAND_MAX - 1.0);

            //if more than 1 left and random double is within probability, do crossover
            if ((i+1 < popSize/2) && randX < crossoverRate) {

                Moves* choice1 = chooseParent(population, popFitness, popSize);
                Moves* choice2 = choice1;
                while (choice2 == choice1) {
                    choice2 = chooseParent(population, popFitness, popSize);
                }

                Moves* parent1 = new Moves(*choice1);
                Moves* parent2 = new Moves(*choice2);

                //pass in by reference, so now parents are their children
                crossover(*parent1, *parent2);

                //overwrite current lower half of population with the new children
                delete population[i];
                delete population[i+1];
                population[i] = parent1;
                population[i+1] = parent2;

                //increment i again because 2 children were set
                i++;

            } else {

                Moves* choice = chooseParent(population, popFitness, popSize);
                Moves* parent = new Moves(*choice);

                //pass in by reference, so now parent is the child
                mutate(*parent, mutateRate);

                //overwrite current lower half of population with the new child
                delete population[i];
                population[i] = parent;
            }
        }
    }

    //return most fit element
    return population[popSize-1];
}


int main(int argc, char *argv[]) {

    srand(time(NULL));

    //create just one game
    game = new PacmanGame();
    //seed the ghost moves once
    game->SeedGhostMoves();

    //adjustable variables for genetic algorithm
    int popSize = 400;
    int maxIter = 1000;
    float crossoverRate = 0.60;
    float mutateRate = 0.1;

    Moves *finalMoves;

    //check if deterministic crowding or genetic algorithm
    if (argc > 1) {
	if (argv[1][0] == 'd') {
	    //adjustable variables for deterministic crowding
	    popSize = 200;
	    maxIter = 800;
	    mutateRate = 0.001;
	    //run deterministic crowding
	    finalMoves = deterministicCrowding(popSize, maxIter, mutateRate);	    
	} else {
	    //run normal genetic algorithm
	    finalMoves = geneticAlgorithm(popSize, maxIter, crossoverRate, mutateRate);
	}
    } else {
	//run normal genetic algorithm
	finalMoves = geneticAlgorithm(popSize, maxIter, crossoverRate, mutateRate);
    }

    string message = "Type \"repeat\", \"blocks\", or \"quit\".\n> ";

    //allow for commands after the program finishes running
    string again = "r";
    while (again != "q" && again != "quit") {
	if (again == "dna") {
	    //just for fun- convert to dna
	    string finalDNA = finalMoves->expandDNA();
	    cout << "Final Pacman DNA = " << finalDNA << endl;
	    cout << message;
	    cin >> again;
	} else if (again == "blocks" || again == "ids") {
	    //print out block ids numbers
	    string finalIDs = finalMoves->expandIDs();
	    cout << "Final Pacman Block IDs = " << finalIDs << endl;
	    cout << message;
	    cin >> again;
	} else if (again == "r" || again == "repeat") {
	    //run the final pacman game again
	    for (int i = 0; i < 50; i++) {
		cout << endl;
	    }
	    int finalFitness = fitness(finalMoves, true);
	    string finalRoute = finalMoves->expand(true);
	    cout << endl << "Final Pacman Fitness = " << finalFitness << ", Moves = " << finalRoute << endl;
	    cout << message;
	    cin >> again;
	} else {
	    //if typo, just prompt again
	    cout << "> ";
	    cin >> again;
	}
    }
}
