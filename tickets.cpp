/* SJSU Fall 2018
 * CS149 Operating Systems
 * Project 3
 * Group 2
 * Ranying Cai, Tianyun Chen, Roger Kuo, Sijing Xie
 */

#include <iostream>
#include <cstdlib>
#include <queue>
#include <string>
#include <map>
#include <pthread.h>

using namespace std;

#define NUM_THREADS 10

int minute = 0; /* 0 - 59, in minutes */
map<int, string> seats; /* map holding all the sold seats */
// dynamically add seats as they're sold using key-value pairs of 
// <seat #, customer name>
// can just check if key exists in map, if not then seat is empty
// print after filling in empty seats (if exist) with dashes

// init vars/arrays to track customer events per seller?

/* customer queues for each seller.
 * will be initialized in respective thread.
 */
priority_queue<int> h1, m1, m2, m3, l1, l2, l3, l4, l5, l6;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* start routine which hanndles all transactions and printing 
 * where arg seller_type is either H, M or L
 */
void * sell(char *seller_type) {
    // RANYING, TIANYUN
    // remember diff level sellers have diff transaction times (rand()?)
    // h starts from row 1
    // l starts from row 10
    // m starts at row 5, 6, 4, 7, etc.
    // remember to give customer names based on seller and place in queue
    // remember to print 10x10 seating chart after each ticket sale
    // remember to update tracking vars/arrays
    return NULL;
}

/* function which wakes up threads from suspension to execute in parallel */
void wakeup_all_seller_threads() {
    // ROGER
    // dealing with mutex lock/unlock and cond broadcast stuff
    // sorry i can't help you much here :(
}

/* where arg N is the command line option for # of customers per queue */
int main() {
    // SIJING
    // An Outline (also refer to p3 preview)
    // 1. init pthreads and suspend
    // 2. prompt command line input for N (5, 10, 15)
    // 3. loop through threads, init queues, init random N arrival times per pq
    // 4. wake up threads to execute in parallel (define new funct for this)
    // 5. exit after all threads complete
    int n;
    pthread_t tids[10];
    char seller_type;
    
    // prompting command line input
    printf("\nEnter the number of customers per queue (5, 10, or 15): "); 
    scanf("%d", n);

    // create H thread
    seller-type = 'H';
    pthread_create(&tids[0], NULL, sell, &seller-type);

    // create M threads
    seller-type = 'M';
    for (int i = 1; i < 4; i++) {
        pthread_create(&tids[i], NULL, sell, &seller-type);
    }

    // create L threads
    seller-type = 'L';
    for (int i = 4; i < 10; i++) {
        pthread_create(&tids[i], NULL, sell, &seller-type);
    }

	// init pqs with n rand customers
	
	// wakeup_all_seller_threads()

    // after all threads complete execution, join back into main thread
    // for (int i = 0 ; i < 10 ; i++) {
    //     pthread_join(&tids[i], NULL);
    // }

    return 0;
}

