// Note: compile with "g++ -O3 -std=c++11 -o name_exe name_file.cpp -lboost_serialization"
// Note: perf commands  "perf stat -e instructions:u,L1-dcache-loads,L1-dcache-loads-misses,cache-references,cache-misses ./proj"
// 						"perf record -e cycles ./proj collects samples"
// 						"perf report"
#include "baseline.hpp"
#include "baseline1.hpp"
#include "baseline2.hpp"
#include "baseline3.hpp"

int main(int argc, char* argv[]) {
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	std::chrono::duration<double> time_span;

	if (argc == 4) {
		switch (atoi(argv[1])) { // Choose witch implementation to use
			case 0: {
				Baseline b;

				b.load(argv[2], "\0", "\0"); // Load file needed to build

				// Measure time construction indexes
				t1 = std::chrono::high_resolution_clock::now();

				b.buildIndex();

				t2 = std::chrono::high_resolution_clock::now();
			 	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

				b.serialize(argv[3], "\0"); // Serialize the index

				std::cout << "Dimension index: " << b.size(argv[3]) << "MB \n"; // Print dimension of the generated index	
				
				break;
			}	
			case 1: {
				Baseline1 b1;

				b1.load(argv[2], "\0", "\0"); // Load file needed to build

				// Measure time construction indexes
				t1 = std::chrono::high_resolution_clock::now();

				b1.buildIndex();

				t2 = std::chrono::high_resolution_clock::now();
			 	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

				b1.serialize(argv[3], "\0"); // Serialize the index

				std::cout << "Dimension index: " << b1.size(argv[3]) << "MB \n"; // Print dimension of the generated index	
				
				break;
			}
			case 2: {
				Baseline2 b2;

				b2.load(argv[2], "\0", "\0"); // Load file needed to build

				// Measure time construction indexes
				t1 = std::chrono::high_resolution_clock::now();

				b2.buildIndex();

				t2 = std::chrono::high_resolution_clock::now();
			 	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

				b2.serialize(argv[3], "\0"); // Serialize the index

				std::cout << "Dimension index: " << b2.size(argv[3]) << "MB \n"; // Print dimension of the generated index	
				
				break;
			}
			case 3: {
				Baseline3 b3;

				b3.load(argv[2], "\0", "\0"); // Load file needed to build

				// Measure time construction indexes
				t1 = std::chrono::high_resolution_clock::now();

				b3.buildIndex();

				t2 = std::chrono::high_resolution_clock::now();
			 	time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
			 	std::cout << "Creation time indexes: " << time_span.count() << " seconds. \n";

				b3.serialize(argv[3], "\0"); // Serialize the index

				std::cout << "Dimension index: " << b3.size(argv[3]) << "MB \n"; // Print dimension of the generated index	
				
				break;
			}
		}
	}
}