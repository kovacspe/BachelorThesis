#define _USE_MATH_DEFINES

#include <vector>
#include "Problem.cpp"
#include <cmath>
#include <algorithm>
#include <memory>


enum InitialSolutionType {FIRST_FIT, LDO};

class SASolution :public Solution
{
public:
	SASolution(size_t numberOfVertices) : vertices_(numberOfVertices) {}
	SASolution(const SASolution &) = default;
	SASolution(SASolution && b) :vertices_(b.vertices_), verticesColoredByColor_{ b.verticesColoredByColor_ } {}
	SASolution & operator=(const SASolution &) = default;
	SASolution & operator=(SASolution&& b) = default;

	// vráti farbu vrcholu
	size_t GetColor(size_t vertex) const override {
		return vertices_[vertex];
	}
	
	//nastaví vrchol na farbu
	void SetColor(size_t vertex, size_t colorNumber)
	{
		// ak bol vrchol už zafarbený, vyradí ho zo zoznamu
		if (vertices_[vertex] != 0) {
			for (size_t j = 0; j < verticesColoredByColor_[vertices_[vertex] - 1].size(); j++)
			{
				if (verticesColoredByColor_[vertices_[vertex] - 1].at(j)==vertex)
				{
					verticesColoredByColor_[vertices_[vertex] - 1].erase(verticesColoredByColor_[vertices_[vertex] - 1].begin() + j);
					break;
				}
			}
		}
		// zafarí vrchol a pridá ho do zoznamu danej farby
		vertices_[vertex] = colorNumber;
		if (colorNumber - 1 < verticesColoredByColor_.size())
			verticesColoredByColor_[colorNumber - 1].push_back(vertex);
		else
			verticesColoredByColor_.push_back(vector<size_t>{vertex});

	}

	//zmeni riesenie na susedne, teda zmeni jednu farbu
	void ModifyToNeighbour(const AdjecencyMatrix & adjecencyMatrix)
	{
		
		std::vector<bool> conflictVertices(vertices_.size(),false);
		for (size_t i = 0; i < verticesColoredByColor_.size(); i++)
		{
			for (size_t j = 0;j<verticesColoredByColor_[i].size();j++)
				for (size_t k = j + 1;k < verticesColoredByColor_[i].size();k++)
				{
					if (adjecencyMatrix.isEdge(j, k)) {
						conflictVertices[j] = true;
						conflictVertices[k] = true;
					}
				}
		}

		size_t sum = 0;
		for (size_t i = 0; i < vertices_.size(); i++)
		{
			if (conflictVertices[i]) ++sum;
		}


		if (sum == 0)
		{
			auto ver = rand() % vertices_.size();
			size_t col= (rand() % GetNumberOfColors()) +1;			
			SetColor(ver, col);
		}
		else {
			size_t pos = rand() % sum;
			sum = 0;
			size_t ver = 0;
			for (size_t i = 0; i < vertices_.size(); i++)
			{
				if (conflictVertices[i]) ++sum;
				
				if (pos == sum) { ver = i; break; }
			}
			size_t col = 1 + (rand() % (GetNumberOfColors()+1));
			SetColor(ver, col);
		}
	}

	//vráti poèet hrán s konfliktom
	size_t GetNumberOfConflictEdges(const AdjecencyMatrix & adjecencyMatrix) const{
		size_t sum = 0;;
		for (size_t i = 0; i < verticesColoredByColor_.size(); i++)
		{
			for(size_t j=0;j<verticesColoredByColor_[i].size();j++)
				for (size_t k = j+1;k < verticesColoredByColor_[i].size();k++)
				{
					if (adjecencyMatrix.isEdge(verticesColoredByColor_[i].at(j), verticesColoredByColor_[i].at(k))) {
						++sum;
					}
				}
		}
		return sum;
	}

	//vráti cenu riešenia
	size_t costFunction(const AdjecencyMatrix & adjecencyMatrix) const
	{
		size_t conflicts = GetNumberOfConflictEdges(adjecencyMatrix);
		if (conflicts > 0) return GetNumberOfColors() + 2*conflicts  + 20;
		return GetNumberOfColors();
	}

	//vráti poèet použitých farieb v riešení
	size_t GetNumberOfColors() const override {
		size_t max = 0;
		for (size_t i = 0; i < verticesColoredByColor_.size(); i++)
		{
			if (verticesColoredByColor_[i].size() != 0) ++max;
		}
		return max;
	}

