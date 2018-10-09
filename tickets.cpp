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
#include <unistd.h>

using namespace std;

int N = 2; // number of customer in each queue.

struct Customer {
    int customerId;
    int arrivalTime;
    int completeTime;

    Customer(int cId, int aTime, int cTime) {
        this->customerId = cId;
        this->arrivalTime = aTime;
        this->completeTime = cTime;
    }
};


string seat[10][10];
pthread_mutex_t mutex;
pthread_cond_t cond;


bool assignLowSeat(string seatId) {
    for (int i = 0; i < 10; i++){
            for (int j = 0; j < 10; j++){
                if ( seat[i][j] == "" ) {
                    seat[i][j] = seatId;
                    return true;
                }
            }
        }
    return false;
}

bool assignMiddleSeat(string seatId) {
    int middleRow = 4;
    for (int rowOffSet = 0; middleRow + rowOffSet >= 0 && middleRow + rowOffSet < 10; rowOffSet += 1) {
            for (int k = -1; k < 2; k += 2) {
                int i = middleRow + k * rowOffSet;
                for (int j = 0; j < 10; j++){
                    if ( seat[i][j] == "" ) {
                        seat[i][j] = seatId;
                        return true;
                    }
                }
            }
        }
    return false; 
}

bool assignHighSeat(string seatId) {
    for (int i = 9; i >= 0; i--){
            for (int j = 0; j < 10; j++){
                if ( seat[i][j] == "" ) {
                    seat[i][j] = seatId;
                    return true;
                }
            }
        }
    return false;
}

// Assign seat to customer.
// Now we assume the seller is 'L'. But we should 
// also handle 'M' and 'H'.
bool assignSeats(string seller, Customer customer) {
    char sellerType = seller[0];
    string assignedSeatId = seller  + '_' + to_string(customer.customerId);
    if (sellerType == 'L') {
        return assignLowSeat(assignedSeatId);
    } else if (sellerType == 'M') {
        return assignMiddleSeat(assignedSeatId);
    } else {// High level
        return assignHighSeat(assignedSeatId);
    }
}

void printTable() {
     int k = 0;
     cout<<"*******table******\n";
     for (int i = 0; i < 10; i++){
         for (int j = 0; j < 10; j++){
             if (seat[i][j].empty() == false) {
               cout << seat[i][j]<<"\t";
             } else {
                cout << "0" << "\t";
             }
         }
         cout<<"\n";
         continue;
     }
}

// Convert the sellerID from 0 to 9 to L1...L6, M1..M3, H1
string mapSellerIdToName(long sellerId) {
    if (sellerId < 6) { // 0 - 5
        return "L" + to_string(sellerId + 1);
    } else if (sellerId < 9) { // 6 - 8
        return "M" + to_string(sellerId + 1 - 6);
    } else { // 9
        return "H" + to_string(sellerId + 1 - 9);
    }
}


// Need to turn this into random.
// 1. We can put all customer in vector first
// 2. then sort them based on arrival time.
// 3. then turn it into queue.
queue<Customer> generateRandomCustomerQueue(int n, char sellerType) {
    queue<Customer> customerQueue;
    customerQueue.push(Customer(1, 5, 4));
    customerQueue.push(Customer(2, 10, 2));
    customerQueue.push(Customer(3, 15, 2));
    customerQueue.push(Customer(4, 19, 2));
    customerQueue.push(Customer(5, 20, 2));

    return customerQueue;
}

