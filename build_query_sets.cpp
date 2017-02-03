#include <algorithm>
#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/lexical_cast.hpp"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream> 
#include <map>
#include <numeric>
#include <queue>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "query.hpp"

int main(int argc, char* argv[]) {
	if (argc == 6) {
		Query q;	// Choose this implementation because it's the only one with the method 'generateQueries'

		q.load(argv[1], "\0", "\0", "\0"); // Load the file to generate the queries

		q.generateQueries(atoi(argv[2])); // Generate all the queries

		q.serialize(argv[3], argv[4], argv[5]); // Save the generated queries
 	}
}