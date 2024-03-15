#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define CARPARKSIZE 512

/*
typedef struct AdjListNode{
    int distanceNextStation;
    struct AdjListNode* leftChild;
    struct AdjListNode* rightChild;
    struct AdjListNode* parent;
    int inUse;
    char colour;
} AdjListNode;
 */

/*typedef struct NodesPool{
    AdjListNode* nodes;
    int nextAvailableIndex;
    int poolDim;
} NodesPool;
 */

typedef struct Station {
    int distance;
    int carPark[CARPARKSIZE];
    int maxAutonomy;
    int actualCars; //numero attuale di auto
    //int stationModified;
    //AdjListNode* adjListHeadForwards;
    //AdjListNode* adjListHeadBackwards;
    //int adjListForwardsDim;
    //int adjListBackwardsDim;
    //int adjListForwardsCounter;
    //int adjListBackwardsCounter;
} Station;

typedef struct Road {
    int roadModified;
    int isStationModified;
    Station* stationList; //dovrà essere array ordinato in senso crescente, in modo da implementare ricerca binaria, rimozione elemento implica gestione ottimale chiusura buchi
    int stationCounter;
    int roadDim;
} Road;

typedef struct MarkerArray{
    int distance;
    int steps;
    int predecessor;
} MarkerArray;

