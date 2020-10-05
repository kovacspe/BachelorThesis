#ifndef _TS
#define _TS

#include "Matrices.cpp"
#include "SATravelingSalesman.cpp"
#include<map>

// reprezentuje zoznam zakázaných riešení
class TabuList {
public:
	TabuList(size_t maxTabu) :maxTabuSize{ maxTabu } {}

	// zistí, èi riešenie je v tabu liste
	bool isInTabuList(const Solution & solution) const
	{
		for (int j = 0;j<tabuList.size();j++)
		{
			for (int i = 0;i<tabuList[j]->getPath().size();i++)
			{
				if (tabuList[j]->getPath().at(i) != solution.getPath().at(i)) break;
				if ((tabuList[j]->getPath().at(i) == solution.getPath().at(i)) && (i == tabuList[j]->getPath().size() - 1)) return true;

			}
		}
		return false;
	}

	//pridá riešenie do tabulistu
	void AddToTabuList(SolPtr solution)
	{
		tabuList.push_back(std::move(solution));
		if (tabuList.size() > maxTabuSize) {
			
			tabuList.erase(tabuList.begin());
		}
	}

private:
	vector<SolPtr> tabuList;
	size_t maxTabuSize;
};


// reprezentuje zoznam zakázaných riešení
class NewTabuList {
public:
	NewTabuList(size_t maxTabu) :maxTabuSize{ maxTabu }, validationNumber{0} {}

	// zistí, èi riešenie je v tabu liste
	bool isInTabuList(double cost,const Solution & solution, size_t first, size_t second) const
	{
		auto SolutionsWithThatCost= tabuList.find(ceil(cost));
		if (SolutionsWithThatCost == tabuList.end()) return false;
		
		for (auto && sol: (*SolutionsWithThatCost).second)
		{
			if (sol.first < validationNumber - maxTabuSize) continue;
			auto path =  sol.second->getPath();
			for (size_t i = 0;i<path.size();++i)
			{
				if (i == first)
				{
					if (path.at(i) != solution.getPath().at(second)) break;
					if ((path.at(i) == solution.getPath().at(second)) && (i == path.size() - 1)) {  return true; }
				}
				else if (i == second)
				{
					if (path.at(i) != solution.getPath().at(first)) break;
					if ((path.at(i) == solution.getPath().at(first)) && (i == path.size() - 1)) { return true; }
				}
				else
				{
					if (path.at(i) != solution.getPath().at(i)) break;
					if ((path.at(i) == solution.getPath().at(i)) && (i == path.size() - 1)) { ; return true; }
				
				}

			}
		}
		return false;
	}

	//pridá riešenie do tabulistu
	void AddToTabuList(SolPtr solution,double cost)
	{
		tabuList[ceil(cost)].push_back(std::pair<size_t,SolPtr>(validationNumber,std::move(solution)));
		++validationNumber;
	}

private:
	map<double,vector<std::pair<size_t,SolPtr>>> tabuList;
	size_t maxTabuSize;
	size_t validationNumber;
};

// vyberie najlepšie susedné riešenie
static SolPtr getBestNeighbour(const Solution & currTour, const LowerMatrix<double> & distances, const TabuList & tabuList)
{
	SolPtr bestSoFar = currTour.createNeighbour(0,1);
	auto bestCost = bestSoFar->getCost(distances);
	for (size_t i = 2; i<distances.size(); ++i)
		for (size_t j = 0; j<i; ++j)
		{

			SolPtr newTour = currTour.createNeighbour(j,i);
			if ( (newTour->getCost(distances)<bestCost) && !tabuList.isInTabuList(*newTour.get()) )
				bestSoFar = make_unique<Solution>(*newTour.get());
			bestCost = bestSoFar->getCost(distances);

		}
	return bestSoFar;
}



static SolPtr getBestNeighbour2(const Solution & currTour, const LowerMatrix<double> & distances, const NewTabuList & tabuList)
{
	auto baseCost = currTour.getCost(distances);
	size_t bestFirst=0,
		bestSecond=1;
	auto currPath = currTour.getPath();
	double bestIncrease = countDelta(currTour, distances, bestFirst, bestSecond);


	for (size_t second = 2; second<distances.size(); ++second)
		for (size_t first = 0; first<second; ++first)
		{
			auto currInc = countDelta(currTour, distances, first, second);
			//cout << currInc << std::endl;
			if (bestIncrease > currInc && !tabuList.isInTabuList(baseCost+currInc,currTour,first,second))
			{
				bestIncrease = currInc;
				bestFirst = first;
				bestSecond = second;
			}

			

		}
	auto ret=  currTour.createNeighbour(bestFirst,bestSecond);
	// cout << "SWAP\t"<<bestFirst<<"\t"<<bestSecond << endl;
	return std::move(ret);
}

//Spustí tabu search s danými parametrami
static SolPtr TabuSearch(size_t numberOfIterations,size_t maxTabuSize,const LowerMatrix<double> & distances)
{
	SolPtr bestSoFar = generateRandomTour(distances.size());
	double bestCost = bestSoFar->getCost(distances);
	cout << bestCost;
	SolPtr currTour = make_unique<Solution>(*bestSoFar);
	NewTabuList tabuList(maxTabuSize);
	for (int i = 0;i<numberOfIterations;i++)
	{
		
		currTour = getBestNeighbour2(*currTour.get(),distances,tabuList);
		auto cost = currTour->getCost(distances);
		//cout << i << "/" << numberOfIterations << ", COST:\t" << cost << std::endl;
		if (bestCost > cost)
		{
			
			cout << "BETTER" << endl;
			bestSoFar = make_unique<Solution>(*currTour.get());
			bestCost = cost;

		}

		tabuList.AddToTabuList(make_unique<Solution>(*currTour.get()),cost);


	}

	return std::move(bestSoFar);
}

#endif