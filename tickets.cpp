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
int tAway = 0; // turned away customers

pthread_mutex_t mutex;

bool assignLowSeat(string seatId) {
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

bool assignMiddleSeat(string seatId) {
    int middleRow = 4;
    for (int rowOffSet = 0; middleRow - rowOffSet >= 0 && middleRow + rowOffSet < 10; rowOffSet++) {
        int k = -1;
        int i = middleRow + k * rowOffSet;
        for (int j = 0; j < 10; j++){
            if ( seat[i][j] == "" ) {
                seat[i][j] = seatId;
                return true;
            }
        }
        k *= -1;
    }
    return false;
}

bool assignHighSeat(string seatId) {
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

// Assign seat to customer.
// Now we assume the seller is 'L'. But we should
// also handle 'M' and 'H'.
bool assignSeats(string seller, Customer customer) {
    char sellerType = seller[0];
    string assignedSeatId = seller  + '_' + to_string(customer.getCID());
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
            // This customer has already arrived, lock the seat and assign to this customer
            pthread_mutex_lock(&mutex);
            // Assign seats to customers
            cout << "\n@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << '_' << currentCustomer.getCID() << " HAS ARRIVED." << endl;
            stillHasSeat = assignSeats(sellerName, currentCustomer);
            if (stillHasSeat == false) {
                // No more empty seats
                pthread_mutex_unlock(&mutex);
                cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << "SEATS ARE FULL." << endl;
                cQ.pop();
                cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << "_" << currentCustomer.getCID() << " HAS LEFT.\n" << endl;
                tAway++; // increment turned away buyers by 1
                break;
            }
            
            // Unlock the table since this seller already book the seat for the customer
            pthread_mutex_unlock(&mutex);
            
            //keep working for the customer with complete time
            currentTimeStamp = currentTimeStamp + currentCustomer.getCT();
            cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << "SEAT BOOKED BY " << sellerName << "." << endl;
            cQ.pop(); // remove customer who complete purchase
            cout << "@0:" << setfill('0') << setw(2) << currentTimeStamp << " " << sellerName << "_" << currentCustomer.getCID() << " HAS LEFT.\n" << endl;
            sleep(currentCustomer.getCT()); // actually completing
            printTable(); // print table after each ticket sell
        }
    }
    if (!cQ.empty()) {
        tAway += cQ.size(); // add rest of unprocessed buyers
    }
    pthread_cancel(pthread_self());
    return NULL;
}


/* where arg N is the command line option for # of customers per queue */
int main() {
    pthread_t threads[numberOfSellers];
    srand((unsigned) time(0));
    // prompting command line input
    cout << "\nEnter the number of customers per queue (5, 10, or 15): ";
    cin >> N;
    
    pthread_mutex_init(&mutex, NULL);
    
    for (int i = 0; i < numberOfSellers; i++) {
        int sellerId = i;
        pthread_create(&threads[i], NULL, eachSeller,  reinterpret_cast<void*>(sellerId));
    }
    
    for (int i = 0; i < numberOfSellers; i++) {
        pthread_join(threads[i], NULL);
    }
    
    printTable();
    cout << "\n" << tAway << " BUYERS TURNED AWAY." << endl;
    exit(0);
}
