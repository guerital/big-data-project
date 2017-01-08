#pragma once

class Baseline4 {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::map<std::string, uint64_t> m_pages_serialize; // Data structure containing the index
	std::map<std::string, std::tuple<sdsl::bit_vector, sdsl::sd_vector<>, sdsl::rmq_succinct_sct<0>>> m_pages;
	std::map<std::string, uint32_t> m_date2id; // Map structure to get an id from a date
	uint64_t size_sdsl;

	public:
		Baseline4() { }

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

			// Creation of index "tmp_pages"
			std::map<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>> tmp_pages;

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
					if (tmp_pages.find(name_page)==tmp_pages.end()) {
						sdsl::bit_vector tmp_bit_vector(all_date.size());
						std::vector<uint64_t> tmp_counter(all_date.size());
						tmp_bit_vector[date] = 1;
						tmp_counter[date] = counter;
						tmp_pages.insert(std::pair<std::string, std::pair<sdsl::bit_vector, std::vector<uint64_t>>>(name_page, std::pair<sdsl::bit_vector, std::vector<uint64_t>>(tmp_bit_vector, tmp_counter)));
					} else {
						tmp_pages.find(name_page)->second.first[date] = 1;
						tmp_pages.find(name_page)->second.second[date] += counter;
					}
				}
				m_series.close();
			}

			for(auto it = tmp_pages.begin(); it != tmp_pages.end(); ++it) {
				for (int i=0; i<it->second.second.size(); i++) {
					if (it->second.second[i]==0) {
						it->second.second.erase(it->second.second.begin() + i);
						i--;
					} 
				}

				sdsl::rmq_succinct_sct<0> rmq;
				rmq = sdsl::rmq_succinct_sct<0>(&it->second.second);

				for (int i=1; i<it->second.second.size(); i++) {
					it->second.second[i]+=it->second.second[i-1];		
				}

				sdsl::sd_vector<> tmp_vector(it->second.second.begin(), it->second.second.end());
				m_pages.insert(std::pair<std::string, std::tuple<sdsl::bit_vector, sdsl::sd_vector<>, sdsl::rmq_succinct_sct<0>>>(it->first, std::tuple<sdsl::bit_vector, sdsl::sd_vector<>, sdsl::rmq_succinct_sct<0>>(it->second.first, tmp_vector, rmq)));
			}
		}

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& dataset_filename) {
        	m_series.open(series_filename);

			std::ifstream dataset;
        	dataset.open(dataset_filename);

          	if (dataset.is_open()) {
          		boost::archive::text_iarchive iarch2(dataset);
           		iarch2 >> m_pages_serialize;
                iarch2 >> m_date2id;

                sdsl::bit_vector tmp_bit_vector;
                sdsl::sd_vector<> tmp_vector;
                sdsl::rmq_succinct_sct<0> rmq;
                for(auto it = m_pages_serialize.begin(); it != m_pages_serialize.end(); ++it) {
                	sdsl::load_from_file(tmp_bit_vector, "sdsl-4/" + std::to_string(it->second) + "a.sdsl");
                	sdsl::load_from_file(tmp_vector, "sdsl-4/" + std::to_string(it->second) + "b.sdsl");
                	sdsl::load_from_file(rmq, "sdsl-4/" + std::to_string(it->second) + "c.sdsl");
                	m_pages.insert(std::pair<std::string, std::tuple<sdsl::bit_vector, sdsl::sd_vector<>, sdsl::rmq_succinct_sct<0>>>(it->first, std::tuple<sdsl::bit_vector, sdsl::sd_vector<>, sdsl::rmq_succinct_sct<0>>(tmp_bit_vector, tmp_vector, rmq)));
                }
        	
        	    dataset.close();
        	}
		}

		// Serialize the needed file. If a file not wanted to be serialize pass a name "\O"
		void serialize(const std::string& dataset_filename) {
			std::ofstream dataset;
			dataset.open(dataset_filename);

			if (dataset.is_open()) {
				// transform m_pages in m_pages_serialize
				int id_sdsl = 0;
				size_sdsl = 0;
				for(auto it = m_pages.begin(); it != m_pages.end(); ++it) {
					m_pages_serialize.insert(std::pair<std::string, uint64_t>(it->first, id_sdsl));
					sdsl::store_to_file(std::get<0>(it->second), "sdsl-4/" + std::to_string(id_sdsl) + "a.sdsl");
					sdsl::store_to_file(std::get<1>(it->second), "sdsl-4/" + std::to_string(id_sdsl) + "b.sdsl");
					sdsl::store_to_file(std::get<2>(it->second), "sdsl-4/" + std::to_string(id_sdsl) + "c.sdsl");

					std::ifstream in1("sdsl-4/" + std::to_string(id_sdsl) + "a.sdsl", std::ifstream::ate | std::ifstream::binary);
					std::ifstream in2("sdsl-4/" + std::to_string(id_sdsl) + "b.sdsl", std::ifstream::ate | std::ifstream::binary);
					std::ifstream in3("sdsl-4/" + std::to_string(id_sdsl) + "c.sdsl", std::ifstream::ate | std::ifstream::binary);
					size_sdsl += in1.tellg() + in2.tellg() + in3.tellg();

					id_sdsl++;
				}

				boost::archive::text_oarchive oarch1(dataset);
				oarch1 << m_pages_serialize;
				oarch1 << m_date2id;
			}
		}

		// Calculate the dimension of a file
		int size(const std::string& filename) {
			std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
			
			return (size_sdsl + in.tellg()) / 1000000;
		}

		// Function that compute the range query
		inline std::vector<uint64_t> range(const std::string& q_name_page, const std::string& date1, const std::string& date2) const {
		    const sdsl::bit_vector & tmp_bit_vector = std::ref(std::get<0>(m_pages.find(q_name_page)->second));
		    const sdsl::sd_vector<> & tmp_counter = std::ref(std::get<1>(m_pages.find(q_name_page)->second));
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

		    std::vector<uint64_t> v; 

		    sdsl::rank_support_v<1> tmp_rank(&tmp_bit_vector);
		    sdsl::sd_vector<>::select_1_type tmp_select(&tmp_counter);

		    for (int i=left_date; i<=right_date; i++) {          
		        if (tmp_bit_vector[i]==0)
		        	v.push_back(0);
		        else if (tmp_rank(i)!=0)
		        	v.push_back(tmp_select(tmp_rank(i)+1)-tmp_select(tmp_rank(i)));	
		        else
		        	v.push_back(tmp_select(tmp_rank(i)+1));  
		    }

		    return v;
		}

		struct weight_interval{
		    uint64_t w;
		    size_t idx, lb, rb;
		    weight_interval(uint64_t f_w, size_t f_idx, size_t f_lb, size_t f_rb) : 
		        w(f_w), idx(f_idx), lb(f_lb), rb(f_rb) {}

		    bool operator<(const weight_interval& wi) const {
		        return std::tie(w, idx, lb, rb) < std::tie(wi.w, wi.idx, wi.lb, wi.rb);
		    }
		};

		inline std::vector<std::pair<uint64_t, uint32_t>> heaviest_indexes_in_range(size_t k, uint64_t i, uint64_t j, const sdsl::sd_vector<> & w, const sdsl::rmq_succinct_sct<0> & rmq) const {
		    sdsl::sd_vector<>::select_1_type tmp_select(&w);

		    std::priority_queue<weight_interval> pq;
		    auto push_interval = [&](size_t f_lb, size_t f_rb) {
		        if ( f_rb > f_lb ) {
		            size_t max_idx = rmq(f_lb, f_rb-1);
		            
		            auto tmp = tmp_select(max_idx+1);
		            if (max_idx>0) tmp -= tmp_select(max_idx);
		            
		            pq.push(weight_interval(tmp, max_idx, f_lb, f_rb));
		        }
		    };

		    std::vector<std::pair<uint64_t, uint32_t>> res;
		    push_interval(i, j);
		    while ( res.size() < k and !pq.empty() ) {
		        auto iv = pq.top(); pq.pop();

		        auto tmp = tmp_select(iv.idx+1);
		        if (iv.idx>0) tmp -= tmp_select(iv.idx);

		        res.push_back(std::pair<uint64_t, uint32_t>(tmp, iv.idx+1));
		        push_interval(iv.lb, iv.idx);
		        push_interval(iv.idx+1, iv.rb);
		    }

		    while (res.size()<k)
				res.push_back(std::pair<uint64_t, uint32_t>(0,0));
		    return res; 
		}

		// Function that compute the top k query
		inline std::vector<std::pair<uint64_t, uint32_t>> topKRange(const std::string& q_name_page, const std::string& date1, const std::string& date2, const int& k) const {
			const sdsl::bit_vector & tmp_bit_vector = std::ref(std::get<0>(m_pages.find(q_name_page)->second));
		    const sdsl::sd_vector<> & tmp_counter = std::ref(std::get<1>(m_pages.find(q_name_page)->second));
		    const sdsl::rmq_succinct_sct<0> & rmq = std::ref(std::get<2>(m_pages.find(q_name_page)->second));
		    const int left_date = m_date2id.find(date1)->second;
		    const int right_date = m_date2id.find(date2)->second;

			sdsl::rank_support_v<1> tmp_rank(&tmp_bit_vector);

			if (tmp_rank(left_date)!=tmp_rank(right_date)) 
				return heaviest_indexes_in_range(k, tmp_rank(left_date), tmp_rank(right_date), tmp_counter, rmq);
	
			
			std::vector<std::pair<uint64_t, uint32_t>> res;
			while (res.size()<k)
				res.push_back(std::pair<uint64_t, uint32_t>(0,0));
			return res;
		}		
};