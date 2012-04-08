//Daniel Malmer
//5/1/09
//Set of moves for the pacman

#include <vector>
#include <queue>
#include <stdlib.h>

using namespace std;

//block struct which holds a block of directions
struct Block {
    string route;
    Block* next;
    int id;
};

class Moves {
public:
    //moves are a linked list of blocks
    Block* head;
    
    Moves();
    Moves(const Moves& m);

    void generateRandomMoves();
    void createNewRoute(Block* block);

    string expand(bool debug);
    string expandDNA();
    string expandIDs();

    void crossover(Moves* otherMoves);
    void mutate(float rate);
};
 
int MAX_MOVES = 1500;

Moves::Moves() {
}

//copy constructer
Moves::Moves(const Moves& m) {
    int size = 0;
    if (m.head != NULL) {
	Block* block = new Block;	
	block->route = m.head->route;
	block->next = NULL;
	block->id = m.head->id;

	head = block;

	size += m.head->route.size();

	Block* mIter = m.head->next;
	Block* cIter = block;

	while (mIter != NULL && size < MAX_MOVES) {
	    block = new Block;
	    block->route = mIter->route;
	    block->next = NULL;
	    block->id = mIter->id;
	    cIter->next = block;

	    size += mIter->route.size();
	    
	    cIter = block;
	    mIter = mIter->next;
	}
    }
}

void Moves::generateRandomMoves() {
    //need at least one block
    Block* block = new Block;
    createNewRoute(block);    
    block->next = NULL;
        
    head = block;
    Block* iter = head;
        
    int numMoves = (rand() % 64);
    
    //create 0-63 more blocks
    for (int i = 0; i < numMoves; i++) {
	block = new Block;
	createNewRoute(block);
	block->next = NULL;
		
	iter->next = block;
	iter = iter->next;
    }
}

void Moves::createNewRoute(Block* block) {
    //resets the route and creates a new set of directions for the block
    //used in initial creation and mutation
    int i = 0;
    block->route = "";
    block->id = blockCount++;
    do {
	int randDirection = rand() % 4;
	switch (randDirection) {
	case 0:
	    block->route.append("l");
	    break;
	case 1:
	    block->route.append("r");
	    break;
	case 2:
	    block->route.append("u");
	    break;
	case 3:
	    block->route.append("d");
	    break;
	}
	i++;
    } while(rand() % 12 > 0 && i < MAX_MOVES);
}

string Moves::expand(bool debug = false) {
    //returns the string of directions
    string expandedMoves = "";
    Block* iter = head;
    while (iter != NULL) {	
	expandedMoves.append(iter->route);
	if (debug) {
	    expandedMoves.append(" ");
	}
	iter = iter->next;
    }
    return expandedMoves;
}

string Moves::expandDNA() {
    //returns lrud converted to atgc (no point, just for fun)
    string expandedDNA = "";
    int i = 0;
    Block* iter = head;
    while (iter != NULL) {
	string currRoute = iter->route;
	for (int j = 0; j < currRoute.size(); j++) {
	    i++;
	    switch ((char) currRoute.at(j)) {
	    case 'l':
		expandedDNA.append("a");
		break;
	    case 'r':
		expandedDNA.append("t");
		break;
	    case 'u':
		expandedDNA.append("g");
		break;
	    case 'd':
		expandedDNA.append("c");
		break;
	    }
	    i++;
	}
	expandedDNA.append(" ");
	iter = iter->next;
    }
    return expandedDNA;
}

string Moves::expandIDs() {
    //returns a string of the block ids
    string expandedIDs = "";
    int intID;
    char strID[10];
    Block* iter = head;
    while (iter != NULL) {
	intID = iter->id;
	expandedIDs.append(strID);
	iter = iter->next;
    }
    return expandedIDs;
}