/*
AdjListNode* newNode(int distance, NodesPool pool){   // O(n) dipende però da tempo speso da eventuale realloc
    if(pool.nextAvailableIndex < pool.poolDim){
        AdjListNode* node = &(pool.nodes[pool.nextAvailableIndex]);
        int helper = pool.nextAvailableIndex++;
        pool.nodes[pool.nextAvailableIndex].inUse = 1;
        pool.nodes[pool.nextAvailableIndex].distanceNextStation = distance;
        pool.nextAvailableIndex = pool.poolDim;
        for(int i=helper;i<pool.poolDim;i++){
            if(pool.nodes[i].inUse == 0){
                pool.nextAvailableIndex = i;
                break;
            }
        }
        return node;
    }else{
        pool.poolDim = pool.poolDim * 2;
        pool.nodes = (AdjListNode*) realloc(pool.nodes, pool.poolDim * sizeof(AdjListNode));
        if(pool.nodes == NULL){
            printf("Errore allocazione memoria.\n");
            exit(EXIT_FAILURE);
        }
        for(int i=pool.nextAvailableIndex;i<pool.poolDim;i++){
            pool.nodes[i].inUse = 0;
        }
        AdjListNode* node = &(pool.nodes[pool.nextAvailableIndex]);
        pool.nodes[pool.nextAvailableIndex].inUse = 1;
        pool.nextAvailableIndex++;
        return node;
    }
}

void nodeRemoval(NodesPool pool, AdjListNode* node){     // O(n)
    node->distanceNextStation = 0;
    node->leftChild = NULL;
    node->rightChild = NULL;

    if(node >= &(pool.nodes[0]) && node <= &(pool.nodes[pool.poolDim-1])){
        int index = node - &(pool.nodes[0]);
        pool.nodes[index].inUse = 0;

        for(int i=0;i<pool.poolDim;i++){
            if(pool.nodes[i].inUse == 0){
                pool.nextAvailableIndex = i;
                break;
            }
        }
    }
}

AdjListNode* treeSearch(AdjListNode* tree, int distance){   // O[log(n)]
    if(tree == NULL || distance == tree->distanceNextStation){
        return tree;
    }else if(distance < tree->distanceNextStation){
        return treeSearch(tree->leftChild, distance);
    }else{
        return treeSearch(tree->rightChild, distance);
    }
}

int searchNode(AdjListNode* tree, int distance){  // O[log(n)]
    if(tree == NULL){
        return 0;
    }else if(tree->distanceNextStation == distance){
        return 1;
    }else if(distance < tree->distanceNextStation){
        return searchNode(tree->leftChild, distance);
    }else{
        return searchNode(tree->rightChild, distance);
    }
}

AdjListNode* treeMinimum(AdjListNode* node){   // O[log(n)]
    while(node->leftChild != NULL){
        node = node->leftChild;
    }
    return node;
}

AdjListNode* treeSuccessor(AdjListNode* node){    // O[log(n)]
    if(node->rightChild != NULL){
        return treeMinimum(node->rightChild);
    }
    AdjListNode* y = node->parent;
    while(y != NULL && node == y->rightChild){
        node = y;
        y = y->parent;
    }
    return y;
}

void leftRotate(AdjListNode* tree, AdjListNode* node){  // O[1]
    AdjListNode* y = node->rightChild;
    node->rightChild = y->leftChild;
    if(y->leftChild != NULL){
        y->leftChild->parent = node;
    }
    y->parent = node->parent;
    if(node->parent == NULL){
        tree = y;
    }else if(node == node->parent->leftChild){
        node->parent->leftChild = y;
    }else{
        node->parent->rightChild = y;
    }
    y->leftChild = node;
    node->parent = y;
}

void rightRotate(AdjListNode* tree, AdjListNode* node){ // O[1]
    AdjListNode* y = node->leftChild;
    node->leftChild = y->rightChild;
    if(y->rightChild != NULL){
        y->rightChild->parent = node;
    }
    y->parent = node->parent;
    if(node->parent == NULL){
        tree = y;
    }else if(node == node->parent->rightChild){
        node->parent->rightChild = y;
    }else{
        node->parent->leftChild = y;
    }
    y->rightChild = node;
    node->parent = y;
}

void RBDeleteFixup(AdjListNode* tree, AdjListNode* node) {
    if (node->colour == 'R' || node->parent == NULL) {
        node->colour = 'B';
    } else if (node == node->parent->leftChild) {
        AdjListNode *w = node->parent->rightChild;
        if (w->colour == 'R') {
            w->colour = 'B';
            node->parent->colour = 'R';
            leftRotate(tree, node->parent);
            w = node->parent->rightChild;
        }
        if (w->leftChild->colour == 'B' && w->rightChild->colour == 'B') {
            w->colour = 'R';
            RBDeleteFixup(tree, node->parent);
        } else if (w->rightChild->colour == 'B') {
            w->leftChild->colour = 'B';
            w->colour = 'R';
            rightRotate(tree, w);
            w = node->parent->rightChild;
        }
        w->colour = node->parent->colour;
        node->parent->colour = 'B';
        w->rightChild->colour = 'B';
        leftRotate(tree, node->parent);
    } else{
        AdjListNode *w = node->parent->leftChild;
        if (w->colour == 'R') {
            w->colour = 'B';
            node->parent->colour = 'R';
            rightRotate(tree, node->parent);
            w = node->parent->leftChild;
        }
        if (w->rightChild->colour == 'B' && w->leftChild->colour == 'B') {
            w->colour = 'R';
            RBDeleteFixup(tree, node->parent);
        } else if (w->leftChild->colour == 'B') {
            w->rightChild->colour = 'B';
            w->colour = 'R';
            leftRotate(tree, w);
            w = node->parent->leftChild;
        }
        w->colour = node->parent->colour;
        node->parent->colour = 'B';
        w->leftChild->colour = 'B';
        rightRotate(tree, node->parent);
    }
}

void RBInsertFixup(AdjListNode* tree, AdjListNode* node){   // O[log(n)]
    if(node == tree){
        tree->colour = 'B';
    }else {
        AdjListNode *x = node->parent;
        if (x->colour == 'R') {
            if (x == x->parent->leftChild) {
                AdjListNode *y = x->parent->rightChild;
                if (y->colour == 'R') {
                    x->colour = 'B';
                    y->colour = 'B';
                    x->parent->colour = 'R';
                    RBInsertFixup(tree, x->parent);
                } else {
                    if (node == x->rightChild) {
                        node = x;
                        leftRotate(tree, node);
                        x = node->parent;
                    }
                    x->colour = 'B';
                    x->parent->colour = 'R';
                    rightRotate(tree, x->parent);
                }
            } else {
                AdjListNode *y = x->parent->leftChild;
                if (y->colour == 'R') {
                    x->colour = 'B';
                    y->colour = 'B';
                    x->parent->colour = 'R';
                    RBInsertFixup(tree, x->parent);
                } else {
                    if (node == x->leftChild) {
                        node = x;
                        rightRotate(tree, node);
                        x = node->parent;
                    }
                    x->colour = 'B';
                    x->parent->colour = 'R';
                    leftRotate(tree, x->parent);
                }
            }
        }
    }
}

void insertNodeRedBlackTree(AdjListNode* tree, AdjListNode* node){ // O[log(n)]
    AdjListNode* y = NULL;
    AdjListNode* x = tree;
    while(x != NULL){
        y = x;
        if(node->distanceNextStation < x->distanceNextStation){
            x = x->leftChild;
        }else{
            x = x->rightChild;
        }
    }
    node->parent = y;
    if(y == NULL) {
        tree = node;
    }else if(node->distanceNextStation < y->distanceNextStation) {
        y->leftChild = node;
    }else{
        y->rightChild = node;
    }
    node->leftChild = NULL;
    node->rightChild = NULL;
    node->colour = 'R';
    RBInsertFixup(tree, node);
}

AdjListNode* removeNodeRedBlackTree(AdjListNode* tree, int distance) {  // O[log(n)]
    AdjListNode *z = treeSearch(tree, distance);
    AdjListNode *y;
    AdjListNode *x;
    if (z->leftChild == NULL || z->rightChild == NULL) {
        y = z;
    } else {
        y = treeSuccessor(z);
    }
    if (y->leftChild != NULL) {
        x = y->leftChild;
    } else {
        x = y->rightChild;
    }
    x->parent = y->parent;
    if (y->parent == NULL) {
        tree = x;
    } else if (y == y->parent->leftChild) {
        y->parent->leftChild = x;
    } else {
        y->parent->rightChild = x;
    }
    if (y != z) {
        z->distanceNextStation = y->distanceNextStation;
    }
    if (y->colour == 'B') {
        RBDeleteFixup(tree, x);
    }
    return y;
}
*/

