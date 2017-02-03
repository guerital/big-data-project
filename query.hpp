#pragma once

class Query {
	std::ifstream m_series; // ifstream to open file 'series.txt'
	std::vector<std::vector<std::string>> m_rangeQueries_b; // Vector with big range queries
	std::vector<std::vector<std::string>> m_topKQueries_b; // Vector with big top k queries
	std::vector<std::vector<std::string>> m_rangeQueries_m; // Vector with medium range queries
	std::vector<std::vector<std::string>> m_topKQueries_m; // Vector with medium top k queries
	std::vector<std::vector<std::string>> m_rangeQueries_s; // Vector with small range queries
	std::vector<std::vector<std::string>> m_topKQueries_s; // Vector with small top k queries

	public:
		Query() { }

		// Load the needed file. If a file not needed pass a name "\O"
		void load(const std::string& series_filename, const std::string& query_filename_b, const std::string& query_filename_m, const std::string& query_filename_s) {
        	m_series.open(series_filename);

        	std::ifstream queries_b;
            queries_b.open(query_filename_b);

            if (queries_b.is_open()) {
                boost::archive::text_iarchive iarch1(queries_b);
                iarch1 >> m_rangeQueries_b;
                iarch1 >> m_topKQueries_b;

                queries_b.close();
            }

            std::ifstream queries_m;
            queries_m.open(query_filename_m);

            if (queries_m.is_open()) {
                boost::archive::text_iarchive iarch2(queries_m);
                iarch2 >> m_rangeQueries_m;
                iarch2 >> m_topKQueries_m;

                queries_m.close();
            }

            std::ifstream queries_s;
            queries_s.open(query_filename_s);

            if (queries_s.is_open()) {
                boost::archive::text_iarchive iarch3(queries_s);
                iarch3 >> m_rangeQueries_s;
                iarch3 >> m_topKQueries_s;

                queries_s.close();
            }
		}

		// Serialize the needed file. If a file not wanted to be serialize pass a name "\O"
		void serialize(const std::string& query_filename_b, const std::string& query_filename_m, const std::string& query_filename_s) {
			std::ofstream queries_b;
			queries_b.open(query_filename_b);

			if (queries_b.is_open()) {
				boost::archive::text_oarchive oarch1(queries_b);
				oarch1 << m_rangeQueries_b;
				oarch1 << m_topKQueries_b;
			}

			std::ofstream queries_m;
			queries_m.open(query_filename_m);

			if (queries_m.is_open()) {
				boost::archive::text_oarchive oarch2(queries_m);
				oarch2 << m_rangeQueries_m;
				oarch2 << m_topKQueries_m;
			}

			std::ofstream queries_s;
			queries_s.open(query_filename_s);

			if (queries_s.is_open()) {
				boost::archive::text_oarchive oarch3(queries_s);
				oarch3 << m_rangeQueries_s;
				oarch3 << m_topKQueries_s;
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
			int range_date[] = {5000, 2500, 500};
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

					switch (j) { 
						case 0: 
							m_rangeQueries_b.push_back(tmp_range);
							m_topKQueries_b.push_back(tmp_top);
							break;
						case 1:
							m_rangeQueries_m.push_back(tmp_range);
							m_topKQueries_m.push_back(tmp_top);
							break;
						case 2:
							m_rangeQueries_s.push_back(tmp_range);
							m_topKQueries_s.push_back(tmp_top);
							break;
					}
				}
			}
		}

		std::vector<std::vector<std::string>> getRangeQueriesB() { return m_rangeQueries_b; }
		std::vector<std::vector<std::string>> getTopKQueriesB() { return m_topKQueries_b; }

		std::vector<std::vector<std::string>> getRangeQueriesM() { return m_rangeQueries_m; }
		std::vector<std::vector<std::string>> getTopKQueriesM() { return m_topKQueries_m; }		

		std::vector<std::vector<std::string>> getRangeQueriesS() { return m_rangeQueries_s; }
		std::vector<std::vector<std::string>> getTopKQueriesS() { return m_topKQueries_s; }				
};