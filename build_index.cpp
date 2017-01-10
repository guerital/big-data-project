// Note: compile with "g++ -O3 -std=c++11 -o name_exe name_file.cpp -lboost_serialization"
// Note: perf commands  "perf stat -e instructions:u,L1-dcache-loads,L1-dcache-loads-misses,cache-references,cache-misses ./proj"
// 						"perf record -e cycles ./proj collects samples"
// 						"perf report"
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
#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <sdsl/vectors.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "baseline.hpp"
#include "baseline1.hpp"
#include "baseline2.hpp"
#include "baseline3.hpp"
#include "baseline4.hpp"

template<typename BaselineNumber, typename LoadFileName, typename StoreFileName>
void execute(BaselineNumber& b, const LoadFileName& lfn, const StoreFileName& sfn) {
	b.load(lfn, "\0"); // Load file needed to build

	// Measure time construction indexes
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

	b.buildIndex();

	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
 	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

	b.serialize(sfn); // Serialize the index

	std::cout << "Dimension index: " << b.size(sfn) << "MB \n"; // Print dimension of the generated index
}

int main(int argc, char* argv[]) {
	if (argc == 4) {
		switch (atoi(argv[1])) { // Choose which implementation to use
			case 0: {
				Baseline b;
				execute(b, argv[2], argv[3]);	
			
				break;
			}	
			case 1: {
				Baseline1 b1;
				execute(b1, argv[2], argv[3]);
				
				break;
			}
			case 2: {
				Baseline2 b2;
				execute(b2, argv[2], argv[3]);
				
				break;
			}
			case 3: {
				Baseline3 b3;
				execute(b3, argv[2], argv[3]);
				
				break;
			}
			case 4: {
				Baseline4 b4;
				execute(b4, argv[2], argv[3]);
				
				break;
			}
		}
	}
}