void bubbleSort(Station array[], int dim){  // O(n)
    int swap = 1;
    Station tmp;

    for(int i = dim-1; i>0 && swap == 1;i--){
        if(array[i].distance < array[i-1].distance){
            tmp = array[i-1];
            array[i-1] = array[i];
            array[i] = tmp;
        }else{
            swap = 0;
        }
    }
}

// Funzione per leggere una parola (sequenza di caratteri non-spazio)
char* readWord(char* buffer) {
    char ch;
    int length = 0;

    // Leggi caratteri finché non incontri uno spazio o un carattere di fine riga
    while ((ch = getc_unlocked(stdin)) != EOF && ch != ' ' && ch != '\n'){
        buffer[length] = ch;
        length++;
    }
    if(ch == EOF){
        return NULL;
    }

    // Aggiungi il terminatore di stringa '\0'
    buffer[length] = '\0';

    return buffer;
}

// Funzione per leggere un numero intero
int readInt() {
    int num = 0;
    char ch;

    // Ignora gli spazi e i caratteri di fine riga iniziali
    while ((ch = getc_unlocked(stdin)) != EOF && (ch == ' ' || ch == '\n')){}

    // Leggi e costruisci l'intero
    while (ch <= '9' && ch >= '0') {
        num = num * 10 + (ch - '0');
        ch = getc_unlocked(stdin);
    }

    return num;
}

int binarySearch(Station array[], int dimension, int value){  // O[log(n)]
    int p, u, m;
    p = 0;
    u = dimension - 1;
    if(dimension == 0) return -1;
    if(value < array[p].distance || value > array[u].distance ) return -1;
    while(p <= u) {
        m = (p + u) / 2;
        if(array[m].distance == value)
            return m; // valore x trovato alla posizione m
        if(array[m].distance < value)
            p = m + 1;
        else
            u = m - 1;
    }
    return -1;
}

int carSearch(int array[], int dimension, int value){  // O[log(n)]
    for(int i=0; i<dimension; i++){
        if(array[i] == value){
            return i;
        }
    }

    return -1;
}

int binarySearchModMaxMinor(Station array[], int dimension, int value){
    int left = 0;
    int right = dimension - 1;
    int result = -1;

    while(left <= right){
        int middle = (left + right) / 2;
        if(array[middle].distance < value){
            result = middle;
            left = middle + 1;
        }else{
            right = middle - 1;
        }
    }

    return result;
}

int binarySearchModMinMajor(Station array[], int dimension, int startingIndex, int value){
    int left = startingIndex + 1;
    int right = dimension + startingIndex;
    int result = -1;

    while (left <= right){
        int middle = (left + right) / 2;
        if(array[middle].distance > value){
            result = middle;
            right = middle - 1;
        }else{
            left = middle + 1;
        }
    }

    return result;
}

void aggiungiAuto(Road road, int stationDist, int carAutonomy){  // O[log(n)]
    // ricerca stazione nella lista
    // eventuale modifica di found
    int found = binarySearch(road.stationList, road.stationCounter, stationDist);  // O[log(n)]

    if(found != -1){
        //puntatore viene collegato alla station corretta, modifica l'array di macchine e il flag "modified"
        if(road.stationList[found].maxAutonomy < carAutonomy){
            road.stationList[found].maxAutonomy = carAutonomy ;
            /*road.stationList[found].stationModified = 1;*/
            road.isStationModified = 1;
        }
        road.stationList[found].carPark[road.stationList[found].actualCars] = carAutonomy;
        road.stationList[found].actualCars++;
        printf("aggiunta\n");
    }else{
        printf("non aggiunta\n");
    }
}

