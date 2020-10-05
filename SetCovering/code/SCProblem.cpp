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
	// vráti všetky prvky, ktoré sa nachádzajú v danej množine
	vector<vector<size_t>>::const_reference getElementsInSet(size_t setID) const { return ElementsInSets[setID]; }
	// vráti const referenciu na všetky ceny
	const vector<size_t> & getCosts() const { return costs; }

	// vráti objekt, cez ktoré je možné iterova. Iteráto prechádza všetky platné ID množín
	MatrixView getSets() const
	{
		return MatrixView(ElementsInSets.size());
	}
	// vráti objekt, cez ktoré je možné iterova. Iteráto prechádza všetky platné ID prvkov
	MatrixView getElements() const
	{
		return MatrixView(SetsCoveringElements.size());
	}
	
	// vráti poèet množín v probléme
	size_t getNumOfSets() const {
		return ElementsInSets.size();
	}

	// vráti poèet prvkov v probléme
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

	// vráti cenu riešenia
	size_t getCost(const vector<size_t> & costs) const {
		size_t sum=0;
		for (size_t i = 0; i < chosenSets.size(); i++)
		{
			 sum += costs[chosenSets[i]];
		}
		return sum;
	}
	
	
	

	// vracia iterátor ukazujúci na zaèiatok zoznamu vybraných množín 
	vector<size_t>::iterator begin() { return chosenSets.begin(); }
	// vracia const iterátor ukazujúci na zaèiatok zoznamu vybraných množín 
	vector<size_t>::const_iterator cbegin() const{ return chosenSets.cbegin(); }
	// vracia iterátor ukazujúci na koniec zoznamu vybraných množín 
	vector<size_t>::iterator end() { return chosenSets.end(); }
	// vracia const iterátor ukazujúci na koniec zoznamu vybraných množín 
	vector<size_t>::const_iterator cend() const { return chosenSets.cend(); }

	// overuje, ci je riesenie korektne. Teda èi už každý prvok je pokrytý apsoò jednou množinou
	bool isComplete() const { return numOfuncoveredElements == 0; }

	// vráti náhodný prvok, ktorý ešte nie je pokrytý
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

	//pridá množinu do riešenia
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

	// vráti, èi daný prvok už je pokrytý
	bool isElementCovered(size_t elementID) const
	{
		return ElementsCovered[elementID];
	}

	// vráti èi sa zadaná množina  nachádza v aktuálnom pokrytí(riešení)
	bool isSetInCover(size_t setID) const { return chosenSetsBool[setID]; }

	
	// odstráni náhodnú množinu z riešenia
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
	// pridá množinu do zoznamov vybratych
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