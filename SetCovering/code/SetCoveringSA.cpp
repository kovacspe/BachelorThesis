#ifndef _SA
#include "SCProblem.cpp"
#include <memory>
#define _SA

using SolPtr = unique_ptr<Solution>;
class TabuList {
public:
	TabuList(size_t maxTabu) :maxTabuSize{ maxTabu } {}

	// zistí, èi riešenie je v tabu liste
	bool isInTabuList(const Solution & solution, const Problem & problem) const
	{
		for (int j = 0;j<tabuList.size();++j)
		{
			bool isEqual = true;
			for (size_t i = 0; i < problem.getNumOfSets(); ++i)
			{
				
				if (tabuList.at(j)->isSetInCover(i) != solution.isSetInCover(i))
				{
					isEqual = false;
					break;
				}
			}
			if (isEqual) return true;
		}
		return false;
	}

	//pridá riešenie do tabulistu
	void AddToTabuList(const Solution & solution)
	{
		tabuList.push_back(make_unique<Solution>(solution));
		if (tabuList.size() > maxTabuSize) {

			tabuList.erase(tabuList.begin());
		}
	}

private:
	vector<SolPtr> tabuList;
	size_t maxTabuSize;
};


// hladný algoritmus
static Solution Greedy(const Problem & problem)
{
	Solution sol(problem.getNumOfSets(), problem.getNumOfElements());
	
	while (!sol.isComplete())
	{
		size_t bestSet=0;
		double bestSetTarget=0;
		// h¾adá množinu s najlepším prírastkom prvkov vzh¾adom na cenu
		for (size_t i = 0; i <problem.getNumOfSets(); ++i)
		{
			if (!sol.isSetInCover(i)) // iba množiny ktoré ešte niesú v pokrytí
			{
				double currTarget = 0;
				auto elems = problem.getElementsInSet(i);
				for (auto && e : elems)
				{
					if (!sol.isElementCovered(e)) // pre každú množinu zapoèítaj prvky, ktoré by pokryla a ešte nie sú pokryté
					{
						++currTarget;
					}
				}
				currTarget /= problem.getCost(i);
				if (currTarget > bestSetTarget)
				{
					bestSet = i;
					bestSetTarget = currTarget;
				}
			}
		}
		sol.AddSetToSolution(problem,bestSet);
	}
	return sol;
}

// skonštruuje nové susedné riešenie
static Solution constructNeighbourSolution(const Problem & problem, const Solution & solution)
{
	Solution newSol = solution;
	newSol.RemoveRandomFromSolution(problem);

	while (!newSol.isComplete())
	{
		size_t elem = newSol.getUncoveredElement();
		const vector<size_t> & sets = problem.getSetsCoveringElement(elem);
		newSol.AddSetToSolution(problem, sets[rand()%sets.size()]);
	}
	
	return newSol;
	
}

static Solution constructNeighbourSolution2(const Problem & problem, const Solution & solution)
{
	size_t set = rand() % problem.getNumOfSets();
	Solution newSol = solution;
	if (newSol.isSetInCover(set))
	{
		newSol.RemoveSetFromSolution(problem, set);
	}
	else {
		newSol.AddSetToSolution(problem, set);
	}

	return newSol;

}


// skonštruuje nové susedné riešenie
static Solution findBestNeighbourSolution(const Problem & problem, Solution & solution,const TabuList & tabu)
{
	Solution bestSolution(problem.getNumOfSets(), problem.getNumOfElements());
	size_t bestCost = bestSolution.getCost(problem.getCosts()) + problem.maxCost*bestSolution.getNumOfUncoveredElements();
	for (size_t set:problem.getSets())
	{
		
		if (solution.isSetInCover(set))
		{
			solution.RemoveSetFromSolution(problem,set);
		}
		else {
			solution.AddSetToSolution(problem,set);
		}
		if (!tabu.isInTabuList(solution, problem) &&
			(bestCost > (solution.getCost(problem.getCosts()) + problem.maxCost*solution.getNumOfUncoveredElements()))
			)
		{
			bestSolution = solution;
			bestCost = bestSolution.getCost(problem.getCosts()) + problem.maxCost*bestSolution.getNumOfUncoveredElements();
		}
		if (solution.isSetInCover(set))
		{
			solution.RemoveSetFromSolution(problem, set);
		}
		else {
			solution.AddSetToSolution(problem,set);
		}


	}
	return bestSolution;

}

// pustí Simulated Annealing na danom probléme. Vráti najlepšie riešenie aké sa podarilo dosiahnu.
static Solution SimulatedAnnealing(double coolingCoef, double temperature, const Problem & problem, const Solution & initial)
{
	
	Solution currSolution = initial;
	Solution bestSoFar = currSolution;

	while (temperature > 0.5) {
		//Solution candidate = constructNeighbourSolution(problem, currSolution);
		Solution candidate = constructNeighbourSolution2(problem, currSolution);

		//double candidateCost = candidate.getCost(problem.getCosts());
		//double delta = candidateCost - currSolution.getCost(problem.getCosts());
		
		double candidateCost = candidate.getCost(problem.getCosts())+problem.maxCost*candidate.getNumOfUncoveredElements();
		double delta = candidateCost - currSolution.getCost(problem.getCosts()) - problem.maxCost*currSolution.getNumOfUncoveredElements();
		
		double probability = (pow(2.71828, -(delta / temperature)));
		if ((rand() / (double)RAND_MAX) <= probability)
		{
			//cout << "accept new:" << candidateCost <<" Current: " <<currSolution.getCost(problem.getCosts())<<std::endl;
			currSolution = candidate;
			if (candidate.isComplete() && candidateCost< bestSoFar.getCost(problem.getCosts()))
			{
				
				bestSoFar = candidate;
			}
		}
	
		temperature *= coolingCoef;
	}

	return bestSoFar;
}

// pustí Tabu Search na danom probléme. Vráti najlepšie riešenie aké sa podarilo dosiahnu.
static Solution TabuSearch(size_t numberOfIterations, size_t sizeOfTabuList, const Problem & problem, const Solution & initial)
{

	Solution candidate = initial;
	Solution bestSoFar = initial;
	TabuList tabu(sizeOfTabuList);
	for (size_t i = 0; i < numberOfIterations; i++)
	{
		//cout << "iteration" << std::endl;
		candidate = findBestNeighbourSolution(problem, candidate, tabu);
		tabu.AddToTabuList(candidate);
		double candidateCost = candidate.getCost(problem.getCosts());
		

			//cout << "accept new:" << candidateCost <<" Current: " <<currSolution.getCost(problem.getCosts())<<std::endl;
			if (candidate.isComplete() && candidateCost < bestSoFar.getCost(problem.getCosts()))
			{
				
				cout << "niesom kripel" << std::endl;
				if (!candidate.isComplete()) cout << " ... ale kokod";
				cout<< std::endl;
				bestSoFar = candidate;
			}
		
	}

	return bestSoFar;
}


#endif
