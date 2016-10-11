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

void remove_duplicates(std::vector<std::string>& vec) {
  	std::sort(vec.begin(), vec.end());
 	vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

bool sort_pred(const std::pair<int, int> &left, const std::pair<int, int> &right) {
    return left.first < right.first;
}

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

		   	std::istringstream iss(line);

		   	int c = 0;
		    while (iss) {
		    	std::string sub;
		       	iss >> sub;
		       	switch (c++) {
		       		case 0:
		       			all_date.push_back(sub);
		       			break;
		       	}	       		 
		    }
	    }
	}
  	else std::cout << "Unable to open file";

    remove_duplicates(all_date);

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

		   	std::istringstream iss(line);

		   	int c = 0;
		   	int date;
		   	std::string name_page;
		   	int counter;

		    while (iss) {
		    	std::string sub;
		       	iss >> sub;
		       	//std::cout << sub << "\n";
		       	switch (c++) {
		       		case 0:
		       			date = date_to_id.find(sub)->second;
		       			break;
		       		case 1:
		       			name_page = sub;
		       			break;
		       		case 2: 
		       			counter = std::stoi(sub);
		       			break;
		       	}	       		 
		    }

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
  		std::sort(pages.find(it->first)->second.begin(), pages.find(it->first)->second.end(), sort_pred);
	}

  	// Serialize "pages" and "date_to_id"
	boost::archive::text_oarchive oarch1(dataset);
	oarch1 << pages;

	boost::archive::text_oarchive oarch2(date);
	oarch2 << date_to_id;
}