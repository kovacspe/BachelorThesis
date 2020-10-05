#ifndef _Problem
#define _Problem


#include <iostream>
#include <vector>
#include <fstream>
#include "Matrices.cpp"
#include <string>


// Nadradena trieda Solution popisujuca riesenie problemu
class Solution {
public:
	// vypíše ofarbenie do streamu
	void printSolution(std::ostream & out) const
	{
		out << "Coloring in format (vertexNumber,colorNumber)" << std::endl;
		for (size_t i = 0; i < GetNumberOfVertices(); i++)
		{
			out << "(" << i << ", " << GetColor(i) << "), ";
		}
		out << std::endl << "Number of colors: " << GetNumberOfColors() << std::endl;
	};
	// vráti poèet použitých farieb v riešení
	virtual size_t GetNumberOfColors() const = 0;
	// vráti èíslo farby daného vrcholu
	virtual size_t GetColor(size_t vertex) const = 0;
	// vráti poèet vrcholov
	virtual size_t GetNumberOfVertices() const = 0;

};

// Parsuje popisy problemov do matice susednosti
static AdjecencyMatrix ParseProblem(string fileName) 
{
	char c;
	int e1, e2;
	std::string line;
	bool haveDimension = false;
	std::ifstream input = ifstream(fileName);
	// zistuje kolko vrcholov má problém
	while (input.good() && !haveDimension) {
		input >> c;
		if (c == 'p')
		{
			input >> line >> e1 >> e2;
			cout << "Problem \"" << fileName << "\" parsed. Number of vertices: " << e1 << endl;
			haveDimension = true;
		}
		else std::getline(input,line);
		
	}if (!haveDimension) return AdjecencyMatrix(0);
	AdjecencyMatrix adjMatrix(e1);
	//Naplna maticu susednosti
	while (input.good()) {
		input >> c >> e1 >> e2;
		
		if (c == 'e') adjMatrix.setEdge(e1-1, e2-1,true);
	}
	input.close();
	return adjMatrix;

}
#endif