#include "AirlineReservationSystem.h"

void AirlineReservationSystem::addPassenger(const std::string &firstname, const std::string &lastname) {
    Passenger ps(firstname,lastname);
    if(!passengers.search(ps)){
        passengers.insert(ps);
    }
}

Passenger *AirlineReservationSystem::searchPassenger(const std::string &firstname, const std::string &lastname) {
    Passenger ps(firstname,lastname);
    if(passengers.search(ps)){
        return &(passengers.search(ps)->data);
    }
    return NULL;
}

void AirlineReservationSystem::addFlight(const std::string &flightCode, const std::string &departureTime, const std::string &arrivalTime, const std::string &departureCity, const std::string &arrivalCity, int economyCapacity, int businessCapacity) {
    Flight ff(flightCode,departureTime,arrivalTime,departureCity,arrivalCity,economyCapacity,businessCapacity);
    if(!flights.search(ff)){
        flights.insert(ff);
    }
}

std::vector<Flight *> AirlineReservationSystem::searchFlight(const std::string &departureCity, const std::string &arrivalCity) {
    BSTNode<Flight> *root = flights.getRoot();
    std::vector<Flight *> vct;
    searchflighthelper(root,departureCity,arrivalCity,vct);
    return vct;
}

void AirlineReservationSystem::issueTicket(const std::string &firstname, const std::string &lastname, const std::string &flightCode, TicketType ticketType) {
    Passenger *pas = passengersearch(passengers.getRoot(),firstname,lastname);
    Flight *fli = flightsearch(flights.getRoot(),flightCode);
    if(fli && pas){
        Ticket tic(pas,fli,ticketType);
        fli->addTicket(tic);
    }
}

void AirlineReservationSystem::saveFreeTicketRequest(const std::string &firstname, const std::string &lastname, const std::string &flightCode, TicketType ticketType) {
    Passenger *pas = passengersearch(passengers.getRoot(),firstname,lastname);
    Flight *fli = flightsearch(flights.getRoot(),flightCode);
    if(fli && pas){
        Ticket tic(pas,fli,ticketType);
        freeTicketRequests.enqueue(tic);
    }
}

void AirlineReservationSystem::executeTheFlight(const std::string &flightCode) {
    Flight *fli = flightsearch(flights.getRoot(),flightCode);
    if(fli){
        for(int i=1;i<=freeTicketRequests.size();i++){
            Ticket t = freeTicketRequests.dequeue();
            if(t.getFlight() == fli){
                if(!fli->addTicket(t)){
                    freeTicketRequests.enqueue(t);
                }
            }
            else{
                freeTicketRequests.enqueue(t);
            }
        }
        fli->setCompleted(true);
    }
}

void AirlineReservationSystem::print() const {
    std::cout << "# Printing the airline reservation system ..." << std::endl;

    std::cout << "# Passengers:" << std::endl;
    passengers.print(inorder);

    std::cout << "# Flights:" << std::endl;
    flights.print(inorder);

    std::cout << "# Free ticket requests:" << std::endl;
    freeTicketRequests.print();

    std::cout << "# Printing is done." << std::endl;
}
