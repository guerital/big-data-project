#include "baseline.hpp"

int main(int argc, char* argv[]) {
	if (argc == 4) {
		baseline b;	// Choose this implementation because it's the only one with the method 'generateQueries'

		b.load(argv[1], "\0", "\0"); // Load the file to generate the queries

		b.generateQueries(atoi(argv[2])); // Generate all the queries

		b.serialize("\0", argv[3]); // Save the generated queries
 	}
}