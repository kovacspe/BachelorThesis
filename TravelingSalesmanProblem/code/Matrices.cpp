#ifndef _Matrix
#define _Matrix

#include <vector>
#include <iostream>
#include <string>
#include <memory>

using namespace std;

class VertexIterator {
public:
	VertexIterator(size_t i) : currentVertex_{ i } {}
	VertexIterator operator++() {
		++currentVertex_;
		return *this;
	}
	size_t operator*() {
		return currentVertex_;
	}
	bool operator!=(const VertexIterator & x) const
	{
		return x.currentVertex_ != currentVertex_;
	}
private:
	size_t currentVertex_;

};


class Vertices {
public:
	Vertices(size_t numberOfVertices) : numberOfVertices_{ numberOfVertices } {}

	VertexIterator begin() {
		return VertexIterator(0);
	}
	VertexIterator end() {
		return VertexIterator(numberOfVertices_);
	}

private:
	size_t numberOfVertices_;
};






/*
Dolna trojuholnikova matica
*/
template< typename T>
class LowerMatrix {
public:
	LowerMatrix<T>() : size_{0} {}
	LowerMatrix<T>(std::size_t size) : size_{ size }, v_(size*(size + 1) / 2, T{}) {}
	LowerMatrix<T>(const LowerMatrix<T> &) = default;
	LowerMatrix<T>(LowerMatrix<T> && b) : size_{ b.size_ }, v_{ std::move(b.v_) } { b.size_ = 0; }
	LowerMatrix<T> & operator=(const LowerMatrix<T> &) = default;
	LowerMatrix<T> & operator=(LowerMatrix<T> && b) {
		size_ = b.size()
		v_ = std::move(b.v_);
		b.size = 0;
		return *this;
	}
	
	//Vracia objekt na ktorom je mozné iterovat. Iterátor vracia všetky platné èísla vrcholov.
	Vertices vertices() const{
		return Vertices(size_);
	}
	
	// vracia prvok v matici
	typename vector<T>::reference at(std::size_t i, std::size_t j) { return v_.at(pos(i, j)); }
	typename vector<T>::const_reference at(std::size_t i, std::size_t j) const { return v_.at(pos(i, j)); }

	// vracia iterátor, ktorı ukazuje na prvú hranu.Tımto iterátorom je moné preiterova všetky hrany v grafe.
	typename std::vector<T>::iterator begin() {
		return v_.begin();
	}
	// vracia iterátor, ktorı znázoròuje koniec zoznamu hrán. Tımto iterátorom je moné preiterova všetky hrany v grafe.
	typename std::vector<T>::iterator end() { return v_.end(); }
	// vracia const iterátor, ktorı znázoròuje koniec zoznamu hrán. Tımto iterátorom je moné preiterova všetky hrany v grafe.
	typename std::vector<T>::const_iterator cbegin() const {
		return v_.cbegin();
	}
	// vracia const iterátor, ktorı znázoròuje koniec zoznamu hrán. Tımto iterátorom je moné preiterova všetky hrany v grafe.
	typename std::vector<T>::const_iterator cend()  const { return v_.cend(); }
	
	// vracia rozmer matice
	std::size_t size() const { return size_; }
private:
	// mapuje suradnice v matici na poziciu vo vektore
	std::size_t pos(std::size_t i, std::size_t j) const {
		if (i > size_ || j > size_|| i==j) throw std::range_error("Matrix index out of range");
		
		else if (i>j) return (i*(i + 1) / 2) + j;
		else return (j*(j + 1) / 2) + i;

	}

	
	
	std::vector<T> v_;
	std::size_t size_;
};
/*
// Matica susednosti, analogicky ako predošlá matica iba pre vector<bool>
template<typename T>
class LowerMatrix:public Matrix<T> {
public:
	/*
	AdjecencyMatrix() : LowerMatrix<bool>() {}
	AdjecencyMatrix(std::size_t size) : size_{ size }, v_(size*(size + 1) / 2, T{}){}
	AdjecencyMatrix(const AdjecencyMatrix &) = default;
	AdjecencyMatrix(AdjecencyMatrix && b) : Matrix<T>(b) {}
	AdjecencyMatrix & operator=(const AdjecencyMatrix &) = default;

	
	bool isEdge(size_t x, size_t y) const {
		if (x == y) return false;
		return  at(x,y);
	}

	void setEdge(size_t x, size_t y, bool val) {
		at(x,y) =val;
	}
};



/**/


// Trieda popisujúca riešenie TSP
class Solution
{
public:
	// pridá vrchol do cesty
	void AddToPath(size_t vertex)
	{
		path.push_back(vertex);

	}
	// Vráti pointer na novo vytvorenú instanciu Solution, ktorá je susednım riešením. Teda nejakı interval je v opaènom poradí.
	unique_ptr<Solution> createNeighbour(size_t v1, size_t v2) const
	{
		unique_ptr<Solution> newTour = std::make_unique<Solution>();
		if (v1>v2)
			swap(v1, v2);

		for (size_t i = 0; i<path.size(); i++)
		{
			if ((i<v1) || (i>v2))
			{
				newTour->AddToPath(path[i]);
			}
			else
			{
				newTour->AddToPath(path[v2 - (i - v1)]);
			}
		}
		return std::move(newTour);
		
	}

	// vráti cenu riešenia, teda súèet vzdialeností okrunej cesty
	double getCost(LowerMatrix<double> distances) const
	{
		
		double sum = 0;
		for (size_t i = 0; i < path.size(); i++)
		{
			sum+=distances.at(path[i], path[(i + 1) % path.size()]);
		}
		return sum;
	}
	//vráti poslednı vrchol v aktuálnej ceste
	size_t currentVertex() const
	{
		return path.back();
	}
	// vráti const referenciu na vector vrcholov
	const vector<size_t> & getPath() const
	{
		return path;
	}

	void log(string instName,ostream & os) const
	{
		os << instName << ";";
		for (auto &&x : path)
		{
			os << ";" << x;
		}
	}

private:
	vector<size_t> path;
};




static double countDelta(const Solution & currTour, const LowerMatrix<double> & distances, std::size_t first, std::size_t second)
{

	auto currPath = currTour.getPath();

	if ((second + 1) % currPath.size() == first) return 0;
	else
		return distances.at(currPath[first], currPath[(second + 1) % currPath.size()])
		+ distances.at(currPath[second], currPath[(first - 1 + currPath.size()) % currPath.size()])
		- distances.at(currPath[second], currPath[(second + 1) % currPath.size()])
		- distances.at(currPath[first], currPath[(first - 1 + currPath.size()) % currPath.size()]);


}

#endif