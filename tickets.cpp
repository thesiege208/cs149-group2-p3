/* SJSU Fall 2018
 * CS149 Operating Systems
 * Project 3
 * Group 2
 * Ranying Cai, Tianyun Chen, Roger Kuo, Sijing Xie
 */

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <queue>
#include <string>
#include <map>
#include <pthread.h>
#include <unistd.h>

using namespace std;

#define numberOfSellers 10

class Customer {
    int customerId;
    int arrivalTime;
    int completeTime;

    public:
    Customer(int cId, int aTime, int cTime) {
        customerId = cId;
        arrivalTime = aTime;
        completeTime = cTime;
    }
    
    int getCID() { return customerId; }
    int getAT() { return arrivalTime; }
    int getCT() { return completeTime; }
};

class Compare {
    public:
        bool operator() (Customer a, Customer b) {
            if (a.getAT() > b.getAT()) { return true;}
            return false;
        }
};

string seat[10][10]; /* 2D array containing all 100 seats */
int N; /* command line input deciding # customers per queue */

pthread_cond_t cond = PTHREAD_MUTEX_INITIALIZER;            // main wakeup condition
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;          // main mutex
pthread_mutex_t seatMutex = PTHREAD_MUTEX_INITIALIZER;      // would be necessary if sellers have different # of buyers

bool assignLowSeat(string seatId) {
    pthread_mutex_lock(&seatMutex);         // lock the seat and assign to this customer
    for (int i = 0; i < 10; i++){
            for (int j = 0; j < 10; j++){
                if ( seat[i][j] == "" ) {
                    seat[i][j] = seatId;
                    return true;
                }
            }
        }
    pthread_mutex_unlock(&seatMutex);       // Unlock the table since this seller already book the seat for the customer
    return false;
}

bool assignMiddleSeat(string seatId) {
    int middleRow = 4;
    pthread_mutex_lock(&seatMutex);         // lock the seat and assign to this customer
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
    pthread_mutex_unlock(&seatMutex);       // Unlock the table since this seller already book the seat for the customer
    return false; 
}

bool assignHighSeat(string seatId) {
    pthread_mutex_lock(&seatMutex);         // lock the seat and assign to this customer
    for (int i = 9; i >= 0; i--){
            for (int j = 0; j < 10; j++){
                if ( seat[i][j] == "" ) {
                    seat[i][j] = seatId;
                    return true;
                }
            }
        }
    pthread_mutex_unlock(&seatMutex);       // Unlock the table since this seller already book the seat for the customer
    return false;
}

// Assign seat to customer.
// Now we assume the seller is 'L'. But we should 
// also handle 'M' and 'H'.
bool assignSeats(string seller, Customer customer) {
    char sellerType = seller[0];
    string assignedSeatId = seller  + '_' + to_string(customer.getCID());
    pthread_mutex_lock(&mutex);         
    pthread_cond_wait(&cond, &mutex);   // wait for wakeup cond broadcast
    pthread_mutex_unlock(&mutex);
    if (sellerType == 'L') {
        return assignLowSeat(assignedSeatId);
    } else if (sellerType == 'M') {
        return assignMiddleSeat(assignedSeatId);
    } else { // High level
        return assignHighSeat(assignedSeatId);
    }
}

void printTable() {
     int k = 0;
     cout << "\n---------------------SEATING CHART---------------------\n";
     for (int i = 0; i < 10; i++) {
         for (int j = 0; j < 10; j++) {
             if (seat[i][j].empty() == false) {
               cout << seat[i][j] << "\t";
             } else {
                cout << "-\t\t";
             }
         }
         cout << "\n";
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

priority_queue<Customer, vector<Customer>, Compare> generateRandomCustomerQueue(string sellerName) {
    priority_queue<Customer, vector<Customer>, Compare> customerQueue;
    char sellerType = sellerName[0];
    srand((unsigned) time(0));
    for (int i = 0; i < N; i++) {
        int a = rand() % 59;
        int min;
        int max;
        if (sellerType == 'L') {
            min = 4;
            max = 7;
        } else if (sellerType = 'M') {
            min = 2;
            max = 4;
        } else {
            min = 1;
            max = 2;
        }
        int c = rand() % (max - min + 1) + min;
        Customer cust = Customer(i + 1, a, c);
        customerQueue.push(cust);
    }
    return customerQueue;
}

void *eachSeller(void *sellerId) {
    string sellerName = mapSellerIdToName((long) sellerId);

    // Generate random customer queue for this seller.
    // seller is the name of this seller, for example, "L1", so seller[0] is the type of seller, for example, 'L'.
    char sellerType = sellerName[0];
    priority_queue<Customer, vector<Customer>, Compare> cQ = generateRandomCustomerQueue(sellerName); 

    int currentTimeStamp = 0;
    bool stillHasSeat = true;
    while (currentTimeStamp < 60 && !cQ.empty()) {
        Customer currentCustomer = cQ.top();
        if (currentCustomer.getAT() > currentTimeStamp) {
            // The customer hasn’t arrived yet, wait until they arrive
            int waitTime = currentCustomer.getAT() - currentTimeStamp;
            currentTimeStamp += waitTime; // get the complete time stamp
            sleep(waitTime);
        } else {
            // This customer has already arrived, 
            
            // Assign seats to customers
            cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << '_' << currentCustomer.getCID() << " HAS ARRIVED." << endl;
            stillHasSeat = assignSeats(sellerName, currentCustomer);
            if (stillHasSeat == false) {
                // No more empty seats
                cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << "SEATS ARE FULL." << endl;
                cQ.pop();
                cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << "_" << currentCustomer.getCID() << " HAS LEFT.\n" << endl;
                currentTimeStamp = 100;
                break;
            }
            
            //keep working for the customer with complete time 
            currentTimeStamp = currentTimeStamp + currentCustomer.getCT(); 
            cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << "SEAT BOOKED BY " << sellerName << "_" << currentCustomer.getCID() << "." << endl;
            cQ.pop(); // remove customer who complete purchase
            cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << "_" << currentCustomer.getCID() << " HAS LEFT.\n" << endl;
            sleep(currentCustomer.getCT()); // actually completing
        }
    }
    pthread_cancel(pthread_self());
    return NULL;
}

void wakeup_all_seller_threads() {
    pthread_mutex_lock(&mutex);
    pthread_cond_broadcast(&cond); // wakeup all threads
    pthread_mutex_unlock(&mutex);
}

/* where arg N is the command line option for # of customers per queue */
int main() {
    pthread_t threads[numberOfSellers];
    
    // prompting command line input
    cout << "\nEnter the number of customers per queue (5, 10, or 15): ";
    cin >> N;
    cout << "\n";
    
    pthread_mutex_init(&mutex, NULL);

    for (int i = 0; i < numberOfSellers; i++) {
        int sellerId = i;
        pthread_create(&threads[i], NULL, eachSeller,  reinterpret_cast<void*>(sellerId));
    }

    wakeup_all_seller_threads();
    
    for (int i = 0; i < numberOfSellers; i++) {
        pthread_join(threads[i], NULL);
    }
    
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&seatMutex);
    pthread_cond_destroy(&cond);
    
    printTable();
    exit(0);
}

