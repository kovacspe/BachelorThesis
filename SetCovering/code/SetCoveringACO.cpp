
#ifndef _ACO
#include "SCProblem.cpp"
#define _ACO






// pridava feromony z jedneho riesenia
static void AddPheromones(vector<double> & pheromones, Solution & sol,const Problem & problem) 
{
	auto toAdd = 1/sol.getCost(problem.getCosts());
	for (auto && set : sol)
	{
		pheromones[set] += toAdd;
	}
}

// vyparuje feromony
static void Evaporate(double EvaporationCoef, vector<double> & pheromones)
{
	for (auto && p : pheromones)
	{
		p *= EvaporationCoef;
	}
}


// pocita hodnotu heuristickeho koeficientu
static double getHeuristicCoef(size_t setID, const Problem & problem, const Solution & solution)
{
	size_t currTarget = 0;
	 const vector<size_t> & elems = problem.getElementsInSet(setID);
	for (auto && e : elems)
	{
		if (!solution.isElementCovered(e))
		{
			++currTarget;
		}
	}
	currTarget /= problem.getCost(setID);
	return currTarget;
	
}

// pocita tau pre vzorec na rozhodovanie 
static double tau(size_t setID, const vector<double> & pheromones)
{
	return pheromones[setID];
}

// pocita hodnotu pre nasledujuci krok, cim vacsia hodnota, tym pravdepodobvnejsie
static double probability(size_t setID, const vector<double> & pheromones, double BETA,const Solution & solution, const Problem & problem)
{
	
	return tau(setID, pheromones)*pow((getHeuristicCoef(setID,problem,solution)), BETA);

}

// scita hodnotu "pravdepodobnosti" pre vsetky moûnosti 
static double countProbSum(const vector<size_t> & possibleSets, const vector<double> & pheromones, double BETA,const Solution & solution, const Problem & problem)
{
	double sum = 0;
	for (size_t i = 0; i<possibleSets.size(); i++)
	{
		sum += probability(possibleSets[i], pheromones, BETA,solution,problem);
	}
	return sum;
}


// vyberie podla pravdepodobnosti nasledujuci krok
static std::size_t chooseSet(const vector<size_t> & possibleSets,const vector<double> & pheromones, const Problem & problem, const Solution & solution, double BETA)
{

	double p = rand() / ((double)RAND_MAX);

	size_t setIndex = 0;
	double probSum = countProbSum(possibleSets, pheromones,BETA,solution,problem);
	double sum = probability(possibleSets.front(), pheromones,BETA,solution,problem) ;

	probSum *= p*0.9999; // suma mensich rieseni
	while (sum<probSum)
	{
		setIndex++;
		sum += probability(possibleSets[setIndex], pheromones, BETA,solution,problem) ;

	}

	return possibleSets[setIndex];
}


// konötruuje rieöenie jednÈho mravca
static Solution constructSolution(Problem & problem, vector<double> & pheromones, double beta)
{
	Solution sol(problem.getNumOfSets(), problem.getNumOfElements());
	while (!sol.isComplete())
	{
		size_t notCoveredElement = sol.getUncoveredElement();
		const vector<size_t> &  possibleSets = problem.getSetsCoveringElement(notCoveredElement);
		size_t chosenSet = chooseSet(possibleSets, pheromones, problem,sol, beta);
		sol.AddSetToSolution(problem,chosenSet);
	}
	return sol;
}


// simuluje AntColony a vr·ti najlepöie rieöenie, ktorÈ sa heuristike podarilo dosiahnuù
static Solution AntColony(size_t iterations, size_t ants, double beta,double evaporation, Problem & problem)
{
	vector<double> pheromones(problem.getNumOfSets(),1);
	Solution bestSoFar(problem.getNumOfSets(),problem.getNumOfElements());
	
	for (size_t i = 0; i < iterations; ++i)
	{
		vector<Solution> solutions;
		for (size_t j = 0; j < ants; ++j)
		{
			solutions.push_back(constructSolution(problem, pheromones, beta));
		}
		for (size_t j = 0; j < ants; j++)
		{
			AddPheromones(pheromones, solutions[j],problem);
			// aktualizuje najlepsie videne riesenie
			if (!bestSoFar.isComplete() || bestSoFar.getCost(problem.getCosts()) > solutions[j].getCost(problem.getCosts()))
			{
				bestSoFar = solutions[j];
			}
		}
		Evaporate(evaporation, pheromones);
	}
	return bestSoFar;
}

#endif // !_ACO