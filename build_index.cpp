// Note: compile with "g++ -O3 -std=c++11 -o name_exe name_file.cpp -lboost_serialization"
// Note: perf commands  "perf stat -e instructions:u,L1-dcache-loads,L1-dcache-loads-misses,cache-references,cache-misses ./proj"
// 						"perf record -e cycles ./proj collects samples"
// 						"perf report"
#include "baseline.hpp"

int main(int argc, char* argv[]) {
	if (argc == 4) {
		switch (atoi(argv[1])) { // Choose witch implementation to use
			case 0:
				baseline b;

				b.load(argv[2], "\0", "\0"); // Load file needed to build

				// Measure time construction indexes
				std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

				b.buildIndex();

				std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
			 	std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

				b.serialize(argv[3], "\0"); // Serialize the index

				std::cout << "Dimension index: " << b.size(argv[3]) << "MB \n"; // Print dimension of the generated index	
				
				break;
		}
	}
}