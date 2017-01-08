#pragma once

class Query {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::vector<std::vector<std::string>> m_rangeQueries; // Vector with all the range queries
	std::vector<std::vector<std::string>> m_topKQueries; // Vector with all the top k queries

	public:
		Query() { }

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& queries_filename) {
        	m_series.open(series_filename);

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
		void serialize(const std::string& query_filename) {
			std::ofstream queries;
			queries.open(query_filename);

			if (queries.is_open()) {
				boost::archive::text_oarchive oarch2(queries);
				oarch2 << m_rangeQueries;
				oarch2 << m_topKQueries;
			}
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
					int k = rand() % 3;
					switch (k) { 
						case 0: 
							k=5;
							break;
						case 1:
							k=10;
							break;
						case 2:
							k=20;
							break;
					}
					tmp_top.push_back(std::to_string(k));

					m_rangeQueries.push_back(tmp_range);
					m_topKQueries.push_back(tmp_top);
				}
			}
		}

		std::vector<std::vector<std::string>> getRangeQueries() {
			return m_rangeQueries;
		}

		std::vector<std::vector<std::string>> getTopKQueries() {
			return m_topKQueries;
		}		
};