void *eachSeller(void *sellerId) {
    string sellerName = mapSellerIdToName((long) sellerId);

    cout << "here we are in the seller thread function " << sellerName << endl;
    // Generate random customer queue for this seller.
    // seller is the name of this seller, for example, "L1", so seller[0] is the type of seller, for example, 'L'.
    char sellerType = sellerName[0];
    queue<Customer> cQ = generateRandomCustomerQueue(N, sellerType); 

    int currentTimeStamp = 0;
    bool stillHasSeat = true;
    while (currentTimeStamp < 60 && !cQ.empty()) {
        Customer currentCustomer = cQ.front();
        if (currentCustomer.arrivalTime > currentTimeStamp) {
            // The customer hasn’t arrived yet, wait until he/she arrives
            int waitTime = currentCustomer.arrivalTime - currentTimeStamp;
            currentTimeStamp += waitTime; //get the complete time stamp
            sleep(waitTime);
        } else {
            //This customer has already arrived, lock the seat and assign to this customer
            pthread_mutex_lock(&mutex);
             //Assign seats to customers
            stillHasSeat = assignSeats(sellerName, currentCustomer);
            if (stillHasSeat == false) {
                // No seat any more
                break;
            }
            // Print the current table, for debugging only.
            printTable();
            // Unlock the table since this seller already book the seat for the customer
            pthread_mutex_unlock(&mutex);
            
            //keep working for the customer with complete time 
            currentTimeStamp = currentTimeStamp + currentCustomer.completeTime; 
            cout<<"book the seat for a customer, everything will be finished in time stamp :"<<currentTimeStamp<<"\n";
            cQ.pop(); //remove customer who complete purchase
            sleep(currentCustomer.completeTime);// actual working for completeTime
        }
    }
    pthread_exit(NULL);
}


/* where arg N is the command line option for # of customers per queue */
int main() {
    int numberOfSellers = 10;
    pthread_t threads[numberOfSellers];

    for (int i = 0; i < numberOfSellers; i++) {
        int sellerId = i;
        pthread_create(&threads[i], NULL, eachSeller,  (void*) sellerId);
    }

    for (int i = 0; i < numberOfSellers; i++) {
        pthread_join(threads[i], 0);
    }
}


// #define NUM_THREADS 10

// int minute = 0; /* 0 - 59, in minutes */
// int availableSeats = 100; // 10 x 10 seats
// map<int, string> seats; /* map holding all the sold seats */
// // dynamically add seats as they're sold using key-value pairs of 
// // <seat #, customer name>
// // can just check if key exists in map, if not then seat is empty
// // print after filling in empty seats (if exist) with dashes

// // init vars/arrays to track customer events per seller?

// /* customer queues for each seller.
//  * will be initialized in respective thread.
//  */
// priority_queue<int> h1, m1, m2, m3, l1, l2, l3, l4, l5, l6;

// pthread_mutex_t seatsMutex = PTHREAD_MUTEX_INITIALIZER;	// Seats
// pthread_mutex_t buyersMutex = PTHREAD_MUTEX_INITIALIZER;// Buyers mutex

// pthread_cond_t HQcond = PTHREAD_COND_INITIALIZER;	// High Seller Queue condition
// pthread_mutex_t HQmutex = PTHREAD_MUTEX_INITIALIZER;	// High Seller Queue mutex
// pthread_cond_t MQcond = PTHREAD_COND_INITIALIZER;	// Mid Sellers Queue condition
// pthread_mutex_t MQmutex = PTHREAD_MUTEX_INITIALIZER;	// Mid Sellers Queue mutex
// pthread_cond_t LQcond = PTHREAD_COND_INITIALIZER;	// Low Sellers Queue condition
// pthread_mutex_t LQmutex = PTHREAD_MUTEX_INITIALIZER;	// Low Sellers Queue mutex

// bool soldOut()
// {
//     pthread_mutex_lock(&seatsMutex);
//     bool result = (availableSeats == 0);
//     pthread_mutex_unlock(&seatsMutex);
//     return result;
// }

