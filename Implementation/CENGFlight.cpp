#include "CENGFlight.h"
#include <iostream>

//=======================//
// Implemented Functions //
//=======================//
void CENGFlight::PrintCanNotHalt(const std::string& airportFrom,
                                 const std::string& airportTo,
                                 const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines is not found and cannot be halted"
              << std::endl;
}

void CENGFlight::PrintCanNotResumeFlight(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         const std::string& airlineName)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" via "
              << airlineName
              << " airlines cannot be resumed"
              << std::endl;
}

void CENGFlight::PrintFlightFoundInCache(const std::string& airportFrom,
                                         const std::string& airportTo,
                                         bool isCostWeighted)
{
    std::cout << "A flight path between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  " is found in cache." << std::endl;
}

void CENGFlight::PrintFlightCalculated(const std::string& airportFrom,
                                       const std::string& airportTo,
                                       bool isCostWeighted)
{
    std::cout << "A flight path is calculated between \""
              << airportFrom << "\" and \""
              << airportTo << "\" using "
              << ((isCostWeighted) ? "cost" : "price")
              <<  "." << std::endl;
}

void CENGFlight::PrintPathDontExist(const std::string& airportFrom,
                                    const std::string& airportTo)
{
    std::cout << "A flight path does not exists between \""
              << airportFrom << "\" and \""
              << airportTo <<"\"." << std::endl;
}

void CENGFlight::PrintSisterAirlinesDontCover(const std::string& airportFrom)
{
    std::cout << "Could not able to generate sister airline list from \""
              << airportFrom <<"\"." << std::endl;
}

void CENGFlight::PrintMap()
{
    navigationMap.PrintEntireGraph();
}

void CENGFlight::PrintCache()
{
    lruTable.PrintTable();
}

CENGFlight::CENGFlight(const std::string& flightMapPath)
    : navigationMap(flightMapPath)
{}

//=======================//
//          TODO         //
//=======================//
void CENGFlight::HaltFlight(const std::string& airportFrom,
                            const std::string& airportTo,
                            const std::string& airlineName)
{
    
    
    if(navigationMap.edgeexists(airlineName,airportFrom,airportTo)){
        HaltedFlight halted_flight;
        halted_flight.airportFrom = airportFrom;
        halted_flight.airportTo = airportTo;
        halted_flight.airline = airlineName;
        GraphEdge haltedEdge = navigationMap.edgeweights(airlineName,airportFrom,airportTo);
        halted_flight.w0 = haltedEdge.weight[0];
        halted_flight.w1 = haltedEdge.weight[1];
        haltedFlights.push_back(halted_flight);
        navigationMap.RemoveEdge(airlineName,airportFrom,airportTo);
        
    }
    else{
        PrintCanNotHalt(airportFrom,airportTo,airlineName);
    }

}

    // (Direct Function call)
void CENGFlight::ContinueFlight(const std::string& airportFrom,
                                const std::string& airportTo,
                                const std::string& airlineName)
{
    bool ishalted;
    int index;
    for(int i = 0; i < haltedFlights.size(); i++){
        if(haltedFlights[i].airportFrom == airportFrom && haltedFlights[i].airline == airlineName && haltedFlights[i].airportTo == airportTo){
            ishalted = true;
            index = i;
        }
    }
    if(navigationMap.edgeexists(airlineName, airportFrom, airportTo) || !ishalted){
        PrintCanNotResumeFlight(airportFrom,airportTo,airlineName);
        return;
}
    navigationMap.AddEdge(airlineName,airportFrom,airportTo,haltedFlights[index].w0,haltedFlights[index].w1);

}