void rottamaAuto(Road road, int stationDist, int carAutonomy){  // O[log(n)]
    // ricerca stazione nella lista
    // eventuale modifica di found
    int found = binarySearch(road.stationList, road.stationCounter, stationDist);   // O[log(n)]

    if(found != -1) {
        //puntatore viene collegato alla station corretta, modifica l'array di macchine e il flag "modified"
        int foundCar = carSearch(road.stationList[found].carPark, road.stationList[found].actualCars, carAutonomy);  // O[log(n)]
        if (foundCar != -1) {
            if (road.stationList[found].carPark[foundCar] == road.stationList[found].maxAutonomy) {
                int max = 0;
                for (int i = 0; i < CARPARKSIZE && i != foundCar; i++) {  // O(512) costante
                    if (road.stationList[found].carPark[i] > max)
                        max = road.stationList[found].carPark[i];
                }
                if (max < road.stationList[found].carPark[foundCar]) {
                    road.stationList[found].maxAutonomy = max;
                    /*road.stationList[found].stationModified = 1;*/
                    road.isStationModified = 1;
                }
                road.stationList[found].carPark[foundCar] = road.stationList[found].carPark[road.stationList[found].actualCars - 1];
                road.stationList[found].carPark[road.stationList[found].actualCars - 1] = 0;
                road.stationList[found].actualCars--;
                printf("rottamata\n");
            } else {
                road.stationList[found].carPark[foundCar] = road.stationList[found].carPark[road.stationList[found].actualCars - 1];
                road.stationList[found].carPark[road.stationList[found].actualCars - 1] = 0;
                road.stationList[found].actualCars--;
                printf("rottamata\n");
            }
        } else {
            printf("non rottamata\n");
        }
    }else{
        printf("non rottamata\n");
    }
}

