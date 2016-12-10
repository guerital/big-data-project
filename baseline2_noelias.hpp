// g++ -O3 -I ~/include -std=c++11 -o build_index build_index.cpp -lboost_serialization -lsdsl
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
#include <sdsl/sd_vector.hpp>
#include <sdsl/vectors.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

class Baseline2 {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::map<std::string, std::pair<uint64_t, std::vector<uint64_t>>> m_pages_serialize; // Data structure containing the index
	std::map<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>> m_pages;
	std::map<std::string, uint32_t> m_date2id; // Map structure to get an id from a date
	std::vector<std::vector<std::string>> m_rangeQueries; // Vector with all the range queries
	std::vector<std::vector<std::string>> m_topKQueries; // Vector with all the top k queries
	uint64_t size_sdsl;

	public:
		Baseline2() { }

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
						sdsl::bit_vector tmp_bit_vector(all_date.size());
						std::vector<uint64_t> tmp_counter(all_date.size());
						tmp_bit_vector[date] = 1;
						tmp_counter[date] = counter;
						m_pages.insert(std::pair<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>>(name_page, std::pair<sdsl::bit_vector, std::vector<uint64_t>>(tmp_bit_vector, tmp_counter)));
					} else {
						m_pages.find(name_page)->second.first[date] = 1;
						m_pages.find(name_page)->second.second[date] += counter;
					}
				}
				m_series.close();
			}

			for(std::map<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>>::iterator it = m_pages.begin(); it != m_pages.end(); ++it) {
				for (int i=0; i<it->second.second.size(); i++) {
					if (it->second.second[i]==0) {
						it->second.second.erase(it->second.second.begin() + i);
						i--;
					}
				}

				//sdsl::sd_vector<> tmp_bit_vector(it->second.first);
				//m_pages.insert(std::pair<std::string, std::pair<sdsl::sd_vector<>, std::vector<uint64_t>>>(it->first, std::pair<sdsl::sd_vector<>, std::vector<uint64_t>>(tmp_bit_vector, it->second.second)));
			}
		}

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& dataset_filename, const std::string& queries_filename) {
        	m_series.open(series_filename);

			std::ifstream dataset;
        	dataset.open(dataset_filename);

          	if (dataset.is_open()) {
          		boost::archive::text_iarchive iarch2(dataset);
           		iarch2 >> m_pages_serialize;
                iarch2 >> m_date2id;

                sdsl::bit_vector tmp_bit_vector;
                for(std::map<std::string, std::pair<uint64_t, std::vector<uint64_t>>>::iterator it = m_pages_serialize.begin(); it != m_pages_serialize.end(); ++it) {
                	sdsl::load_from_file(tmp_bit_vector, "sdsl/" + std::to_string(it->second.first) + ".sdsl");
                	m_pages.insert(std::pair<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>>(it->first, std::pair<sdsl::bit_vector, std::vector<uint64_t>>(tmp_bit_vector, it->second.second)));
                }
        	
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
				// transform m_pages in m_pages_serialize
				int id_sdsl = 0;
				size_sdsl = 0;
				for(std::map<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>>::iterator it = m_pages.begin(); it != m_pages.end(); ++it) {
					m_pages_serialize.insert(std::pair<std::string, std::pair<uint64_t, std::vector<uint64_t>>>(it->first, std::pair<uint64_t, std::vector<uint64_t>>(id_sdsl, it->second.second)));
					sdsl::store_to_file(it->second.first, "sdsl/" + std::to_string(id_sdsl) + ".sdsl");

					std::ifstream in("sdsl/" + std::to_string(id_sdsl) + ".sdsl", std::ifstream::ate | std::ifstream::binary);
					size_sdsl += in.tellg();

					id_sdsl++;
				}

				boost::archive::text_oarchive oarch1(dataset);
				oarch1 << m_pages_serialize;
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
			
			return (size_sdsl + in.tellg()) / 1000000;
		}

		// Function that compute the range query
		inline std::vector<uint64_t> range(const std::string& q_name_page, const std::string& date1, const std::string& date2) const {
		    const sdsl::bit_vector & tmp_bit_vector = std::ref(m_pages.find(q_name_page)->second.first);
		    const std::vector<uint64_t> & tmp_counter = std::ref(m_pages.find(q_name_page)->second.second);
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

		    std::vector<uint64_t> v; 

		    sdsl::rank_support_v<1> tmp_rank(&tmp_bit_vector);

		    for (int i=left_date; i<=right_date; i++) {          
		        if (tmp_bit_vector[i]==0)
		        	v.push_back(0);
		        else
		        	v.push_back(tmp_counter[tmp_rank(i)]);   
		    }

		    return v;
		}

		// Function that compute the top k query
		inline std::priority_queue<std::pair<uint64_t, uint32_t>, std::vector<std::pair<uint64_t, uint32_t>>, std::greater<std::pair<uint64_t, uint32_t>>> topKRange(const std::string& q_name_page, const std::string& date1, const std::string& date2, const int& k) const {
			const sdsl::bit_vector & tmp_bit_vector = std::ref(m_pages.find(q_name_page)->second.first);
			const std::vector<uint64_t> & tmp_counter = std::ref(m_pages.find(q_name_page)->second.second);
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

			std::priority_queue<std::pair<uint64_t, uint32_t>, std::vector<std::pair<uint64_t, uint32_t>>, std::greater<std::pair<uint64_t, uint32_t>>> min_heap;

			sdsl::rank_support_v<1> tmp_rank(&tmp_bit_vector);
		    for (int i=left_date; i<=right_date; i++) {
		    	if (tmp_bit_vector[i]==0) {
		    		if (min_heap.size()!=k) min_heap.push(std::pair<uint64_t, uint32_t>(0, i));	
		    	} else  {
	    			if (min_heap.size()==k) {
		    			if (min_heap.top().first<tmp_counter[tmp_rank(i)]) {
			    			min_heap.pop();
			    			min_heap.push(std::pair<uint64_t, uint32_t>(tmp_counter[tmp_rank(i)], i));
			    		}
			    	} else {
			    		min_heap.push(std::pair<uint64_t, uint32_t>(tmp_counter[tmp_rank(i)], i));
			    	}   
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