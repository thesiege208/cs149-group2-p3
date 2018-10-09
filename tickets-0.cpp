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


