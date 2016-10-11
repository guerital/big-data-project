#include <iostream> 
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <queue>
#include <utility>
#include <iomanip>

#include "boost/serialization/map.hpp"
#include "boost/serialization/vector.hpp"
#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"

// Data structure containing the dataset
std::map<std::string, std::vector<std::pair<int, int>>> pages;

// Map structure to sort date on int and not on string
std::map<std::string, int> date_to_id;

void range(std::string q_name_page, std::string date1, std::string date2) {
	std::vector<std::pair<int, int>> tmp_vect = pages.find(q_name_page)->second;

    int j=0;
    for (int i=date_to_id.find(date1)->second; i<=date_to_id.find(date2)->second; i++) {
    	if (i!=tmp_vect[j].first) {
    		std::cout << " 0\n";
    		i++;
    	} else 
       		std::cout << tmp_vect[j].second << "\n";
       	j++;
        std::cout << i << "\n";
    }
}

void topKRange(std::string q_name_page, std::string date1, std::string date2, int k) {
	std::priority_queue<std::pair<int, std::string>, std::vector<std::pair<int, std::string>>, std::greater<std::pair<int, std::string>>> min_heap;
	std::vector<std::pair<int, int>> tmp_vect = pages.find(q_name_page)->second;

    int j=0;
    for (auto it = date_to_id.find(date1); it != date_to_id.find(date2); ++it) {
    	if (it->second!=tmp_vect[j].first) {
    		if (min_heap.size()!=k) 
    			min_heap.push(std::pair<int,std::string>(0, it->first));
    		++it;
    	} else {
    		if (min_heap.size()==k) {
    			if (min_heap.top().first<tmp_vect[j].second) {
	    			min_heap.pop();
	    			min_heap.push(std::pair<int,std::string>(tmp_vect[j].second, it->first));
	    		}
	    	} else {
	    		min_heap.push(std::pair<int,std::string>(tmp_vect[j].second, it->first));
	    	}
       	}
       	j++;    
    }

    while(!min_heap.empty()) {
        std::cout << min_heap.top().first << " " << min_heap.top().second << "\n";
        min_heap.pop();
    }  
}

int main() {
	// File needed
	std::ifstream date;
	date.open("date.txt");
	std::ifstream dataset;
	dataset.open("dataset.txt");
	std::string line;

	// Get the need data structure
  	if (dataset.is_open()) {
  		boost::archive::text_iarchive iarch1(dataset);
   		iarch1 >> pages;
	
	    dataset.close();
	} else std::cout << "Unable to open file";

	if (date.is_open()) {
		boost::archive::text_iarchive iarch2(date);
   		iarch2 >> date_to_id;

	    date.close();
	} else std::cout << "Unable to open file";

  	// Try the functions "range" and "topKRange"
  	range("2014_FIFA_World_Cup", "20160101-00", "20160102-00");

  	std::cout << "\n";

  	topKRange("2014_FIFA_World_Cup", "20160101-00", "20160102-00", 3);
}