/*
void updateAdjLists(Road road, NodesPool pool, int updateCase){  // O[n^2*log(n)]
    if(updateCase == 1){
        for(int i=0;i<road.stationCounter;i++) {  // O(n) ciclo sulle stazioni per aggiornarle una a una
            int kmMin;
            int kmMax = road.stationList[i].distance + road.stationList[i].maxAutonomy;
            if (road.stationList[i].distance - road.stationList[i].maxAutonomy >= 0) {
                kmMin = road.stationList[i].distance - road.stationList[i].maxAutonomy;
            } else {
                kmMin = 0;
            }

            //remove delle stazioni nei due alberi
            if (i > 0) {

                for (int j = binarySearchModMaxMinor(road.stationList, i, kmMin); j >= 0; j--) {    // O(n)
                    if (searchNode(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance) != 0) { // O[log(n)]
                        // rimuovo la stazione all'albero binario rosso-nero
                        AdjListNode* node = removeNodeRedBlackTree(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance);  // O[log(n)]
                        nodeRemoval(pool, node);  // O(n)
                        road.stationList[i].adjListBackwardsCounter++;
                    }
                }
            }
            if (i < road.stationCounter - 1) {
                for (int k = binarySearchModMinMajor(road.stationList, road.stationCounter-1-i, i, kmMax) ; k <= road.stationCounter - 1; k++) {    // O(n)
                    int index = searchNode(road.stationList[i].adjListHeadForwards, road.stationList[k].distance);  // O[log(n)]
                    if (index >= 0) {
                        // rimuovo la stazione all'albero binario rosso-nero
                        AdjListNode* node = removeNodeRedBlackTree(road.stationList[i].adjListHeadForwards, road.stationList[k].distance);  // O[log(n)]
                        nodeRemoval(pool, node);  // O(n)
                        road.stationList[i].adjListForwardsCounter++;
                    }
                }

            }

            //ricerca ed eventuale aggiunta delle stazioni da aggiungere nei due alberi
            if (i > 0) {
                for (int j = i - 1; road.stationList[j].distance >= kmMin && j >= 0; j--) {     // O(n)
                    if (searchNode(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance) == -1) {      // O[log(n)]
                        // aggiungo la stazione all'albero binario rosso-nero
                        if(road.stationList[i].adjListBackwardsCounter == road.stationList[i].adjListBackwardsDim-1){
                            road.stationList[i].adjListBackwardsDim *= 2;
                            road.stationList[i].adjListHeadBackwards = realloc(road.stationList[i].adjListHeadBackwards, road.stationList[i].adjListBackwardsDim * sizeof(AdjListNode));
                        }
                        insertNodeRedBlackTree(road.stationList[i].adjListHeadBackwards, newNode(road.stationList[j].distance, pool)); // O[log(n)]
                        road.stationList[i].adjListBackwardsCounter++;
                    }
                }
            }
            if (i < road.stationCounter - 1) {
                for (int k = i + 1; road.stationList[k].distance <= kmMax && k <= road.stationCounter - 1; k++) {       // O(n)
                    if (searchNode(road.stationList[i].adjListHeadForwards, road.stationList[k].distance) == -1) {       // O[log(n)]
                        // aggiungo la stazione all'albero binario rosso-nero
                        if(road.stationList[i].adjListForwardsCounter == road.stationList[i].adjListForwardsDim-1){
                            road.stationList[i].adjListForwardsDim *= 2;
                            road.stationList[i].adjListHeadForwards = realloc(road.stationList[i].adjListHeadForwards, road.stationList[i].adjListForwardsDim * sizeof(AdjListNode));
                        }
                        insertNodeRedBlackTree(road.stationList[i].adjListHeadForwards, newNode(road.stationList[k].distance, pool));  // O[log(n)]
                        road.stationList[i].adjListForwardsCounter++;
                    }
                }

            }
        }
    }else if(updateCase == 2) {
        for (int i = 0; i < road.stationCounter; i++) { // O(n) ciclo sulle stazioni per aggiornarle una a una
            if (road.stationList[i].stationModified == 1) {
                int kmMin;
                int kmMax = road.stationList[i].distance + road.stationList[i].maxAutonomy;
                if (road.stationList[i].distance - road.stationList[i].maxAutonomy >= 0) {
                    kmMin = road.stationList[i].distance - road.stationList[i].maxAutonomy;
                } else {
                    kmMin = 0;
                }

                //remove delle stazioni nei due alberi
                if (i > 0) {

                    for (int j = binarySearchModMaxMinor(road.stationList, i, kmMin); j >= 0; j--) {        // O(n)
                        int index = searchNode(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance);     // O[log(n)]
                        if (index >= 0) {
                            // rimuovo la stazione all'albero binario rosso-nero
                            AdjListNode* node = removeNodeRedBlackTree(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance);  // O[log(n)]
                            nodeRemoval(pool, node);  // O(n)
                            road.stationList[i].adjListBackwardsCounter++;
                        }
                    }
                }
                if (i < road.stationCounter - 1) {

                    for (int k = binarySearchModMinMajor(road.stationList, road.stationCounter - 1 - i, i, kmMax);
                         k <= road.stationCounter - 1; k++) {       // O(n)
                        int index = searchNode(road.stationList[i].adjListHeadForwards, road.stationList[k].distance); // O[log(n)]
                        if (index >= 0) {
                            // rimuovo la stazione all'albero binario rosso-nero
                            AdjListNode* node = removeNodeRedBlackTree(road.stationList[i].adjListHeadForwards, road.stationList[k].distance);  // O[log(n)]
                            nodeRemoval(pool, node);  // O(n)
                            road.stationList[i].adjListForwardsCounter++;
                        }
                    }

                }

                //ricerca ed eventuale aggiunta delle stazioni da aggiungere nei due alberi
                if (i > 0) {
                    for (int j = i - 1; road.stationList[j].distance >= kmMin && j >= 0; j--) {     // O(n)
                        if (searchNode(road.stationList[i].adjListHeadBackwards, road.stationList[j].distance) == -1) {      // O[log(n)]
                            // aggiungo la stazione all'albero binario rosso-nero
                            if(road.stationList[i].adjListBackwardsCounter == road.stationList[i].adjListBackwardsDim-1){
                                road.stationList[i].adjListBackwardsDim *= 2;
                                road.stationList[i].adjListHeadBackwards = realloc(road.stationList[i].adjListHeadBackwards, road.stationList[i].adjListBackwardsDim * sizeof(struct node*));
                            }
                            insertNodeRedBlackTree(road.stationList[i].adjListHeadBackwards, newNode(road.stationList[j].distance, pool));  // O[log(n)]
                            road.stationList[i].adjListBackwardsCounter++;
                        }
                    }
                }
                if (i < road.stationCounter - 1) {
                    for (int k = i + 1; road.stationList[k].distance <= kmMax && k <= road.stationCounter - 1; k++) {   // O(n)
                        if (searchNode(road.stationList[i].adjListHeadForwards, road.stationList[k].distance) == -1) {       // O[log(n)]
                            // aggiungo la stazione all'albero binario rosso-nero
                            if(road.stationList[i].adjListForwardsCounter == road.stationList[i].adjListForwardsDim-1){
                                road.stationList[i].adjListForwardsDim *= 2;
                                road.stationList[i].adjListHeadForwards = realloc(road.stationList[i].adjListHeadForwards, road.stationList[i].adjListForwardsDim*sizeof(AdjListNode));
                            }
                            insertNodeRedBlackTree(road.stationList[i].adjListHeadForwards, newNode(road.stationList[k].distance, pool));  // O[log(n)]
                            road.stationList[i].adjListForwardsCounter++;
                        }
                    }
                }
            }
        }
    }
}
*/

