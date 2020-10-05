#ifndef _ACO
#define _ACO


#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <fstream>
#include <sstream>
#include <memory>
#include <vector>
#include "Matrices.cpp"


using namespace std;


using ACOSolPtr = unique_ptr<Solution>;
//PoËÌta pravdepododbnosù pre jednu hranu
static double probability(size_t currVertex, int nextVertex,double alpha,double beta,const LowerMatrix<double> & distances, const LowerMatrix<double> & pheromones)
{
	//cout << pow(pheromones.at(currVertex, nextVertex), alpha)*pow((1 / distances.at(currVertex, nextVertex)), beta);
	return pow(pheromones.at(currVertex,nextVertex), alpha)*pow((1 / distances.at(currVertex,nextVertex)), beta);
}

// poËÌta s˙Ëet vöetk˝ch pravdepodobnostÌ z vrchola do zatiaæ nenavötÌven˝ch vrcholov
static double countProbSum(size_t currVertex, const vector<size_t> & notVisited, double alpha, double beta, const LowerMatrix<double> & distances, const LowerMatrix<double> & pheromones,vector<double> & propabilities)
{
	double sum = 0;
	for (auto && v : notVisited)
	{

		auto x= probability(currVertex, v, alpha, beta, distances, pheromones);
		sum += x;
		propabilities.push_back(x);

	}
	return sum;
}

//vyberie na z·klade pravdepodobnostÌ vrchol, kam sa mravec najbliûöie posunie
static size_t chooseVertex(vector<size_t> & notVisited,size_t currVertex,double alpha, double beta, const LowerMatrix<double> & distances,const LowerMatrix<double> & pheromones)
{

	double p = rand() / ((double)RAND_MAX);

	std::size_t vertex = 0;
	vector<double> propabilities;
	double probSum = countProbSum(currVertex, notVisited, alpha, beta, distances, pheromones,propabilities);
	double sum = probability(currVertex, notVisited[vertex], alpha, beta, distances, pheromones);


	while (sum<(p*0.9999*probSum))
	{
		vertex++;
		sum += propabilities[vertex];



	}
	size_t chosen = notVisited[vertex];
	notVisited.at(vertex)=notVisited.back();
	notVisited.pop_back();
	return chosen;
}

// skonötruuje rieöenie pre jednÈho mravca
static ACOSolPtr constructSolution(double alpha, double beta, const LowerMatrix<double> & distances,const LowerMatrix<double> & pheromones)
{
	ACOSolPtr sol= make_unique<Solution>();
	vector<size_t> notVisited;
	for (auto && x : distances.vertices())
	{
		if (x != 0) {
			notVisited.push_back(x);
		}
	}

	sol->AddToPath(0);
	while(sol->getPath().size()<distances.size())
	{
		sol->AddToPath(chooseVertex(notVisited, sol->currentVertex(), alpha, beta, distances, pheromones));
	}

	return sol;

}

// Prid· feromÛn na hranu podæa danÈho rieöenia
static void UpdatePheromone(const Solution & solution,double cost,LowerMatrix<double> & pheromones) 
{
	cost = 1 / cost;
	auto path = solution.getPath();
	for (size_t i = 0; i < path.size(); i++)
	{
		pheromones.at(path[i], path[(i + 1) % path.size()]) += cost;
	}
}

//vyparÌ feromÛny
static void Evaporate(LowerMatrix<double> & pheromones, double evaporation)
{
	for (auto && p : pheromones)
	{
		p *= evaporation;
	}
}


// SpustÌ AntColony s dan˝mi paramatrami a vr·ti najlepöie rieöenie akÈ naöiel
static ACOSolPtr AntColony(size_t numberOfIterations, size_t numberOfAnts, double alpha, double beta, double evaporation, const LowerMatrix<double> & distances)
{
	ACOSolPtr bestSoFar=NULL;
	LowerMatrix<double> pheromones(distances.size());
	double bestCost = 0;
	for (auto && p: pheromones)
	{
		p = 1;
	}
	

	for (size_t i = 0; i < numberOfIterations; ++i)
	{
		vector<ACOSolPtr> solutions;
		for (size_t j = 0; j < numberOfAnts; ++j)
		{
			solutions.push_back(constructSolution(alpha, beta, distances,pheromones));
		}
		
		for (auto && s : solutions)
		{
			double cost = s->getCost(distances);
			UpdatePheromone(*s.get(), cost,pheromones);
			if (bestSoFar == NULL || bestCost>cost)
			{
				bestSoFar = std::move(s);
				bestCost = cost;
			}

		}
		Evaporate(pheromones, evaporation);
	}
	return std::move(bestSoFar);
}

#endif