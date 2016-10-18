// Naive implementation for the reading, creation and serialization of the data structure needed.
// Note: compile with "g++ -O3 -std=c++11 -o name_exe name_file.cpp -lboost_serialization"
// Note: perf commands  "perf stat -e instructions:u,L1-dcache-loads,L1-dcache-loads-misses,cache-references,cache-misses ./proj"
// 						"perf record -e cycles ./proj collects samples"
// 						"perf report"
// Removed all the space from series.txt
#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <iomanip>

#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/lexical_cast.hpp"

int main() {
	// File needed
	std::ifstream time_series;
	time_series.open("series.txt");
	std::ofstream dataset;
	dataset.open("dataset.txt");
	std::ofstream date;
	date.open("date.txt");
	std::string line;

	// Data structure containing the dataset
	std::map<std::string, std::vector<std::pair<int, int>>> pages;

	// Map structure to sort date on int and not on string
	std::map<std::string, int> date_to_id;

	// Scan the dataset to search all the date and then save it on the map "date_to_id"
	std::vector<std::string> all_date;
	if (time_series.is_open()) {
		while (std::getline(time_series,line)) {
		    std::stringstream linestream(line);
		    std::string data;

		    std::getline(linestream, data, '\t'); 
		    all_date.push_back(data);
	    }
	}
  	else std::cout << "Unable to open file";

  	// Removing duplicates of the date
    std::sort(all_date.begin(), all_date.end());
 	all_date.erase(std::unique(all_date.begin(), all_date.end()), all_date.end());

 	// Creating hash "date_to_id"
    int idDate=0;
	for (auto i=all_date.begin(); i<all_date.end(); i++) {
		date_to_id.insert(std::pair<std::string, int>(*i, idDate++));	
	}

	// Restart the pointer of the file "time_series"
	time_series.clear();
	time_series.seekg(0, std::ios::beg);

	// Creation of the data structure "pages"
	if (time_series.is_open()) {
		while (std::getline(time_series,line)) {
		    std::stringstream linestream(line);
		    std::string data;

		    std::getline(linestream, data, '\t'); 
		    int date = boost::lexical_cast<int>(date_to_id.find(data)->second);
		    std::getline(linestream, data, '\t'); 
		    std::string name_page = data;
		    std::getline(linestream, data, '\t'); 
		    int counter = boost::lexical_cast<int>(data);

		    if (pages.find(name_page)==pages.end()) {
		    	std::vector<std::pair<int, int>> tmp;
			    tmp.push_back(std::pair<int, int>(date, counter));
				pages.insert(std::pair<std::string, std::vector<std::pair<int, int>>>(name_page, tmp));
			} else {
				pages.find(name_page)->second.push_back(std::pair<int, int>(date, counter));
			}
	    }
	    time_series.close();
	}
  	else std::cout << "Unable to open file";

  	// Sorting all the list of the data structure "pages"
  	for(std::map<std::string, std::vector<std::pair<int, int>>>::iterator it = pages.begin(); it != pages.end(); ++it) {
  		std::sort(pages.find(it->first)->second.begin(), pages.find(it->first)->second.end());
	}

  	// Serialize "pages" and "date_to_id"
	boost::archive::text_oarchive oarch1(dataset);
	oarch1 << pages;

	boost::archive::text_oarchive oarch2(date);
	oarch2 << date_to_id;
}