void shortestPathForward(Station* stationList, int stationCounter, int station1Dist, int station2Dist, MarkerArray* markerArray, int startingIndex){
    //TODO
    int markerIndex = 0;
    int foundIndex = 0;

    for(int i=startingIndex;stationList[i].distance<station2Dist && i<stationCounter;i++){
        foundIndex = i;
        if(stationList[i].distance<markerArray[markerIndex].distance || markerArray[markerIndex].distance == -1){
            int maxKm = stationList[i].distance + stationList[i].maxAutonomy;
            if(maxKm>markerArray[markerIndex].distance){
                if(markerArray[markerIndex].distance == -1) {
                    markerArray[markerIndex].distance = maxKm;
                    markerArray[markerIndex].steps = 1;
                    markerArray[markerIndex].predecessor = stationList[i].distance;
                }else{
                    markerIndex++;
                    markerArray[markerIndex].distance = maxKm;
                    markerArray[markerIndex].steps = markerArray[markerIndex-1].steps+1;
                    markerArray[markerIndex].predecessor = stationList[i].distance;
                }
            }
        }else if(stationList[i].distance == markerArray[markerIndex].distance){
            int maxKm = stationList[i].distance + stationList[i].maxAutonomy;
            if(maxKm>markerArray[markerIndex].distance){
                markerIndex++;
                markerArray[markerIndex].distance = maxKm;
                markerArray[markerIndex].steps = markerArray[markerIndex-1].steps+1;
                markerArray[markerIndex].predecessor = stationList[i].distance;
            }
        }
    }

    for(int j=0; j<=markerIndex; j++){
        if(markerArray[j].distance>=station2Dist){
            int* predecessorArray = malloc((markerArray[j].steps+1)*sizeof(int));
            predecessorArray[markerArray[j].steps] = station2Dist;
            int actualPredecessor = markerArray[j].predecessor;
            int searchIndex = markerArray[j].steps-1;

            if(markerIndex == 0){
                predecessorArray[0] = station1Dist;
            }else { //sistemare
                for (int k = j - 1; k >= 0 && searchIndex >= 0; k--) {
                    if (markerArray[k].distance > actualPredecessor) {
                        predecessorArray[searchIndex] = actualPredecessor;
                        searchIndex--;
                        actualPredecessor = markerArray[k].predecessor;
                    }
                }
                predecessorArray[0] = station1Dist;
            }

            //stampa
            for(int l=0; l<=markerArray[j].steps; l++){
                printf("%d ", predecessorArray[l]);
            }
            printf("\n");
            return;
        }
    }

    printf("nessun percorso\n");
}

void shortestPathBackward(Station* stationList, int stationCounter, int station1Dist, int station2Dist, MarkerArray* markerArray, int startingIndex){
    //TODO
    int markerIndex = 0;
    int foundIndex = 0;

    for(int i=startingIndex; stationList[i].distance>station2Dist && i>=0;i--){
        foundIndex = i;
        if(stationList[i].distance>markerArray[markerIndex].distance || markerArray[markerIndex].distance == INT_MAX){
            int minKm = stationList[i].distance - stationList[i].maxAutonomy;
            if(minKm<0){
                minKm=0;
            }
            if(minKm<markerArray[markerIndex].distance){
                if(markerArray[markerIndex].distance == INT_MAX) {
                    markerArray[markerIndex].distance = minKm;
                    markerArray[markerIndex].steps = 1;
                    markerArray[markerIndex].predecessor = stationList[i].distance;
                }else{
                    markerIndex++;
                    markerArray[markerIndex].distance = minKm;
                    markerArray[markerIndex].steps = markerArray[markerIndex-1].steps+1;
                    markerArray[markerIndex].predecessor = stationList[i].distance;
                }
            }
        }else if(stationList[i].distance == markerArray[markerIndex].distance){
            int minKm = stationList[i].distance - stationList[i].maxAutonomy;
            if(minKm<markerArray[markerIndex].distance){
                markerIndex++;
                markerArray[markerIndex].distance = minKm;
                markerArray[markerIndex].steps = markerArray[markerIndex-1].steps+1;
                markerArray[markerIndex].predecessor = stationList[i].distance;
            }
        }
    }

    for(int j=0; j<=markerIndex; j++){
        if(markerArray[j].distance<=station2Dist){
            int* predecessorArray = malloc((markerArray[j].steps+1)*sizeof(int));
            predecessorArray[markerArray[j].steps] = station2Dist;
            int actualPredecessor = markerArray[j].predecessor;
            int searchIndex = markerArray[j].steps-1;

            if(markerIndex == 0) {
                predecessorArray[0] = station1Dist;
            }else {
                for (int k = j - 1; k >= 0 && searchIndex >= 0; k--) {
                    if (markerArray[k].distance < actualPredecessor) {
                        predecessorArray[searchIndex] = actualPredecessor;
                        searchIndex--;
                        actualPredecessor = markerArray[k].predecessor;
                    }
                }
                predecessorArray[0] = station1Dist;
            }

            //stampa
            for(int l=0; l<=markerArray[j].steps; l++){
                printf("%d ", predecessorArray[l]);
            }
            printf("\n");
            return;
        }
    }

    printf("nessun percorso\n");
}

