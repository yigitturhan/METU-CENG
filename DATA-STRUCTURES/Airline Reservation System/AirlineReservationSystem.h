#ifndef AIRLINERESERVATIONSYSTEM_H
#define AIRLINERESERVATIONSYSTEM_H

#include "BST.h"
#include "Queue.h"
#include "Passenger.h"
#include "Flight.h"
#include "Ticket.h"

class AirlineReservationSystem {
public:
    AirlineReservationSystem() {}

    void addPassenger(const std::string &firstname, const std::string &lastname);

    Passenger *searchPassenger(const std::string &firstname, const std::string &lastname);

    void addFlight(const std::string &flightCode, const std::string &departureTime, const std::string &arrivalTime, const std::string &departureCity, const std::string &arrivalCity, int economyCapacity, int businessCapacity);

    std::vector<Flight *> searchFlight(const std::string &departureCity, const std::string &arrivalCity);

    void issueTicket(const std::string &firstname, const std::string &lastname, const std::string &flightCode, TicketType ticketType);

    void saveFreeTicketRequest(const std::string &firstname, const std::string &lastname, const std::string &flightCode, TicketType ticketType);

    void executeTheFlight(const std::string &flightCode);

    void print() const;

private:
    void searchflighthelper(BSTNode<Flight> *node, std::string const &departurecity , std::string const &arrivalcity, std::vector<Flight *> &vct){
        if(node == NULL){
            return;
        }
        if(node->data.getDepartureCity() == departurecity && node->data.getArrivalCity() == arrivalcity){
            vct.insert(vct.begin(),&(node->data));
        }
        searchflighthelper(node->right,departurecity,arrivalcity,vct);
        searchflighthelper(node->left,departurecity,arrivalcity,vct);
        
    }
    Passenger* passengersearch(BSTNode<Passenger> *node,const std::string firstname,const std::string lastname){
        if(node == NULL){
            return NULL;
        }
        else if(node->data.getFirstname() == firstname && node->data.getLastname() == lastname){
            return &(node->data);
        }
        else{
            if(passengersearch(node->right,firstname,lastname) != NULL){
                return passengersearch(node->right,firstname,lastname);
            }
            else{
                return passengersearch(node->left,firstname,lastname);
            }
        }
    }
    Flight* flightsearch(BSTNode<Flight> *node,const std::string flightcode){
        if(node == NULL){
            return NULL;
        }
        else if(node->data.getFlightCode() == flightcode){
            return &(node->data);
        }
        else{
            if(flightsearch(node->right,flightcode) != NULL){
                return flightsearch(node->right,flightcode);
            }
            else{
                return flightsearch(node->left,flightcode);
            }
        }
    }

private:
    BST<Passenger> passengers;
    BST<Flight> flights;

    Queue<Ticket> freeTicketRequests;
};

#endif //AIRLINERESERVATIONSYSTEM_H
