#ifndef _Problem
#define _Problem


#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include "Matrices.cpp"
#include <string>


// popisuje instaci aktualneho problemu
class Problem {
public:
	size_t maxCost;
	Problem(size_t elements, size_t sets) : SetsCoveringElements(elements), ElementsInSets(sets) {

	}
	// vrati cenu danej mnoziny
	size_t getCost(size_t setID) const { return costs[setID]; }
	//vrati vsetky mnoziny ktore pokryvaju zadany prvok
	vector<vector<size_t>>::const_reference getSetsCoveringElement(size_t elementID) const{ return SetsCoveringElements[elementID]; }
	// vr�ti v�etky prvky, ktor� sa nach�dzaj� v danej mno�ine
	vector<vector<size_t>>::const_reference getElementsInSet(size_t setID) const { return ElementsInSets[setID]; }
	// vr�ti const referenciu na v�etky ceny
	const vector<size_t> & getCosts() const { return costs; }

	// vr�ti objekt, cez ktor� je mo�n� iterova�. Iter�to prech�dza v�etky platn� ID mno��n
	MatrixView getSets() const
	{
		return MatrixView(ElementsInSets.size());
	}
	// vr�ti objekt, cez ktor� je mo�n� iterova�. Iter�to prech�dza v�etky platn� ID prvkov
	MatrixView getElements() const
	{
		return MatrixView(SetsCoveringElements.size());
	}
	
	// vr�ti po�et mno��n v probl�me
	size_t getNumOfSets() const {
		return ElementsInSets.size();
	}

	// vr�ti po�et prvkov v probl�me
	size_t getNumOfElements() const {
		return SetsCoveringElements.size();
	}

	// naparsuje ceny zo vstupu, ktory je nastaveny na zaciatok cien, metoda predpoklada, ze dalej v subore je tolko cisel ako je pocet mnozin a su to prislusne ceny tychto mnozin
	void parseCosts(std::ifstream & input)
	{
		size_t num;
		maxCost = 0;
		for (auto && s:getSets())
		{
			input >> num;
			costs.push_back(num);
			maxCost += num;

		}
	}
	//Vlozi do problemu vztah prvok patri do mnoziny.
	void AddRelationSetElement(size_t element, size_t set)
	{
		ElementsInSets[set].push_back(element);
		SetsCoveringElements[element].push_back(set);
	}

private:
	vector<vector<size_t>> SetsCoveringElements;
	vector<vector<size_t>> ElementsInSets;
	vector<size_t> costs;
	
};


// Solution popisujuca riesenie problemu
class Solution {
public:
	Solution(const Solution & m) = default;
	Solution(Solution && b) = default;
	Solution &operator=(const Solution &) = default;
	Solution &operator=(Solution && s) = default;
	Solution(size_t numOfSets, size_t numOfElements) : ElementsCovered(numOfElements, false), numOfuncoveredElements{ numOfElements }, chosenSetsBool(numOfSets, false) {}

	// vr�ti cenu rie�enia
	size_t getCost(const vector<size_t> & costs) const {
		size_t sum=0;
		for (size_t i = 0; i < chosenSets.size(); i++)
		{
			 sum += costs[chosenSets[i]];
		}
		return sum;
	}
	
	
	

	// vracia iter�tor ukazuj�ci na za�iatok zoznamu vybran�ch mno��n 
	vector<size_t>::iterator begin() { return chosenSets.begin(); }
	// vracia const iter�tor ukazuj�ci na za�iatok zoznamu vybran�ch mno��n 
	vector<size_t>::const_iterator cbegin() const{ return chosenSets.cbegin(); }
	// vracia iter�tor ukazuj�ci na koniec zoznamu vybran�ch mno��n 
	vector<size_t>::iterator end() { return chosenSets.end(); }
	// vracia const iter�tor ukazuj�ci na koniec zoznamu vybran�ch mno��n 
	vector<size_t>::const_iterator cend() const { return chosenSets.cend(); }

