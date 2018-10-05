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

pthread_cond_t cond = PTHREAD_COND_INITIALIZER; // ???
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // ???

// define start routine funct sell(seller_type), includes all printing
// remember diff level sellers have diff transaction times (rand()?)
// h starts from row 1
// l starts from row 10
// m starts at row 5, 6, 4, 7, etc.
// remember to give customer names based on seller and place in queue
// remember to print 10x10 seating chart after each ticket sale
// remember to update tracking vars/arrays

// define wake up threads in parallel function

/* where arg N is the command line option for # of customers per queue */
int main() {
    // An Outline (also refer to p3 preview)
    // 1. init pthreads and suspend
    // 2. prompt command line input for N (5, 10, 15)
    // 3. loop through threads, init queues, init random N arrival times per pq
    // 4. wake up threads to execute in parallel (define new funct for this)
    // 5. exit after all threads complete
    return 0;
}