void Moves::crossover(Moves* otherMoves) {    
    //randomly choose cut and splice or uniform cut and splice
    int randChoice = rand() % 2; 

    //CUT AND SPLICE CROSSOVER
    if (randChoice == 0) {
	int numBlocks1 = 0;
	Block* iter1 = head;
	while (iter1 != NULL) {
	    numBlocks1++;
	    iter1 = iter1->next;
	}
	int numBlocks2 = 0;
	Block* iter2 = otherMoves->head;
	while (iter2 != NULL) {
	    numBlocks2++;
	    iter2 = iter2->next;
	}
	int randNum1 = rand() % numBlocks1;
	int randNum2 = rand() % numBlocks2;
	iter1 = head;
	for (int i = 0; i < randNum1; i++) {
	    iter1 = iter1->next;
	}
	iter2 = otherMoves->head;
	for (int i = 0; i < randNum2; i++) {
	    iter2 = iter2->next;
	}
	Block* temp = iter1->next;
	iter1->next = iter2->next;
	iter2->next = temp;
    } 

    //"UNIFORM" CUT AND SPLICE CROSSOVER
    //walk through current blocks
    //.5 chance that it will swap with a random block from parent2
    //(means blocks can get swapped to one, then the other)
    else {
		
	//step 1: walk through both parents, setting T or F at each node
	//T indicates where a cut is made
	queue<bool> p1cuts;
	queue<bool> p2cuts;
	
	Block* p1iter;
	Block* p2iter;
	
	Block* p1shadow = NULL;
	Block* p2shadow = NULL;
	
	//need at least one cut on each parent
	bool validCuts = false;
	while (!validCuts) {
	    p1iter = head;
	    while (!p1cuts.empty()) {
		p1cuts.pop();
	    }
	    while (p1iter != NULL) {
		if (rand() % 2 == 0) {
		    p1cuts.push(true);
		    validCuts = true;
		} else {
		    p1cuts.push(false);
		}
		p1iter = p1iter->next;
	    }
	}
	validCuts = false;
	while (!validCuts) {
	    p2iter = otherMoves->head;
	    while (!p2cuts.empty()) {
		p2cuts.pop();
	    }
	    while (p2iter != NULL) {
		if (rand() % 2 == 0) {
		    p2cuts.push(true);
		    validCuts = true;
		} else {
		    p2cuts.push(false);
		}
		p2iter = p2iter->next;
	    }
	}
	
	//step 2: for every F before a T on either parent, leave that section where it is in the child
	//need 'shadows' to keep track of where we are
	p1iter = head;
	p2iter = otherMoves->head;
	while (!p1cuts.empty() && !p1cuts.front()) {
	    p1cuts.pop();
	    p1shadow = p1iter;
	    p1iter = p1iter->next;
	}
	while (!p2cuts.empty() && !p2cuts.front()) {
	    p2cuts.pop();
	    p2shadow = p2iter;
	    p2iter = p2iter->next;
	}
	
	//step 3: while we haven't reached the end of either parent...
	while (!p1cuts.empty() && !p2cuts.empty()) {
	    //we know front of p1cuts and p2cuts are both true
	    //step 3a: move next section to opposite child
	    p1cuts.pop();
	    if (p2shadow == NULL) {
		otherMoves->head = p1iter;
	    } else {
		p2shadow->next = p1iter;
	    }
	    while (!p1cuts.empty() && !p1cuts.front()) {
		p1cuts.pop();
		p2shadow = p1iter;
		p1iter = p1iter->next;
	    }	    
	    p2shadow = p1iter;
	    //if p1cuts is empty, p1iter will be NULL
	    p1iter = p1iter->next;
	    
	    p2cuts.pop();
	    if (p1shadow == NULL) {
		head = p2iter;
	    } else {
		p1shadow->next = p2iter;
	    }
	    while (!p2cuts.empty() && !p2cuts.front()) {
		p2cuts.pop();
		p1shadow = p2iter;
		p2iter = p2iter->next;
	    }
	    p1shadow = p2iter;
	    //if p2cuts is empty, p2iter will be NULL
	    p2iter = p2iter->next;
	    
	    //step 3b: move next section to same child
	    if (!p1cuts.empty()) {
		p1cuts.pop();
		//check for null like above?
		p1shadow->next = p1iter;
		while (!p1cuts.empty() && !p1cuts.front()) {
		    p1cuts.pop();
		    p1shadow = p1iter;
		    p1iter = p1iter->next;
		}
		p1shadow = p1iter;
		//if p1cuts is empty, p1iter will be NULL
		p1iter = p1iter->next;
	    }
	    
	    if (!p2cuts.empty()) {
		p2cuts.pop();
		p2shadow->next = p2iter;
		while (!p2cuts.empty() && !p2cuts.front()) {
		    p2cuts.pop();
		    p2shadow = p2iter;
		    p2iter = p2iter->next;
		}
		p2shadow = p2iter;
		//if p2cuts is empty, p2iter will be NULL
		p2iter = p2iter->next;
	    }
	}
	
	//step 4: we've reached the end of at least one parent. now alternate the remaining cuts between the two children.
	if (p2iter == NULL && p1iter != NULL) {
	    while (!p1cuts.empty()) {
		//parent 1 still has cuts left
		//step 4a: move next section to opposite child
		p1cuts.pop();
		p2shadow->next = p1iter;
		while (!p1cuts.empty() && !p1cuts.front()) {
		    p1cuts.pop();
		    p2shadow = p1iter;
		    p1iter = p1iter->next;
		}	    
		p2shadow = p1iter;
		p1iter = p1iter->next;
		
		//step 4b: move next section to same child
		//might have finished above
		if (!p1cuts.empty()) {
		    p1cuts.pop();
		    p1shadow->next = p1iter;
		    while (!p1cuts.empty() && !p1cuts.front()) {
			p1cuts.pop();
			p1shadow = p1iter;
			p1iter = p1iter->next;
		    }
		    p1shadow = p1iter;
		    p1iter = p1iter->next;
		}
	    }
	} else if (p1iter == NULL && p2iter != NULL) {
	    while (!p2cuts.empty()) {
		//parent 2 still has cuts left
		//step 4a: move next section to opposite child
		p2cuts.pop();
		p1shadow->next = p2iter;
		while (!p2cuts.empty() && !p2cuts.front()) {
		    p2cuts.pop();
		    p1shadow = p2iter;
		    p2iter = p2iter->next;
		}	    
		p1shadow = p2iter;
		p2iter = p2iter->next;
		
		//step 4b: move next section to same child
		//might have finished above
		if (!p2cuts.empty()) {
		    p2cuts.pop();
		    p2shadow->next = p2iter;
		    while (!p2cuts.empty() && !p2cuts.front()) {
			p2cuts.pop();
			p2shadow = p2iter;
			p2iter = p2iter->next;
		    }
		    p2shadow = p2iter;
		    p2iter = p2iter->next;
		}
	    }
	}
    }
}

void Moves::mutate(float rate) {
/*  
    //PICK ONE BLOCK AND MUTATE
    int numBlocks = 0;
    Block* iter = head;
    while (iter != NULL) {
	numBlocks += 1;
	iter = iter->next;
    }
    int randNum = rand() % numBlocks;
    iter = head;
    for (int i = 0; i < randNum; i++) {
	iter = iter->next;
    }
    createNewRoute(iter);
*/
    //PROBABILITY TO MUTATE EVERY BLOCK    
    Block* iter = head;
    double randX;
    while (iter != NULL) {
	randX = ((double) rand() + 1.0) / ((double) RAND_MAX - 1.0);
	if (randX < rate) {
	    createNewRoute(iter);
	}
	iter = iter->next;
    }

}
