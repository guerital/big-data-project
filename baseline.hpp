#pragma once

class Baseline {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::map<std::string, std::vector<std::pair<uint32_t, uint64_t>>> m_pages; // Data structure containing the index
	std::map<std::string, uint32_t> m_date2id; // Map structure to get an id from a date

	public:
		Baseline() { }

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
				std::sort(	it->second.begin(), 
							it->second.end(), 
							[](const std::pair<uint32_t, uint64_t> & a, const std::pair<uint32_t, uint64_t> & b) { 
			  					return a.first<b.first; 
			  				}
						);
			}

			// Fix bugged value of wikipedia (example page Marxism). Costly in term of time but reduce the space of the file
			for(std::map<std::string, std::vector<std::pair<uint32_t, uint64_t>>>::iterator it = m_pages.begin(); it != m_pages.end(); ++it) {
				for (int i=0; i<it->second.size()-1; i++) {
					if (it->second.at(i).first == it->second.at(i+1).first) {
						it->second.at(i).second += it->second.at(i+1).second;
						it->second.erase(it->second.begin() + i+1);
						i--;
					}
				}
			}
		}

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& dataset_filename) {
        	m_series.open(series_filename);

			std::ifstream dataset;
        	dataset.open(dataset_filename);

          	if (dataset.is_open()) {
          		boost::archive::text_iarchive iarch2(dataset);
           		iarch2 >> m_pages;
                iarch2 >> m_date2id;
        	
        	    dataset.close();
        	}
		}

		// Serialize the needed file. If a file not wanted to be serialize pass a name "\O"
		void serialize(const std::string& dataset_filename) {
			std::ofstream dataset;
			dataset.open(dataset_filename);

			if (dataset.is_open()) {
				boost::archive::text_oarchive oarch1(dataset);
				oarch1 << m_pages;
				oarch1 << m_date2id;
			}
		}

		// Calculate the dimension of a file
		double size(const std::string& filename) {
			std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
			
			return in.tellg() / 1000000;
		}

		// Function that compute the range query
		inline std::vector<uint64_t> range(const std::string& q_name_page, const std::string& date1, const std::string& date2) const {
		    const std::vector<std::pair<uint32_t, uint64_t>>& tmp_vect = std::ref(m_pages.find(q_name_page)->second);
		    const int tmp_vect_size = tmp_vect.size();
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

		    std::vector<uint64_t> v; 

  			int j = (int) ( std::lower_bound(tmp_vect.begin(), 
			  				tmp_vect.end(), 
			  				std::pair<uint32_t, uint64_t>(left_date,0),
			  				[](const std::pair<uint32_t, uint64_t> & a, const std::pair<uint32_t, uint64_t> & b) { 
			  					return a.first<b.first; 
			  				}) 
					- tmp_vect.begin()); 
		    
		    if (j==tmp_vect_size) j=0;

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
		inline std::vector<std::pair<uint64_t, uint32_t>> topKRange(const std::string& q_name_page, const std::string& date1, const std::string& date2, const int& k) const {
			const std::vector<std::pair<uint32_t, uint64_t>>& tmp_vect = std::ref(m_pages.find(q_name_page)->second);
			const int tmp_vect_size = tmp_vect.size();
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

			std::priority_queue<std::pair<uint64_t, uint32_t>, std::vector<std::pair<uint64_t, uint32_t>>, std::greater<std::pair<uint64_t, uint32_t>>> min_heap;

  			int j = (int) ( std::lower_bound(tmp_vect.begin(), 
			  				tmp_vect.end(), 
			  				std::pair<uint32_t, uint64_t>(left_date,0),
			  				[](const std::pair<uint32_t, uint64_t> & a, const std::pair<uint32_t, uint64_t> & b) { 
			  					return a.first<b.first; 
			  				}) 
					- tmp_vect.begin()); 
		    
		    if (j==tmp_vect_size) j=0;

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

		    std::vector<std::pair<uint64_t, uint32_t>> v; 
		    while (!min_heap.empty()) {
		        v.push_back(std::pair<uint64_t, uint32_t>(min_heap.top().first, min_heap.top().second)); 
		        min_heap.pop();
		    }

		    std::sort(v.begin(), v.end(), [](const std::pair<uint64_t, uint32_t> & a, const std::pair<uint64_t, uint32_t> & b) { 
			  					return a.first>b.first; 
			  				});

		    return v;   
		}		
};