void pianificaPercorso(Road road, /*NodesPool pool,*/ int station1Dist, int station2Dist){
    /*
     * if(road.roadModified==1){
        updateAdjLists(road, pool, 1);
    }else if(road.roadModified==0) {
        if (road.isStationModified == 1) {
            updateAdjLists(road, pool, 2);
        }
    }
     */

    //calcolo effettivo percorso
    if(station1Dist == station2Dist) {
        printf("%d\n", station1Dist);
    }else if(station1Dist<station2Dist) {
        //uso liste forwards
        MarkerArray* markerArray = malloc(road.stationCounter * sizeof (MarkerArray));
        int helper;
        for(int i=0; i<road.stationCounter; i++) {
            if (road.stationList[i].distance == station1Dist) {
                markerArray[i].distance = -1;
                helper = i;
            } else {
                markerArray[i].distance = -1;
            }
        }
        shortestPathForward(road.stationList, road.stationCounter, station1Dist, station2Dist, markerArray, helper);
    }else if(station1Dist>station2Dist) {
        //uso liste backwards
        MarkerArray* markerArray = malloc(road.stationCounter * sizeof (MarkerArray));
        int helper = 0;
        for(int i=0; i<road.stationCounter; i++) {
            if (road.stationList[i].distance == station1Dist) {
                markerArray[i].distance = INT_MAX;
                helper = i;
            } else {
                markerArray[i].distance = INT_MAX;
            }
        }
        shortestPathBackward(road.stationList, road.stationCounter, station1Dist, station2Dist, markerArray, helper);
    }

}

int aggiungiStazione(Road road, int stationDist, int carNumb, int carSpecs[]){  // O(n)
    // ricerca stazione nella lista
    // eventuale modifica di found
    int found = binarySearch(road.stationList, road.stationCounter, stationDist); // O[log(n)]

    //aggiunta stazione e parco auto
    if(found != -1){
        printf("non aggiunta\n");
        return 0;
    } else{
        //inserimento nella struttura e aggiornamento adjacency lists
        if(road.stationCounter == 0){
            road.stationList[road.stationCounter].distance = stationDist;
            road.stationList[road.stationCounter].actualCars = carNumb;
            int max = 0;
            for(int i=0; i<CARPARKSIZE; i++) {              // O(512) costante
                if(i<carNumb){
                    road.stationList[road.stationCounter].carPark[i] = carSpecs[i];
                    if(road.stationList[road.stationCounter].carPark[i] > max)
                        max = road.stationList[road.stationCounter].carPark[i];
                }else{
                    road.stationList[road.stationCounter].carPark[i] = 0;
                }
            }
            road.stationList[road.stationCounter].maxAutonomy = max;

            /*
             * road.stationList[road.stationCounter].adjListHeadBackwards = (AdjListNode*) malloc(100 * sizeof (AdjListNode));
             * road.stationList[road.stationCounter].adjListHeadForwards = (AdjListNode*) malloc(100 * sizeof (AdjListNode));
             */

        }else {
            road.stationList[road.stationCounter].distance = stationDist;
            road.stationList[road.stationCounter].actualCars = carNumb;
            int max = 0;
            for(int i=0; i<CARPARKSIZE; i++) {              // O(512) costante
                if(i<carNumb){
                    road.stationList[road.stationCounter].carPark[i] = carSpecs[i];
                    if(road.stationList[road.stationCounter].carPark[i] > max)
                        max = road.stationList[road.stationCounter].carPark[i];
                }else{
                    road.stationList[road.stationCounter].carPark[i] = 0;
                }
            }
            road.stationList[road.stationCounter].maxAutonomy = max;

            /*
             * road.stationList[road.stationCounter].adjListForwardsDim = 100;
             * road.stationList[road.stationCounter].adjListBackwardsDim = 100;
             * road.stationList[road.stationCounter].adjListHeadBackwards = (AdjListNode*) malloc(road.stationList[road.stationCounter].adjListBackwardsDim * sizeof (AdjListNode));
             * road.stationList[road.stationCounter].adjListHeadForwards = (AdjListNode*) malloc(road.stationList[road.stationCounter].adjListForwardsDim * sizeof (AdjListNode));
             */

            bubbleSort(road.stationList, road.stationCounter+1);  // O(n)
        }

        road.roadModified = 1;

        if(road.stationCounter+1==(road.roadDim-1)){
            road.roadDim = road.roadDim * 2;
            road.stationList = (Station*) realloc(road.stationList, road.roadDim*sizeof(Station));
            if(road.stationList == NULL){
                printf("Errore allocazione memoria.\n");
                exit(EXIT_FAILURE);
            }
        }
        fprintf(stdout, "aggiunta\n");
        return 1;
    }
}

int demolisciStazione(Road road, int stationDist){  // O(n)
    // ricerca stazione nella lista
    // eventuale modifica di found
    int found = binarySearch(road.stationList, road.stationCounter, stationDist); // O[log(n)]

    //rimozione stazione
    if(found != -1){
        /*
        free(road.stationList[found].adjListHeadForwards);
        free(road.stationList[found].adjListHeadBackwards);
         */

        for(int i = found; i<road.stationCounter-1; i++){  // O(n)
            road.stationList[i] = road.stationList[i+1];
        }

        road.roadModified = 1;

        printf("demolita\n");
        return 1;
    } else{
        printf("non demolita\n");
        return 0;
    }
}