void CENGFlight::FindFlight(const std::string& startAirportName,
                            const std::string& endAirportName,
                            float alpha)
{
    /* TODO */
    int startindex, endindex;
    startindex = navigationMap.indices(startAirportName);
    endindex = navigationMap.indices(endAirportName);
    if(startindex == -1 || endindex == -1){
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    std::vector<int> intArray;
    bool isCostWeighted = alpha == 0.0 ? true : false;
    bool isweighted = alpha == 0.0 || alpha == 1.0 ? true: false;

    if(!isweighted){
        if(navigationMap.HeuristicShortestPath(intArray, startAirportName, endAirportName, alpha)) navigationMap.PrintPath(intArray, alpha, true);

        else PrintPathDontExist(startAirportName, endAirportName); 
    }
    else{
        if(lruTable.Find(intArray,startindex,endindex,isCostWeighted,true)) PrintFlightFoundInCache(startAirportName,endAirportName,isCostWeighted);
        else{
            if(navigationMap.HeuristicShortestPath(intArray,startAirportName,endAirportName,alpha)){
                PrintFlightCalculated(startAirportName, endAirportName, isCostWeighted);
                int insertionSuccessful = -1;

                // Attempt to insert into lruTable
                insertionSuccessful = lruTable.Insert(intArray, isCostWeighted);
                if(insertionSuccessful == -1){
                    lruTable.RemoveLRU(1);
                    lruTable.Insert(intArray, isCostWeighted);
                }
                
            }
            else PrintPathDontExist(startAirportName, endAirportName);
        }
        navigationMap.PrintPath(intArray, alpha, true);
    

    }
    return;
}



void CENGFlight::FindSpecificFlight(const std::string& startAirportName,
                                    const std::string& endAirportName,
                                    float alpha,
                                    const std::vector<std::string>& unwantedAirlineNames) const
{
    /* TODO */
    int startindex, endindex;
    startindex = navigationMap.indices(startAirportName);
    endindex = navigationMap.indices(endAirportName);
    if(startindex == -1 || endindex == -1){
        PrintPathDontExist(startAirportName,endAirportName);
        return;
    }
    std::vector<int> intArray;
    bool isCostWeighted = alpha == 0.0 ? true : false;
    bool isweighted = alpha == 0.0 || alpha == 1.0 ? true: false;
    if(!isweighted){
        if(navigationMap.FilteredShortestPath(intArray, startAirportName, endAirportName, alpha,unwantedAirlineNames)) navigationMap.PrintPath(intArray, alpha, true);
        else PrintPathDontExist(startAirportName, endAirportName);
    }
    else{
        if(navigationMap.FilteredShortestPath(intArray,startAirportName,endAirportName,alpha,unwantedAirlineNames)){
            navigationMap.PrintPath(intArray, alpha, true);
            }
        else PrintPathDontExist(startAirportName, endAirportName);
        
    }
    return;
    
}

void CENGFlight::FindSisterAirlines(std::vector<std::string>& airlineNames,
                                      const std::string& startAirlineName,
                                      const std::string& airportName) const
{
     // Check if the airportName is on the navigation map
    int airportIndex = navigationMap.indices(airportName);
    if (airportIndex == -1) {
        PrintSisterAirlinesDontCover(airportName);
        
    }
    std::string airlineName = startAirlineName;
    
    if(!navigationMap.VisitAirportsWithAirlineCheck(airportIndex,airlineName)){
        PrintSisterAirlinesDontCover(airportName);
        
    } 
    
    //visit every node
    std::vector<bool> visited(navigationMap.getVertexCount(), false);
    std::vector<bool> lastVisited(navigationMap.getVertexCount(), false);
    airlineNames.push_back(startAirlineName);
    navigationMap.visit_nodes(airportIndex, airlineNames, visited);
    

    std::vector<std::string> sisterAirlines;
    do{
        lastVisited = visited;
        int j = 0;
        for(int i = 0; i < visited.size(); i++){
            if(visited[i]) j++;
        }
        if (j == visited.size()) break;

        int selectedAirport = navigationMap.FindMaxNonVisitedNeighbors(visited);
        const GraphVertex& selectedVertex = navigationMap.getVertex(selectedAirport);

        std::string selectedAirline;
        bool allAirlinesUtilized = true;

        for (const GraphEdge& edge : selectedVertex.edges) {
            bool found = false;
            for (const std::string& airline : airlineNames) {
                if (airline == edge.name || visited[edge.endVertexIndex]) {
                    found = true;
                    break;
                }
            }

            if (!found) {
                // Found a non-utilized airline
                selectedAirline = edge.name;
                allAirlinesUtilized = false;
                break;
            }
        }
        if (allAirlinesUtilized) {
            // If all airlines from that airport are on the sister airline list, print the error.
            PrintSisterAirlinesDontCover(airportName);
            
            break;
        }
        airlineNames.push_back(selectedAirline);
        navigationMap.visit_nodes(selectedAirport, airlineNames, visited);
        if(visited == lastVisited) PrintSisterAirlinesDontCover(airportName);
    }while(visited != lastVisited);

    airlineNames.erase(airlineNames.begin());
    
    
    return;


}
int CENGFlight::FurthestTransferViaAirline(const std::string& airportName,
                                           const std::string& airlineName) const
{
    if(!navigationMap.airportexists(airportName,airlineName)) return -1;
    
    return navigationMap.MaxDepthViaEdgeName(airportName, airlineName);
     
}