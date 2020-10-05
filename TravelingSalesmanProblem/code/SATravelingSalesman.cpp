#ifndef _SA
#define _SA

#include<memory>
#define _USE_MATH_DEFINES
#include <utility>
#include <algorithm>
#include <math.h>
#include <iostream>
#include <cmath>
#include "Matrices.cpp"


using SolPtr = unique_ptr<Solution>;

// vytvorí poèiatoèné riešenie
static SolPtr generateRandomTour(size_t numberOfVertices)
{
	SolPtr tour = make_unique<Solution>();
	for (size_t i = 0; i<numberOfVertices; ++i)
		tour->AddToPath(i);

	return tour;
}

// spustí simulated annealing s danými parametrami
static SolPtr SimulatedAnnealing(double temperature, double coolingCoef, LowerMatrix<double> & distances)
{
	SolPtr bestSoFar = generateRandomTour(distances.size());
	double bestCost=bestSoFar->getCost(distances);
	SolPtr currTour = make_unique<Solution>(*bestSoFar);
	auto newCost = bestCost;
	while (temperature >= 1)
	{
		size_t first = rand() % distances.size();
		size_t second = rand() % distances.size();
		if (first > second) swap(first, second);
		double delta = countDelta(*currTour, distances, first, second);
		//SolPtr newTour = currTour->createNeighbour(first,second);
		//if (delta != (currTour->getCost(distances) - newTour->getCost(distances))) cout << "POzOOOOOR"<< currTour->getCost(distances) - newTour->getCost(distances) <<"\t"<<delta << endl;
		double probability = pow(M_E, -(delta / temperature)); //urèí pravdepodobnos prijatia riešenia
		if ((rand() / (double)RAND_MAX) <= probability)
		{
		
			currTour = currTour->createNeighbour(first,second);
			newCost += delta;
			if (bestCost > newCost) { bestSoFar = make_unique<Solution>(*currTour); bestCost = newCost; }
		}
		
		temperature *=coolingCoef;
	}

	return std::move(bestSoFar);
}

#endif