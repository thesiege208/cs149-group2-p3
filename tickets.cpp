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
int availableSeats = 100; // 10 x 10 seats
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

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;		// Placeholder
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;	// Placeholder

pthread_mutex_t seatsMutex = PTHREAD_MUTEX_INITIALIZER;	// Seats
pthread_mutex_t buyersMutex = PTHREAD_MUTEX_INITIALIZER;// Buyers mutex - delete if we dont need

pthread_cond_t HQcond = PTHREAD_COND_INITIALIZER;	// High Seller Queue condition
pthread_mutex_t HQmutex = PTHREAD_MUTEX_INITIALIZER;	// High Seller Queue mutex
pthread_cond_t MQcond = PTHREAD_COND_INITIALIZER;	// Mid Sellers Queue condition
pthread_mutex_t MQmutex = PTHREAD_MUTEX_INITIALIZER;	// Mid Sellers Queue mutex
pthread_cond_t LQcond = PTHREAD_COND_INITIALIZER;	// Low Sellers Queue condition
pthread_mutex_t LQmutex = PTHREAD_MUTEX_INITIALIZER;	// Low Sellers Queue mutex

bool soldOut()
{
    pthread_mutex_lock(&seatsMutex);
    bool result = (availableSeats == 0);
    pthread_mutex_unlock(&seatsMutex);
    return result;
}

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
	switch(seller_type) {	// Determine which mutex and condition to use
        	case 'H': //high priority
            		mutex = HQmutex;
            		cond = HQcond;
            		break;
        	case 'M':  //medium priority
            		mutex = MQmutex;
            		cond = MQcond;
            		break;
        	case 'L':  //low priority
            		waitMutex = LQmutex;
            		waitCond = LQcond;
            		break;
    	}
    	// while(true) {
        //	pthread_mutex_lock(&mutex);
        //	pthread_cond_wait(&cond, &mutex);	// Block thread on condition variable
        //	pthread_mutex_unlock(&mutex);
        //	if (soldOut()) { break;	}	// break if no more seats available
        //	get next buyer			// code to get next buyer from queue
        //	while (buyer != NULL)		// while buyer exists
        //	{
        //    		count++;		// increment sold count
        //    		sell ticket		// code to sell ticket
        //    		if(count!=10)		// while seller not sold out
        //      		get next buyer	// code to get next buyer
	//	}
        //	if(soldOut()) { break; }	// break if no more seats available
        //	else if(count==10){ break; }	// else break if seller is sold out
    	// }
    	// pthread_cancel(this thread ID);	// close this seller's thread
    	return NULL;
}

/* function which wakes up threads from suspension to execute in parallel */
void wakeup_all_seller_threads() {
    // ROGER
    // dealing with mutex lock/unlock and cond broadcast stuff
    // sorry i can't help you much here :(
	pthread_mutex_lock(&HQmutex);	// High price seller queue
    	pthread_cond_broadcast(&HQcond);
    	pthread_mutex_unlock(&HQmutex);
	pthread_mutex_lock(&MQmutex);	// Mid price sellers queue
    	pthread_cond_broadcast(&MQcond);
    	pthread_mutex_unlock(&MQmutex);
	pthread_mutex_lock(&LQmutex);	// Low price sellers queue
    	pthread_cond_broadcast(&LQcond);
    	pthread_mutex_unlock(&LQmutex);
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

