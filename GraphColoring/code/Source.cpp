#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "ACOGraphColoring.cpp"
#include <ctime>
#include "SAGraphColoring.cpp"
//#include "Problem.cpp"

// Parsuje mená problémov zo súboru
static vector<string> GetInstancesNames(string fileName)
{
	vector<string> names;
	ifstream input = ifstream(fileName);
	while (input.good())
	{
		
		std::string line;
		std::getline(input, line);
		
		names.push_back("instances\\"+line);
	}
	return names;
}

//Reprezentuje parametre algoritov
struct Params
{
public:
	Params(size_t numOfIter, size_t numOfAnts, double alpha, double beta, double evapor, double cool, double temp,size_t numOfTests)
	{
		numberOfIterations = numOfIter;
		numberOfAnts = numOfAnts;
		this->alpha = alpha;
		this->beta = beta;
		evaporation = evapor;
		coolingCoeficient = cool;
		numberOfTests = numOfTests;
		temperature = temp;
	}
	size_t numberOfIterations, numberOfAnts,numberOfTests;
	double alpha, beta, evaporation, coolingCoeficient, temperature;

	void print(std::ostream & os)
	{
		os << "Parameters: NumberOfIterations=" << numberOfIterations << ", NumberOfAnts=" << numberOfAnts << ", alpha=" << alpha << ", beta=" << beta << ", coolingCoeficient=" << coolingCoeficient << ", temperature=" << temperature << std::endl;
	}
};

//Parsuje parametre z konfiguraèného súboru
static Params TryParseParams(string fileName)
{
	Params p(70, 10, 2, 5, 0.4, 0.99, 10000,10);
	ifstream input = ifstream(fileName);
	size_t lineNum = 0;
	while (input.good())
	{
		std::string line;
		std::getline(input, line);
		++lineNum;
		if (line.find('=') !=string::npos)
		{
			
			try {
				std::string paramName = line.substr(0, line.find('='));
				if (paramName == "Temperature") { p.temperature = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Cooling_coeficient") { p.coolingCoeficient = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Alpha") { p.alpha = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Beta") { p.beta = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_ants") { p.numberOfAnts = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Evaporation") { p.evaporation = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_iterations") { p.numberOfIterations = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_tests") { p.numberOfTests = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else cout << "***Unknown parameter name on line " << lineNum << "***"<<endl;
			}
			catch (int a)
			{
				std::cout << "***Error in params.conf file on line: " << lineNum<< "***" << std::endl;
			}
		} else std::cout << "***Error in params.conf file on line: " << lineNum<<" . No \'=\' find***" << std::endl;
	}
	return p;
}





int main(int argc, char* args)
{

	vector<string> instanceNames =  GetInstancesNames("problems.txt"); // vyberie mena problemov
	ofstream os = ofstream("output.csv", std::ofstream::out | std::ofstream::app);
	Params p = TryParseParams("params.conf");
	p.print(os);
	p.print(std::cout);
	//os << "ProblemName;ACO-col;ACO-time;FF-col;FF-time;LDO-col;LDO-time;SA-col;SA-time" << endl;
	//každý problém rozparsuje a následne pustí všetky heuristiky s meraním èasu
	for (auto && instName:instanceNames)
	{
		cout << "Processing " << instName << endl;
		AdjecencyMatrix m = ParseProblem(instName);
		if (m.size() == 0) { cout << "***File " << instName << " is broken***" << std::endl; continue; }
		for (size_t i = 0; i < p.numberOfTests; i++)
		{


			clock_t begin, end;
			


			//////////////////////////////////////////ACO/////////////////////////////////////////////////
			/**/
			begin = clock();
			ACOSolPtr AcoSol = AntColony(p.numberOfIterations, p.numberOfAnts, p.alpha,p.beta, p.evaporation, m);
			end = clock();

			os << instName << ";ACO;" << AcoSol->GetNumberOfColors() << ";" << (float)(end - begin) / CLOCKS_PER_SEC << std::endl;
			cout << "ACO-Solution " << endl << "Number of colors:\t" << AcoSol->GetNumberOfColors() << endl << "Time elapsed:\t" << (float)(end - begin) / CLOCKS_PER_SEC << endl;

			/**/
			////////////////////////////////////////////FirstFit//////////////////////////////////////////////////
			begin = clock();
			SASolPtr ff = createFirstFitSolution(m);
			end = clock();
			os << instName << ";FirstFit;" << ff->GetNumberOfColors() << ";" << (float)(end - begin) / CLOCKS_PER_SEC << std::endl;
			cout << "FirstFit " << endl << "Number of colors:\t" << ff->GetNumberOfColors() << endl << "Time elapsed:\t" << (float)(end - begin) / CLOCKS_PER_SEC << endl;

			////////////////////////////////////////////LDO//////////////////////////////////////////////////
			
			begin = clock();
			SASolPtr ldo = createLDOSolution(m);
			end = clock();
			os << instName << ";LDO;" << ldo->GetNumberOfColors() << ";" << (float)(end - begin) / CLOCKS_PER_SEC << std::endl;
			cout << "LDO " << endl << "Number of colors:\t" << ldo->GetNumberOfColors() << endl << "Time elapsed:\t" << (float)(end - begin) / CLOCKS_PER_SEC << endl;

			////////////////////////////////////////////SA//////////////////////////////////////////////////
			
			begin = clock();
			SASolPtr ldoForSA = createLDOSolution(m);
			SASolPtr sa = SimulatedAnnealing(0.99, 10000, m, std::move(ldoForSA));
			end = clock();
			os << instName << ";SA;" << sa->GetNumberOfColors() << ";" << (float)(end - begin) / CLOCKS_PER_SEC << std::endl;
			cout << "SA-Solution " << endl << "Number of colors:\t" << sa->GetNumberOfColors() << endl << "Time elapsed:\t" << (float)(end - begin) / CLOCKS_PER_SEC << endl;
			/**/
		}
		
	}
	os.close();
	/**/
	char x; 
	std::cin >> x;
}




