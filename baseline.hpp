#pragma once

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
#include <utility>
#include <vector>

class baseline {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::map<std::string, std::vector<std::pair<uint32_t, uint64_t>>> m_pages; // Data structure containing the index
	std::map<std::string, uint32_t> m_date2id; // Map structure to get an id from a date
	std::vector<std::vector<std::string>> m_rangeQueries; // Vector with all the range queries
	std::vector<std::vector<std::string>> m_topKQueries; // Vector with all the top k queries

	public:
		baseline() { }

		// Function that build the index
		void buildIndex() {
			std::string line;

			// Scan the dataset to search all the date in the file 'series.txt'
			std::vector<std::string> all_date;
			if (m_series.is_open()) {
				while (std::getline(m_series,line)) {
					std::stringstream linestream(line);
					std::string data;

					std::getline(linestream, data, '\t'); 
					all_date.push_back(data);
				}
			}

			// Remove duplicate date
			std::sort(all_date.begin(), all_date.end());
			all_date.erase(std::unique(all_date.begin(), all_date.end()), all_date.end());

			// Creating hash "m_date2id", associating a unique id to each date
			int idDate=0;
			for (auto i=all_date.begin(); i<all_date.end(); i++) {
				m_date2id.insert(std::pair<std::string, int>(*i, idDate++));	
			}

			// Restart the pointer of the file "m_series"
			m_series.clear();
			m_series.seekg(0, std::ios::beg);

			// Creation of index "m_pages"
			if (m_series.is_open()) {
				while (std::getline(m_series,line)) {
					std::stringstream linestream(line);
					std::string data;

					std::getline(linestream, data, '\t'); 
					uint32_t date = boost::lexical_cast<int>(m_date2id.find(data)->second);
					std::getline(linestream, data, '\t'); 
					std::string name_page = data;
					std::getline(linestream, data, '\t'); 
					uint64_t counter = boost::lexical_cast<int>(data);

					// Add the new data, if there is no list for that page it create a new one
					if (m_pages.find(name_page)==m_pages.end()) {
						std::vector<std::pair<uint32_t, uint64_t>> tmp;
						tmp.push_back(std::pair<uint32_t, uint64_t>(date, counter));
						m_pages.insert(std::pair<std::string, std::vector<std::pair<uint32_t, uint64_t>>>(name_page, tmp));
					} else {
						m_pages.find(name_page)->second.push_back(std::pair<uint32_t, uint64_t>(date, counter));
					}
				}
				m_series.close();
			}

			// Sorts all the lists of the data structure "m_pages" by date id
			for(std::map<std::string, std::vector<std::pair<uint32_t, uint64_t>>>::iterator it = m_pages.begin(); it != m_pages.end(); ++it) {
				std::sort(m_pages.find(it->first)->second.begin(), m_pages.find(it->first)->second.end());
			}
		}

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& dataset_filename, const std::string& queries_filename) {
        	m_series.open(series_filename);

			std::ifstream dataset;
        	dataset.open(dataset_filename);

          	if (dataset.is_open()) {
          		boost::archive::text_iarchive iarch2(dataset);
           		iarch2 >> m_pages;
                iarch2 >> m_date2id;
        	
        	    dataset.close();
        	}

        	std::ifstream queries;
            queries.open(queries_filename);

            if (queries.is_open()) {
                boost::archive::text_iarchive iarch3(queries);
                iarch3 >> m_rangeQueries;
                iarch3 >> m_topKQueries;

                queries.close();
            }
		}

		// Serialize the needed file. If a file not wanted to be serialize pass a name "\O"
		void serialize(const std::string& dataset_filename, const std::string& query_filename) {
			std::ofstream dataset;
			dataset.open(dataset_filename);

			if (dataset.is_open()) {
				boost::archive::text_oarchive oarch1(dataset);
				oarch1 << m_pages;
				oarch1 << m_date2id;
			}

			std::ofstream queries;
			queries.open(query_filename);

			if (queries.is_open()) {
				boost::archive::text_oarchive oarch2(queries);
				oarch2 << m_rangeQueries;
				oarch2 << m_topKQueries;
			}
		}

		// Calculate the dimension of a file
		int size(const std::string& filename) {
			std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
			
			return in.tellg() / 1000000;
		}

		// Function that generate queries at random
		void generateQueries(int num_query) {
			std::string line;

			// Scan the file to get all the date and all the name of the pages
			std::vector<std::string> all_date;
			std::vector<std::string> all_name_page;
			if (m_series.is_open()) {
				while (std::getline(m_series,line)) {
					std::stringstream linestream(line);
					std::string data;

					std::getline(linestream, data, '\t'); 
					all_date.push_back(data);
					std::getline(linestream, data, '\t'); 
					all_name_page.push_back(data);
				}
			}

			// Remove duplicates
			std::sort(all_date.begin(), all_date.end());
			all_date.erase(std::unique(all_date.begin(), all_date.end()), all_date.end());

			std::sort(all_name_page.begin(), all_name_page.end());
			all_name_page.erase(std::unique(all_name_page.begin(), all_name_page.end()), all_name_page.end());

			// Small, Medium and Big query
			int range_date[] = {500, 2500, 5000};
			for (int i=0; i<num_query/3; i++) {
				for (int j=0; j<3; j++) {
					std::vector<std::string> tmp_range;
					std::vector<std::string> tmp_top;

					// Generate random name page
					int random_name_page = rand() % all_name_page.size();
					tmp_range.push_back(all_name_page.at(random_name_page));
					tmp_top.push_back(all_name_page.at(random_name_page));

					// Generate random date
					int random_date = rand() % (all_date.size()-range_date[j]);
					tmp_range.push_back(all_date.at(random_date));
					tmp_range.push_back(all_date.at(random_date+range_date[j]));
					tmp_top.push_back(all_date.at(random_date));
					tmp_top.push_back(all_date.at(random_date+range_date[j]));

					// Generate random k
					int k = rand() % 10 + 1;
					tmp_top.push_back(std::to_string(k));

					m_rangeQueries.push_back(tmp_range);
					m_topKQueries.push_back(tmp_top);
				}
			}
		}

		// Function that return a searched element or the small greatest one
		inline int binary_search(const std::vector<std::pair<uint32_t, uint64_t>>& v, int k) const {
			if (v.size()==1)
				return 0; 

			int first = 0, last = v.size()-1;
			while (first <= last) {
			    int mid = (first + last) / 2;
			    if (v[mid].first == k)
			    	return mid;
			    if (v[mid].first > k)
			      	last = mid - 1;
			    else
			    	first = mid + 1;
			}
			
			if (last<v.size()-1 and v[last].first<k) last++;
			return last==-1 ? 0 : last;
		}

		// Function that compute the range query
		inline std::vector<uint64_t> range(const std::string& q_name_page, const std::string& date1, const std::string& date2) const {
		    const std::vector<std::pair<uint32_t, uint64_t>>& tmp_vect = std::ref(m_pages.find(q_name_page)->second);
		    const int tmp_vect_size = tmp_vect.size();
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

		    std::vector<uint64_t> v; 

		    // Da cambiare con binary
		    /*int h=0;
		    for (int l=0; l<tmp_vect_size; l++) {
		        if (tmp_vect.at(l).first >= left_date) {
		            h=l;
		            break;
		        }
		    }*/

			int j = binary_search(tmp_vect, left_date);
		    
		    for (int i=left_date; i<=right_date; i++) {          
		        if (i != tmp_vect.at(j).first) {
		            v.push_back(0);
		    	} else {
		       		v.push_back(tmp_vect.at(j).second);
		       	    if (j < tmp_vect_size-1) j++;   
		        }         
		    }

		    return v;
		}

		// Function that compute the top k query
		inline std::priority_queue<std::pair<uint64_t, uint32_t>, std::vector<std::pair<uint64_t, uint32_t>>, std::greater<std::pair<uint64_t, uint32_t>>> topKRange(const std::string& q_name_page, const std::string& date1, const std::string& date2, const int& k) const {
			const std::vector<std::pair<uint32_t, uint64_t>>& tmp_vect = std::ref(m_pages.find(q_name_page)->second);
			const int tmp_vect_size = tmp_vect.size();
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

			std::priority_queue<std::pair<uint64_t, uint32_t>, std::vector<std::pair<uint64_t, uint32_t>>, std::greater<std::pair<uint64_t, uint32_t>>> min_heap;

		    int j=0;
		    for (int l=0; l<tmp_vect_size; l++) {
		        if (tmp_vect.at(l).first >= left_date) {
		            j=l;
		            break;
		        }
		    }

		    for (int i=left_date; i<=right_date; i++) {       
		    	if (i!=tmp_vect.at(j).first) {
		    		if (min_heap.size()!=k) 
		    			min_heap.push(std::pair<uint64_t, uint32_t>(0, i));
		    	} else {
		    		if (min_heap.size()==k) {
		    			if (min_heap.top().first<tmp_vect.at(j).second) {
			    			min_heap.pop();
			    			min_heap.push(std::pair<uint64_t, uint32_t>(tmp_vect.at(j).second, i));
			    		}
			    	} else {
			    		min_heap.push(std::pair<uint64_t, uint32_t>(tmp_vect.at(j).second, i));
			    	}
			    	if (j<tmp_vect_size-1) j++;
		       	}    
		    }

		    return min_heap;  
		}

		std::vector<std::vector<std::string>> getRangeQueries() {
			return m_rangeQueries;
		}

		std::vector<std::vector<std::string>> getTopKQueries() {
			return m_topKQueries;
		}		
};