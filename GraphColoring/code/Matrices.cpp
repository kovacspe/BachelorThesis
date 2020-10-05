#include <vector>
#include <iostream>

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
	
	//Vracia objekt na ktorom je mozn� iterovat. Iter�tor vracia v�etky platn� ��sla vrcholov.
	Vertices vertices() {
		return Vertices(size_);
	}
	
	// vracia prvok v matici
	typename vector<T>::reference at(std::size_t i, std::size_t j) { return v_.at(pos(i, j)); }
	typename vector<T>::const_reference at(std::size_t i, std::size_t j) const { return v_.at(pos(i, j)); }

	// vracia iter�tor, ktor� ukazuje na prv� hranu.T�mto iter�torom je mo�n� preiterova� v�etky hrany v grafe.
	typename std::vector<T>::iterator begin() {
		return v_.begin();
	}
	// vracia iter�tor, ktor� zn�zor�uje koniec zoznamu hr�n. T�mto iter�torom je mo�n� preiterova� v�etky hrany v grafe.
	typename std::vector<T>::iterator end() { return v_.end(); }
	// vracia const iter�tor, ktor� zn�zor�uje koniec zoznamu hr�n. T�mto iter�torom je mo�n� preiterova� v�etky hrany v grafe.
	typename std::vector<T>::const_iterator cbegin() const {
		return v_.cbegin();
	}
	// vracia const iter�tor, ktor� zn�zor�uje koniec zoznamu hr�n. T�mto iter�torom je mo�n� preiterova� v�etky hrany v grafe.
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
/**/
// Matica susednosti, analogicky ako predo�l� matica iba pre vector<bool>
class AdjecencyMatrix:public LowerMatrix<bool> {
public:
	/**/
	AdjecencyMatrix() : LowerMatrix<bool>() {}
	AdjecencyMatrix(std::size_t size) : LowerMatrix<bool>(size){}
	AdjecencyMatrix(const AdjecencyMatrix &) = default;
	AdjecencyMatrix(AdjecencyMatrix && b) : LowerMatrix<bool>(b) {}
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