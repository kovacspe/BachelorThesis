#include "Problem.cpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <memory>


using namespace std;
/*
Riešenie problému vytvorené ACO
Podporuje èiastkové riešenie pomocou ACO 
*/
class ACOSolution:public Solution
{
public:
	ACOSolution(size_t numOfVertices) : vertices_( numOfVertices, 0 ), 
		verticesColoredByColor_( numOfVertices, vector<size_t>{} ) {

	}

	/* 
	nastavuje vrchol na farbu
	Ak vrchol upravi nna farbu vyskrtne ju zo zoznamu moznych farieb pre susedne vrcholy
	*/
	void SetColor(size_t vertex, size_t colorNumber,vector<size_t> & feasibleForColor,const AdjecencyMatrix & adjecencyMatrix) {
		if (vertices_[vertex] == 0)
		{
			vertices_[vertex] = colorNumber;
			verticesColoredByColor_[colorNumber-1].push_back(vertex);
			for (int j = feasibleForColor.size() ;j > 0;--j)
			{
				if (adjecencyMatrix.isEdge(feasibleForColor[j-1], vertex)) {
					feasibleForColor.erase(feasibleForColor.begin() + j-1);
				}
			}

		}
		else throw std::logic_error("Already colored");

	}
	// vráti èíslo farby daného vrcholu
	size_t GetColor(size_t vertex) const override {
		return vertices_[vertex];
	}

	// vráti poèet použitých farieb v riešení
	size_t GetNumberOfColors() const override {
		size_t max = 0;
		while (max<verticesColoredByColor_.size() && verticesColoredByColor_[max].size() != 0) {
			++max;
		}
		return max;
	}

	// vrati pocet vrcholov zafarbenych danou farbou
	size_t GetNumberVerticesColoredByColor(size_t color) const {
		return verticesColoredByColor_[color].size();
	}

	// vráti heuristický koeficient
	double getHeuristicCoef(size_t vertex, const vector<size_t> & feasibleForColor, const AdjecencyMatrix & adjecencyMatrix) const
	{
		size_t sum = feasibleForColor.size();
		for (auto && v : feasibleForColor)
		{
			if (adjecencyMatrix.isEdge(vertex, v))  --sum;
		}
		return  sum;
	}
	/*
	double getHeuristicCoef(size_t vertex,const vector<size_t> & feasibleForColor, const AdjecencyMatrix & adjecencyMatrix) const
	{
		size_t sum = feasibleForColor.size();
		for (auto && v : feasibleForColor)
		{
			if (adjecencyMatrix.isEdge(vertex, v))  --sum;
		}
		return  sum;
	}
	/**/
	/* Like SDO
	double getHeuristicCoef(size_t vertex, const vector<size_t> & feasibleForColor, const AdjecencyMatrix & adjecencyMatrix) const
	{
		vector<bool> neighbourColors(adjecencyMatrix.size(), false);
		size_t sum = 0;
		for (size_t i=0;i<adjecencyMatrix.size();++i)
		{
			if (adjecencyMatrix.isEdge(vertex, i) && neighbourColors.at(GetColor(i)) == false)
			{
				++sum;
				neighbourColors.at(GetColor(i)) = true;
			}
		}
		return  sum;
	}
	/**/
	// pocita tau pre vzorec na rozhodovanie 
	double tau(size_t vertex, size_t color,const LowerMatrix<double> & trailMatrix) const
	{
		double sum=0;
		for (size_t i = 0; i < verticesColoredByColor_[color].size(); i++)
		{
			if (verticesColoredByColor_[color][i] != vertex)
				sum += trailMatrix.at(verticesColoredByColor_[color][i], vertex);
		}
		return sum / GetNumberVerticesColoredByColor(color);
	}
	// scita hodnotu "pravdepodobnosti" pre vsetky možnosti 
	double countProbSum(const vector<size_t> & feasibleForColor, size_t currColor, const LowerMatrix<double> & trailMatrix,const AdjecencyMatrix & adjecencyMatrix,double alpha, double beta) const
	{
		double sum = 0;
		for (size_t i = 0; i<feasibleForColor.size(); i++)
		{
			sum += probability(feasibleForColor[i],currColor, trailMatrix,feasibleForColor,adjecencyMatrix, alpha, beta);
		}
		return sum;
	}
	// pocita hodnotu pre nasledujuci krok, cim vacsia hodnota, tym pravdepodobvnejsie
	double probability(size_t vertex, size_t color, const LowerMatrix<double> & trailMatrix,const vector<size_t> & feasibleForColor,const AdjecencyMatrix & adjecencyMatrix, double alpha, double beta) const
	{

		return pow(tau(vertex,color,trailMatrix), alpha)*pow((getHeuristicCoef(vertex,feasibleForColor,adjecencyMatrix)), beta);

	}