// /*
// // Use or delete anything that you think will help. 
// // These might work if we create some new classes or objects
// // But dont use it if it doesnt look right. I'm just adding suggestions.
// void sellTicket(buyer*, seller*)
// {
//     getTicket(buyer, seller)				// code to get ticket
//     if(ticket != NULL){					// if ticket exists
// 	int waitT = 0;					// set wait time
//         if(sellerType=='H') waitT = (rand() % 2) + 1;		// 1-2 minutes
//         if(sellerType=='M') waitT = (rand() % 3) + 2;		// 2-4 minutes
//         if(sellerType=='L') waitT = (rand() % 4) + 4;      	// 4-7 minutes
//         sleep(waitT);					// wait for x amount of time
//         pthread_mutex_lock(&mutex);
//         pthread_cond_signal(&cond);			// unblock thread
//         pthread_mutex_unlock(&mutex);
//     }
// }

// Ticket* getTicket(buyer*, seller*)	// Depending on if we make a ticket object or something
// {
//     char str[10];
//     ticket=NULL;
//     pthread_mutex_lock(&seatsMutex);
//     if(availableSeats != 0) {
//         if (sellerType == 'H') { 	// high priority
//             // do something
//         }
//         if (sellerType == 'M') {	// medium priority
//             // do something
//         }
//         if (sellerType == 'L') {	// low priority
// 	    // do something
//         }
//         if(ticket != NULL) {
//             // something else
//         }
//     }
//     pthread_mutex_unlock(&seatsMutex);
//     return ticket;
// }

// Buyer* nextBuyer(char buyerType)			// Depending on if we create some class or object for buyers.
// {							// Idk if this is a good or bad idea. Maybe Im making it complicated.
//     buyer = NULL;
//     switch(buyerType){
//         case 'H':					// high priority
//             pthread_mutex_lock(&HQmutex);
//             if(queueH.size() > 0){			// assuming we create a queue of buyers
//                 buyer = queueH.front();			// get next buyer
//                 queueH.pop_front();			// delete buyer from queue
//                 pthread_mutex_lock(&buyersMutex);
//                 // insert code				// do whatever necessary
//                 pthread_mutex_unlock(&buyersMutex);
//             }
//             pthread_mutex_unlock(&HQmutex);
//             break;
//         case 'M':					// medium priority
//             pthread_mutex_lock(&MQmutex);
//             if(queueM.size() > 0){
//                 buyer = queueM.front();
//                 queueM.pop_front();
//                 pthread_mutex_lock(&buyersMutex);
//                 // insert code
//                 pthread_mutex_unlock(&buyersMutex);
//             }
//             pthread_mutex_unlock(&MQmutex);
//             break;
//         case 'L':					// low priority
//             pthread_mutex_lock(&LQmutex);
//             if(queueL.size() > 0){
//                 buyer = queueL.front();
//                 queueL.pop_front();
//                 pthread_mutex_lock(&buyersMutex);
//                 // insert code
//                 pthread_mutex_unlock(&buyersMutex);
//             }
//             pthread_mutex_unlock(&LQmutex);
//             break;
//     }
//     return buyer;
// }
// */

// /* start routine which hanndles all transactions and printing 
//  * where arg seller_type is either H, M or L
//  */
// void * sell(char *seller_type) {
//     // RANYING, TIANYUN
//     // remember diff level sellers have diff transaction times (rand()?)
//     // h starts from row 1
//     // l starts from row 10
//     // m starts at row 5, 6, 4, 7, etc.
//     // remember to give customer names based on seller and place in queue
//     // remember to print 10x10 seating chart after each ticket sale
//     // remember to update tracking vars/arrays
    
//     /* some useful map functions:
//      * to add a key-value pair: 
//      *     seats.insert({<seat #, cust name>});
//      * OR  seats.emplace(<seat #, cust name>);
//      * to get the value at a specific key:
//      *     string custName = seats.at(<seat #>);
//      * to iterate through the map:
//      *     iterator it = seats.begin(); <-- sets iterator to first element
//      *     iterator it = seats.end(); <-- sets iterator to last element
//      *     for (iterator it = seats.begin(); it != seats.end(); ++it) {}
//      */
//     	/*
// 	switch(seller_type) {	// Determine which mutex and condition to use
//         	case 'H': //high priority
//             		mutex = HQmutex;
//             		cond = HQcond;
//             		break;
//         	case 'M':  //medium priority
//             		mutex = MQmutex;
//             		cond = MQcond;
//             		break;
//         	case 'L':  //low priority
//             		mutex = LQmutex;
//             		cond = LQcond;
//             		break;
//     	}
	