/*
int main() {
    int distance, carNumber, autonomy;

    Road road;
    road.roadModified = 0;
    road.roadDim = 100;
    road.stationCounter = 0;

    road.stationList = (Station*) malloc(road.roadDim * sizeof (Station));
    if(road.stationList == NULL){
        printf("Errore allocazione memoria.\n");
        exit(EXIT_FAILURE);
    }

    NodesPool nodesPool;
    nodesPool.nextAvailableIndex = 0;
    nodesPool.poolDim = 100;

    nodesPool.nodes = (AdjListNode*) malloc(nodesPool.poolDim * sizeof(AdjListNode));
    if(nodesPool.nodes == NULL){
        printf("Errore allocazione memoria.\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<nodesPool.poolDim;i++){
        nodesPool.nodes[i].inUse = 0;
    }


    char* command = malloc(20 * sizeof(char));
    while ((command = readWord(command)) != NULL) {
        // Analizza il comando
        if (strcmp(command, "aggiungi-stazione") == 0) {
            distance = readInt();
            carNumber = readInt();
            int* autonomies = malloc(carNumber * sizeof(int));
            for (int i = 0; i < carNumber; i++) {
                autonomies[i] = readInt();
            }
            aggiungiStazione(road, distance, carNumber, autonomies);
        } else if (strcmp(command, "demolisci-stazione") == 0) {
            distance = readInt();
            demolisciStazione(road, distance);
        } else if (strcmp(command, "pianifica-percorso") == 0) {
            int startStation = readInt();
            int endStation = readInt();
            pianificaPercorso(road, nodesPool, startStation, endStation);
        } else if (strcmp(command, "aggiungi-auto") == 0) {
            distance = readInt();
            autonomy = readInt();
            aggiungiAuto(road, distance, autonomy);
        } else if (strcmp(command, "rottama-auto") == 0) {
            distance = readInt();
            autonomy = readInt();
            rottamaAuto(road, distance, autonomy);
        }
    }

    free(road.stationList);

    return 0;
}
*/


int main() {
    int distance, carNumber, autonomy, startStation, endStation;
    int autonomies[512];

    Road road;
    road.roadModified = 0;
    road.roadDim = 100;
    road.stationCounter = 0;

    road.stationList = (Station*) malloc(road.roadDim * sizeof (Station));
    if(road.stationList == NULL){
        printf("Errore allocazione memoria.\n");
        exit(EXIT_FAILURE);
    }

    /*
    NodesPool nodesPool;
    nodesPool.nextAvailableIndex = 0;
    nodesPool.poolDim = 100;

    nodesPool.nodes = (AdjListNode*) malloc(nodesPool.poolDim * sizeof(AdjListNode));
    if(nodesPool.nodes == NULL){
        printf("Errore allocazione memoria.\n");
        exit(EXIT_FAILURE);
    }
    for(int i=0;i<nodesPool.poolDim;i++){
        nodesPool.nodes[i].inUse = 0;
    }
     */


    char* command = malloc(20 * sizeof(char));
    while (fscanf(stdin, "%s", command) != EOF) {

        // Analizza il comando
        if (strcmp(command, "aggiungi-stazione") == 0) {
            if(fscanf(stdin, "%d %d", &distance, &carNumber) != EOF){
                for (int i = 0; i < carNumber; i++) {
                    if(fscanf(stdin, "%d", &autonomies[i]) != EOF){
                    }
                }
                if(aggiungiStazione(road, distance, carNumber, autonomies)) {
                    road.stationCounter++;
                }
            }
        } else if (strcmp(command, "demolisci-stazione") == 0) {
            if(fscanf(stdin, "%d", &distance) != EOF){
                if(demolisciStazione(road, distance)){
                    road.stationCounter--;
                }
            }
        } else if (strcmp(command, "pianifica-percorso") == 0) {
            if(fscanf(stdin, "%d %d", &startStation, &endStation) != EOF){
                pianificaPercorso(road, /*nodesPool,*/ startStation, endStation);
            }
        } else if (strcmp(command, "aggiungi-auto") == 0) {
            if(fscanf(stdin, "%d %d", &distance, &autonomy) != EOF){
                aggiungiAuto(road, distance, autonomy);
            }
        } else if (strcmp(command, "rottama-auto") == 0) {
            if(fscanf(stdin, "%d %d", &distance, &autonomy) != EOF){
                rottamaAuto(road, distance, autonomy);
            }
        }else if(strcmp(command, "fine") == 0){
            break;
        }
    }

    free(road.stationList);

    return 0;
}