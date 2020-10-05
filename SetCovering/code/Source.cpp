//#include "Matrices.cpp"
#include <ctime>
#include "SetCoveringACO.cpp"
#include "SetCoveringSA.cpp"

vector<string> GetInstancesNames(string fileName)
{
	vector<string> names;
	ifstream input = ifstream(fileName);
	while (input.good())
	{
		std::string line;
		std::getline(input, line);
		names.push_back("instances\\" + line);
	}
	return names;
}

struct Params
{
public:
	Params(size_t numOfIter, size_t numOfAnts, double alpha, double beta, double evapor, double cool, double temp, size_t numOfTests, size_t maxTabuSize, size_t numberOfIterInTabu)
	{
		numberOfIterations = numOfIter;
		numberOfAnts = numOfAnts;
		this->alpha = alpha;
		this->beta = beta;
		evaporation = evapor;
		coolingCoeficient = cool;
		numberOfTests = numOfTests;
		temperature = temp;
		numberOfIterationsInTabu = numberOfIterInTabu;
		this->maxTabuSize = maxTabuSize;
	}
	size_t numberOfIterations, numberOfAnts, numberOfTests, maxTabuSize, numberOfIterationsInTabu;
	double alpha, beta, evaporation, coolingCoeficient, temperature;

	void print(std::ostream & os)
	{
		os << "Parameters: NumberOfIterations=" << numberOfIterations << ", NumberOfAnts=" << numberOfAnts << ", alpha=" << alpha << ", beta=" << beta <<
			", coolingCoeficient=" << coolingCoeficient << ", temperature=" << temperature << ", number_of_test=" << numberOfTests << ", maxTabuSize=" << maxTabuSize << std::endl;
	}
};


static Params TryParseParams(string fileName)
{
	Params p(150, 20, 3, 2, 0.4, 0.99999, 1000000, 10, 100, 50);
	ifstream input = ifstream(fileName);
	size_t lineNum = 0;
	while (input.good())
	{
		std::string line;
		std::getline(input, line);
		++lineNum;
		if (line.find('=') != string::npos)
		{

			try {
				std::string paramName = line.substr(0, line.find('='));
				if (paramName == "Temperature") { p.temperature = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Cooling_coeficient") { p.coolingCoeficient = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Evaporation") { p.evaporation = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Alpha") { p.alpha = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Beta") { p.beta = stod(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_ants") { p.numberOfAnts = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_iterations") { p.numberOfIterations = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_tests") { p.numberOfTests = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Max_tabu_size") { p.maxTabuSize = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else if (paramName == "Number_of_iterations_tabu") { p.numberOfIterationsInTabu = stoi(line.substr(line.find('=') + 1, line.size() - line.find('=') + 1)); }
				else cout << "***Unknown parameter name on line " << lineNum << "***" << endl;
			}
			catch (int a)
			{
				std::cout << "***Error in params.conf file on line: " << lineNum << "***" << std::endl;
			}
		}
		else std::cout << "***Error in params.conf file on line: " << lineNum << " . No \'=\' find***" << std::endl;
	}
	return p;
}


int main(int argc, char * argv)
{
	vector<string> instanceNames = GetInstancesNames("problems.txt"); // vyberie mena problemov
	ofstream os = ofstream("output.csv", std::ofstream::out | std::ofstream::app);
	Params params = TryParseParams("params.conf");
	for (auto && instName : instanceNames)	
	{
		clock_t begin, end;
		Problem p = ParseProblem(instName);
		os << instName << ";SOLUTION COMPLEXITY(S,E);" << p.getNumOfSets() << ";" << p.getNumOfElements() << std::endl;
		for (size_t i = 0; i < params.numberOfTests; i++) // pocet testov pre kazdu instanciu
		{
			
			/**/
			////////////////////////////////////ACO/////////////////////////////////////////
			std::cout << "ACO processing..." << std::endl;
			begin = clock();
			auto solACO = AntColony(params.numberOfIterations,params.numberOfAnts, params.beta, params.evaporation, p);
			end = clock();
			
			std::cout << solACO.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
			os << instName << ";ACO;" << solACO.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
			/**/
			if (60 < (end - begin) / CLOCKS_PER_SEC) ++i;
			/**/
			//////////////////////////////////// SA/////////////////////////////////////
			std::cout << "Simulated annealing processing.." << std::endl;
			begin = clock();
			auto solGreedyForSA = Greedy(p);
			auto solSA = SimulatedAnnealing(params.coolingCoeficient, params.temperature, p, solGreedyForSA);
			end = clock();
			clock_t SAtime = end - begin;
			std::cout << solSA.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
			os << instName << ";SA;" << solSA.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
			/**/

			}
		/**/
		//////////////////////////////////// Greedy/////////////////////////////////////
		std::cout << "Greedy processing.." << std::endl;
		begin = clock();
		auto solGreedy = Greedy(p);
		end = clock();
		clock_t GreedyTime = end - begin;
		std::cout << solGreedy.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
		os << instName << ";Greedy;" << solGreedy.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
		/**/
		//////////////////////////////////// TS/////////////////////////////////////
		std::cout << "Tabu processing.." << std::endl;
		begin = clock();
		auto solGreedyForTS = Greedy(p);
		auto solTS = TabuSearch(params.numberOfIterationsInTabu, params.maxTabuSize, p, solGreedyForTS);
		end = clock();
		std::cout << solTS.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
		os << instName << ";TS;" << solTS.getCost(p.getCosts()) << ";" << (double)(end - begin) / CLOCKS_PER_SEC << std::endl;
		/**/
	}
	string a;
	std::cin >> a;
}