// 	// sample code, dont use if it doesnt help.
//     	while(true) {
//         	pthread_mutex_lock(&mutex);
//         	pthread_cond_wait(&cond, &mutex);	// Block thread on condition variable
//         	pthread_mutex_unlock(&mutex);
//         	if (soldOut()) { break;	}	// break if no more seats available
//         	buyer = nextBuyer(char buyerType) // code to get next buyer from queue
//         	while (buyer != NULL)		// while buyer exists
//         	{
//             		count++;		// increment sold count
//             		sellTicket(buyer*, seller*)		// code to sell ticket
//             		if(count!=10)		// while seller not sold out
//               		get next buyer	// code to get next buyer
// 		}
//         	if(soldOut()) { break; }	// break if no more seats available
//         	else if(count==10){ break; }	// else break if seller is sold out
//     	}
//     	pthread_cancel(this thread ID);	// close this seller's thread
// 	*/
//     	return NULL;
// }

// /* function which wakes up threads from suspension to execute in parallel */
// void wakeup_all_seller_threads() {
//     // ROGER
// 	pthread_mutex_lock(&HQmutex);	// High price seller queue
//     	pthread_cond_broadcast(&HQcond);
//     	pthread_mutex_unlock(&HQmutex);
// 	pthread_mutex_lock(&MQmutex);	// Mid price sellers queue
//     	pthread_cond_broadcast(&MQcond);
//     	pthread_mutex_unlock(&MQmutex);
// 	pthread_mutex_lock(&LQmutex);	// Low price sellers queue
//     	pthread_cond_broadcast(&LQcond);
//     	pthread_mutex_unlock(&LQmutex);
// }

// /* where arg N is the command line option for # of customers per queue */
// int main() {
//     // SIJING
//     // 5. exit after all threads complete
//     int n;
//     pthread_t tids[10];
//     char seller_type;
    
//     // prompting command line input
//     printf("\nEnter the number of customers per queue (5, 10, or 15): "); 
//     scanf("%d", n);

//     // create H thread
//     seller-type = 'H';
//     pthread_create(&tids[0], NULL, sell, &seller-type);

//     // create M threads
//     seller-type = 'M';
//     for (int i = 1; i < 4; i++) {
//         pthread_create(&tids[i], NULL, sell, &seller-type);
//     }

//     // create L threads
//     seller-type = 'L';
//     for (int i = 4; i < 10; i++) {
//         pthread_create(&tids[i], NULL, sell, &seller-type);
//     }

// 	// init pqs with n rand customers
// 	priority_queue<int> *qlist[10];
// 	qlist[0] = &h1;
// 	qlist[1] = &m1;
// 	qlist[2] = &m2;
// 	qlist[3] = &m3;
// 	qlist[4] = &l1;
// 	qlist[5] = &l2;
// 	qlist[6] = &l3;
// 	qlist[7] = &l4;
// 	qlist[8] = &l5;
// 	qlist[9] = &l6;
// 	srand((unsigned) time(0));
// 	for (int i = 0; i < 10; i++) {
// 	    for (int j = 0; j < n; j++) {
// 	        int r = rand() % 59;
// 	        *qlist[i].push(r);
// 	    }
// 	}
	
// 	wakeup_all_seller_threads()

//     // after all threads complete execution, join back into main thread
//     // for (int i = 0 ; i < 10 ; i++) {
//     //     pthread_join(&tids[i], NULL);
//     // }

//     return 0;
// }