	// vyberie podla pravdepodobnosti nasledujuci krok
	int chooseVertex(size_t currColor,const vector<size_t> & feasibleForColor, const LowerMatrix<double> & trailMatrix,const AdjecencyMatrix & adjecencyMatrix,double alpha,double beta) const
	{

		double p = rand() / ((double)RAND_MAX);

		size_t vertexIndex = 0;
		double probSum = countProbSum(feasibleForColor,currColor,trailMatrix,adjecencyMatrix,alpha,beta);
		double sum = probability(feasibleForColor.front(),currColor,trailMatrix,feasibleForColor,adjecencyMatrix,alpha,beta) / probSum;


		while (sum<(p*0.9999))
		{
			vertexIndex++;
			sum += probability(feasibleForColor.at(vertexIndex), currColor, trailMatrix,feasibleForColor,adjecencyMatrix,alpha,beta) / probSum;

		}

		return feasibleForColor.at(vertexIndex);
	}
	/**/

	void updateTrailMatrix(LowerMatrix<double> & trailMatrix)
	{
		for (size_t i = 0; i < verticesColoredByColor_.size(); ++i)
		{
			for (size_t j = 0; j < verticesColoredByColor_[i].size(); ++j)
			{
				for (size_t k = j+1; k < verticesColoredByColor_[i].size(); ++k)
				{
					trailMatrix.at(verticesColoredByColor_[i][j], verticesColoredByColor_[i][k]) += 1 / GetNumberOfColors();
				}
			}
		}
	}

	size_t GetNumberOfVertices() const override
	{
		return vertices_.size();
	}

	// skonštruuje riešenie pre jedného mravca
	void constructSolution(const LowerMatrix<double> & trailMatrix,const AdjecencyMatrix & adjecencyMatrix,double alpha, double beta) {
		vector<size_t> uncolored;
		size_t currColor = 0;
		for (size_t i = 0; i <vertices_.size();++i)
		{
			uncolored.push_back(i);
		}
		while (!uncolored.empty())
		{
			++currColor;
			vector<size_t> feasibleForColor = uncolored;
			size_t randIndex = rand() % feasibleForColor.size();

			size_t vertex = feasibleForColor.at(randIndex);
			
			uncolored.erase(uncolored.begin() + randIndex);
			feasibleForColor.erase(feasibleForColor.begin() + randIndex );

			SetColor(vertex, currColor,feasibleForColor,adjecencyMatrix);
			while (!feasibleForColor.empty())
			{
				vertex = chooseVertex(currColor,feasibleForColor, trailMatrix,adjecencyMatrix,alpha, beta);
				auto it = find(feasibleForColor.begin(), feasibleForColor.end(), vertex);
				feasibleForColor.erase(it);
				it = find(uncolored.begin(), uncolored.end(), vertex);
				uncolored.erase(it);
				SetColor(vertex, currColor, feasibleForColor, adjecencyMatrix);
			}

		}
	}

	

private:
	vector<std::size_t> vertices_;
	vector<vector<std::size_t>>	verticesColoredByColor_;
};


using ACOSolPtr = std::unique_ptr<ACOSolution>;
static void evaporate(LowerMatrix<double> & trailMatrix, double evaporation) {


	for (auto && edge : trailMatrix)
	{
		edge *= evaporation;
	}
}

// sputí Ant Colony s danými parametrami na danom probléme a vráti pointer na riešenie
static ACOSolPtr AntColony(size_t numberOfIterations, size_t numberOfAnts,double alpha, double beta,double evaporation,const AdjecencyMatrix & adjecencyMatrix)
{
	// incicializuje feromonove stopy
	LowerMatrix<double> trailMatrix(adjecencyMatrix.size());
	for (auto v : trailMatrix.vertices())
		for (auto v2 : trailMatrix.vertices())
			if (adjecencyMatrix.isEdge(v, v2)) { trailMatrix.at(v, v2) = 1; }

	ACOSolPtr bestSoFarSolution = NULL;
	for (size_t i = 0;i < numberOfIterations;++i) {
		vector<ACOSolPtr> solutions;
		for (size_t i = 0;i<numberOfAnts;++i)
		{
			ACOSolPtr currSolution = make_unique<ACOSolution>(adjecencyMatrix.size());
			currSolution->constructSolution(trailMatrix, adjecencyMatrix,alpha, beta);
			solutions.push_back(std::move(currSolution));
		}
		evaporate(trailMatrix,evaporation);
		while (solutions.size() != 0) {
			ACOSolPtr s = std::move(solutions.back());
			solutions.pop_back();
			s->updateTrailMatrix(trailMatrix);

			if (bestSoFarSolution == NULL || (bestSoFarSolution->GetNumberOfColors()) > (s->GetNumberOfColors())) {
				bestSoFarSolution.release();
				bestSoFarSolution = std::move(s);
			}
			else s.reset();
		}
	}
	return bestSoFarSolution;
}