	// overuje, ci je riesenie korektne. Teda �i u� ka�d� prvok je pokryt� apso� jednou mno�inou
	bool isComplete() const { return numOfuncoveredElements == 0; }

	// vr�ti n�hodn� prvok, ktor� e�te nie je pokryt�
	size_t getUncoveredElement() const
	{
		size_t counter = 0;
		size_t chosen = 1 + (rand() % numOfuncoveredElements);
		//if (ElementsCovered[0])
		size_t i = 0;
		while (true)
		{
			if (ElementsCovered[i] == false) ++counter;
			if (counter == chosen) return i;
			++i;
		}
		return i;
	}

	//prid� mno�inu do rie�enia
	void AddSetToSolution(const Problem & problem, size_t setID)
	{
		AddSet(setID);
		auto elems = problem.getElementsInSet(setID);

		// Odstranuje pokryte prvky zo zonamu nepokrytych
		size_t positionInUncoveredSets = 0;
		for (size_t i = 0; i < elems.size();++i)
		{
			if (ElementsCovered[elems[i]] == false)
			{
				--numOfuncoveredElements;
				ElementsCovered[elems[i]] = true;
			}
		}

	}

	// vr�ti, �i dan� prvok u� je pokryt�
	bool isElementCovered(size_t elementID) const
	{
		return ElementsCovered[elementID];
	}

	// vr�ti �i sa zadan� mno�ina  nach�dza v aktu�lnom pokryt�(rie�en�)
	bool isSetInCover(size_t setID) const { return chosenSetsBool[setID]; }

	
	// odstr�ni n�hodn� mno�inu z rie�enia
	size_t RemoveRandomFromSolution(const Problem & problem)
	{
		size_t pos = rand() % chosenSets.size();
		return RemoveNthSetFromSolution(problem, pos);
	}

	size_t RemoveSetFromSolution(const Problem & problem,size_t set)
	{
		size_t pos = find(chosenSets.begin(), chosenSets.end(), set)-chosenSets.begin();
		 
		return RemoveNthSetFromSolution(problem, pos);
	}

	size_t size() const
	{
		return chosenSets.size();
	}

	size_t RemoveNthSetFromSolution(const Problem & problem, size_t pos)
	{
		size_t setToRemove = chosenSets[pos];
		chosenSetsBool[setToRemove] = false;
		chosenSets.erase(chosenSets.begin()+pos);
		auto elemsInSet = problem.getElementsInSet(setToRemove);
		//kontroluje ktore vrcholy ostali nepokrte
		for (auto && el : elemsInSet)
		{
			auto sets = problem.getSetsCoveringElement(el);
			bool isCovered = false;
			for (auto && s : sets)
			{
				if (isSetInCover(s)) { isCovered = true; break; }

			}
			if (!isCovered) { ElementsCovered[el] = false; ++numOfuncoveredElements; }
		}
		
		return setToRemove;
	}

	size_t getNumOfUncoveredElements()
	{
		return numOfuncoveredElements;
	}
private:
	// prid� mno�inu do zoznamov vybratych
	void AddSet(size_t setID) { chosenSets.push_back(setID);  chosenSetsBool[setID] = true; }

	vector<size_t> chosenSets;
	size_t numOfuncoveredElements;
	vector<bool> ElementsCovered;
	vector<bool> chosenSetsBool;
};

// Parsuje popisy problemov do triedy Problem
static Problem ParseProblem(string fileName)
{
	std::cout << "Parsing problem: " << fileName<<endl;
	size_t elements, sets;
	size_t set;
	std::string line;
	bool haveDimension = false;
	std::ifstream input = ifstream(fileName);
	input >> elements >> sets;
	Problem problem(elements, sets);
	problem.parseCosts(input);
	for (size_t el = 0; el < elements; ++el)
	{
		size_t numOfCoveringSets;
		input >> numOfCoveringSets;
		for (size_t j = 0; j < numOfCoveringSets; ++j)
		{
			input >> set;
			problem.AddRelationSetElement(el, set-1);
		}
	}
	input.close();
	return problem;

}
#endif