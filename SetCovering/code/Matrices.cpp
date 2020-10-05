#include <vector>
#include <iostream>

#ifndef _Matrix
#define _Matrix

using namespace std;

class MatrixIterator {
public:
	MatrixIterator(size_t i) : current_{ i } {}
	MatrixIterator operator++() {
		++current_;
		return *this;
	}
	size_t operator*() {
		return current_;
	}
	bool operator!=(const MatrixIterator & x) const
	{
		return x.current_ != current_;
	}
private:
	size_t current_;

};

class MatrixView {
public:
	MatrixView(size_t n) : n_{ n } {}

	MatrixIterator begin() {
		return MatrixIterator(0);
	}
	MatrixIterator end() {
		return MatrixIterator(n_);
	}

private:
	size_t n_;
};





/*
Matica
*/
template< typename T>
class MyMatrix {
public:
	MyMatrix<T>(): elements_{ 0 }, sets_{0} {}
	MyMatrix<T>(std::size_t elements, std::size_t sets): sets_{ sets }, elements_{elements}, v_(sets*elements, T{}) {}
	MyMatrix<T>(const MyMatrix<T> & m) = default;
	MyMatrix<T>(MyMatrix<T> && b): elements_{ b.elements_ }, sets_{b.sets_}, v_{ std::move(b.v_) } { b.elements_ = 0; b.sets_ = 0;
	}
	MyMatrix<T> & operator=(const MyMatrix<T> &) = default;
	MyMatrix<T> & operator=(MyMatrix<T> && b) 
	{
		elements_ = b.elements_;
		sets_ = b.sets_;
		v_ = std::move(b.v_);
		b.elements_ = 0;
		b.sets_ = 0;
		return *this;
	}

	/*
	Vracia množinu vrcholov
	*/
	MatrixView getSets() const
	{
		return MatrixView(sets_);
	}

	MatrixView getElements() const
	{
		return MatrixView(elements_);
	}
	/**/

	// vracia prvok v matici
	typename vector<T>::reference at(std::size_t elem, std::size_t set) { return v_.at(pos(elem, set)); }
	typename vector<T>::const_reference at(std::size_t elem, std::size_t set) const { return v_.at(pos(elem, set)); }

	typename std::vector<T>::iterator begin() {
		return v_.begin();
	}

	typename std::vector<T>::iterator end() { return v_.end(); }

	typename std::vector<T>::const_iterator cbegin() const {
		return v_.cbegin();
	}

	typename std::vector<T>::const_iterator cend()  const { return v_.cend(); }


private:
	// mapuje suradnice v matici na poziciu vo vektore
	std::size_t pos(std::size_t elem, std::size_t set) const {
		if (elem > elements_ || set > sets_) throw std::range_error("Matrix index out of range");
		return (elem*sets_) + set;

	}
	std::vector<T> v_;
	std::size_t sets_,elements_;
};



#endif

/**/