	//vráti poèet vrcholov
	size_t GetNumberOfVertices() const override
	{
		return vertices_.size();
	}


private:
	std::vector<size_t> vertices_;
	vector<vector<std::size_t>>	verticesColoredByColor_;

	
};



using SASolPtr = std::unique_ptr<SASolution>;

// naplní riešenie metódov First Fit
static SASolPtr createFirstFitSolution(const AdjecencyMatrix & adjecencyMatrix)
{
	SASolPtr solution = std::make_unique<SASolution>(adjecencyMatrix.size());
	for (size_t j = 0; j < adjecencyMatrix.size(); j++)
	{
		
		vector<bool> possibleColors(adjecencyMatrix.size() + 1, true);
		for (size_t i = 0; i < j; i++)
		{
			if (adjecencyMatrix.isEdge(i, j))
			{
				possibleColors.at(solution->GetColor(i)) = false;
			}
		}
		for (size_t i = 1; i < possibleColors.size() + 1; i++)
		{
			if (possibleColors.at(i) == true) {
				solution->SetColor(j, i);
				break;
			}
		}

	}
	return std::move(solution);
}

// pomocná trieda pre urèenie farby vrcholu
class vertex {
public:
	vertex(size_t ID, size_t numberOfVertices) : IDnumber(ID), numberOfRestrictedColors(0), notPossibleColors(numberOfVertices + 1, false), isColored(false) {

	}

	//Zakáže farbu pre tento vrchol
	void restrictColor(size_t colorNumber, size_t vertexID, const  AdjecencyMatrix & adjecencyMatrix) {
		if (notPossibleColors.at(colorNumber) == false && adjecencyMatrix.isEdge(IDnumber, vertexID)) {
			notPossibleColors.at(colorNumber) = true;
			++numberOfRestrictedColors;
		}
	}

	// vráti poèet zakázaných farieb
	size_t getNumberOfRestrictedColors()
	{
		return numberOfRestrictedColors;
	}
	// vráti najnižšiu možnú farbu
	size_t assignLowestColor()
	{
		isColored = true;
		size_t i = 1;
		while (i < notPossibleColors.size())
		{
			if (!notPossibleColors.at(i)) { return i; }
			++i;
		}
		return i;
	}

	bool isColored;
private:
	size_t IDnumber;
	size_t numberOfRestrictedColors;
	vector<bool> notPossibleColors;

};

// vyplni riesenie metodov LDO
static SASolPtr createLDOSolution(const AdjecencyMatrix & adjecencyMatrix)
{
	SASolPtr solution = std::make_unique<SASolution>(adjecencyMatrix.size());
	vector<vertex> vertices;
	for (size_t j = 0; j < adjecencyMatrix.size(); ++j)
	{
		vertices.push_back(vertex(j, adjecencyMatrix.size()));

	}
	for (size_t j = 0; j < vertices.size(); ++j)
	{
		int max = -1;
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			if (vertices.at(i).isColored == false && (max == -1 || vertices.at(i).getNumberOfRestrictedColors() > vertices.at(max).getNumberOfRestrictedColors()))
			{
				max = i;
			}
		}
		size_t col = vertices.at(max).assignLowestColor();
		solution->SetColor(max, col);
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			vertices.at(i).restrictColor(col, max, adjecencyMatrix);
		}

	}
	return std::move(solution);
}

static SASolPtr SimulatedAnnealing(double coolingCoef, double temperature, const AdjecencyMatrix & adjecencyMatrix,SASolPtr initialSolution)
{
	SASolPtr currSolution = std::make_unique<SASolution>(*initialSolution);

	SASolPtr bestSoFarSolution = std::make_unique<SASolution>(*currSolution);
	while (temperature > 0.1) {
		SASolPtr newSol = std::make_unique<SASolution>(*currSolution);
		newSol->ModifyToNeighbour(adjecencyMatrix);
		size_t newSolCost = newSol->costFunction(adjecencyMatrix);
		double delta = newSolCost - (currSolution->costFunction(adjecencyMatrix));
		if (delta < 0) cout << "**********************************************************\n";
		double probability = (pow(2.71828, -(delta / temperature)));
		//cout << temperature_ << ": " << newSolCost << ", prob: " << probability << std::endl;
		if ((rand() / (double)RAND_MAX) <= probability)
		{
			currSolution = std::move(newSol);
			if (bestSoFarSolution == NULL || newSolCost < bestSoFarSolution->costFunction(adjecencyMatrix))
			{

				bestSoFarSolution = std::make_unique<SASolution>(*currSolution);
			}
		}
		temperature *= coolingCoef;
	}
	return std::move(bestSoFarSolution);
}