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
#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support.hpp>
#include <sdsl/rmq_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <sdsl/vectors.hpp>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "query.hpp"
#include "baseline.hpp"
#include "baseline1.hpp"
#include "baseline2.hpp"
#include "baseline3.hpp"
#include "baseline4.hpp"

int main(int argc, char* argv[]) {
    Query q;

	if (argc == 4) {
        switch (atoi(argv[1])) { // Choose witch implementation to use
            case 0: {
                Baseline b;

                b.load("\0", argv[2]); // Load file needed to query
                q.load("\0", argv[3]);

                // Get the range queries of the file .queries
                std::vector<std::vector<std::string>> range_queries = q.getRangeQueries();

              	// Measure avg time for making the function 'range'
              	std::vector<double> time_range_query;

                std::cout << "\n" << "============================================" << "\n\n";
                for (int i=0; i<range_queries.size(); i++) {
                    std::vector<uint64_t> res;      
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                    res = b.range(range_queries.at(i).at(0), range_queries.at(i).at(1), range_queries.at(i).at(2));

                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_range_query.push_back(time_span.count());
                    
                    // Print the average of the results
                    float avg = accumulate(res.begin(), res.end(), 0.0) / res.size();
                    std::cout << range_queries.at(i).at(0) << " (" << range_queries.at(i).at(1) << " / " << range_queries.at(i).at(2) << ") Mean: " << avg << std::endl;
                }

                // Print the average time to make a query
                float avg = accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                /*********************/

                // Get the top k queries of the file .queries
                std::vector<std::vector<std::string>> top_queries = q.getTopKQueries();

                // Measure avg time for making the function 'topk'
                std::vector<double> time_topk_query;

                for (int i=0; i<top_queries.size(); i++) {
                    std::vector<std::pair<uint64_t, uint32_t>> res;
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    
                    res = b.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
                    
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_topk_query.push_back(time_span.count());

                    // Print the k-th elements
                    std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
                }

                // Print the average time to make a query
                avg = accumulate(time_topk_query.begin(), time_topk_query.end(), 0.0) / time_topk_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                break;
            }
            case 1: {
                Baseline1 b1;

                b1.load("\0", argv[2]); // Load file needed to query
                q.load("\0", argv[3]);

                // Get the range queries of the file .queries
                std::vector<std::vector<std::string>> range_queries = q.getRangeQueries();

                // Measure avg time for making the function 'range'
                std::vector<double> time_range_query;

                std::cout << "\n" << "============================================" << "\n\n";
                for (int i=0; i<range_queries.size(); i++) {
                    std::vector<uint64_t> res;      
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                    res = b1.range(range_queries.at(i).at(0), range_queries.at(i).at(1), range_queries.at(i).at(2));

                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_range_query.push_back(time_span.count());
                    
                    // Print the average of the results
                    float avg = accumulate(res.begin(), res.end(), 0.0) / res.size();
                    std::cout << range_queries.at(i).at(0) << " (" << range_queries.at(i).at(1) << " / " << range_queries.at(i).at(2) << ") Mean: " << avg << std::endl;
                }

                // Print the average time to make a query
                float avg = accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                /*********************/

                // Get the top k queries of the file .queries
                std::vector<std::vector<std::string>> top_queries = q.getTopKQueries();

                // Measure avg time for making the function 'topk'
                std::vector<double> time_topk_query;

                for (int i=0; i<top_queries.size(); i++) {
                    std::vector<std::pair<uint64_t, uint32_t>> res;
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    
                    res = b1.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
                    
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_topk_query.push_back(time_span.count());

                    // Print the k-th elements
                    std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
                }

                // Print the average time to make a query
                avg = accumulate(time_topk_query.begin(), time_topk_query.end(), 0.0) / time_topk_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                break;
            }
            case 2: {
                Baseline2 b2;

                b2.load("\0", argv[2]); // Load file needed to query
                q.load("\0", argv[3]);

                // Get the range queries of the file .queries
                std::vector<std::vector<std::string>> range_queries = q.getRangeQueries();

                // Measure avg time for making the function 'range'
                std::vector<double> time_range_query;

                std::cout << "\n" << "============================================" << "\n\n";
                for (int i=0; i<range_queries.size(); i++) {
                    std::vector<uint64_t> res;      
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                    res = b2.range(range_queries.at(i).at(0), range_queries.at(i).at(1), range_queries.at(i).at(2));

                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_range_query.push_back(time_span.count());
                    
                    // Print the average of the results
                    float avg = accumulate(res.begin(), res.end(), 0.0) / res.size();
                    std::cout << range_queries.at(i).at(0) << " (" << range_queries.at(i).at(1) << " / " << range_queries.at(i).at(2) << ") Mean: " << avg << std::endl;
                }

                // Print the average time to make a query
                float avg = accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                /*********************/

                // Get the top k queries of the file .queries
                std::vector<std::vector<std::string>> top_queries = q.getTopKQueries();

                // Measure avg time for making the function 'topk'
                std::vector<double> time_topk_query;

                for (int i=0; i<top_queries.size(); i++) {
                    std::vector<std::pair<uint64_t, uint32_t>> res;
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    
                    res = b2.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
                    
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_topk_query.push_back(time_span.count());

                    // Print the k-th elements
                    std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
                }

                // Print the average time to make a query
                avg = accumulate(time_topk_query.begin(), time_topk_query.end(), 0.0) / time_topk_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                break;
            }
            case 3: {
                Baseline3 b3;

                b3.load("\0", argv[2]); // Load file needed to query
                q.load("\0", argv[3]);

                // Get the range queries of the file .queries
                std::vector<std::vector<std::string>> range_queries = q.getRangeQueries();

                // Measure avg time for making the function 'range'
                std::vector<double> time_range_query;

                std::cout << "\n" << "============================================" << "\n\n";
                for (int i=0; i<range_queries.size(); i++) {
                    std::vector<uint64_t> res;      
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                    res = b3.range(range_queries.at(i).at(0), range_queries.at(i).at(1), range_queries.at(i).at(2));

                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_range_query.push_back(time_span.count());
                    
                    // Print the average of the results
                    float avg = accumulate(res.begin(), res.end(), 0.0) / res.size();
                    std::cout << range_queries.at(i).at(0) << " (" << range_queries.at(i).at(1) << " / " << range_queries.at(i).at(2) << ") Mean: " << avg << std::endl;
                }

                // Print the average time to make a query
                float avg = accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                /*********************/

                // Get the top k queries of the file .queries
                std::vector<std::vector<std::string>> top_queries = q.getTopKQueries();

                // Measure avg time for making the function 'topk'
                std::vector<double> time_topk_query;

                for (int i=0; i<top_queries.size(); i++) {
                    std::vector<std::pair<uint64_t, uint32_t>> res;
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    
                    res = b3.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
                    
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_topk_query.push_back(time_span.count());

                    // Print the k-th elements
                    std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
                }

                // Print the average time to make a query
                avg = accumulate(time_topk_query.begin(), time_topk_query.end(), 0.0) / time_topk_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                break;
            }
            case 4: {
                Baseline4 b4;

                b4.load("\0", argv[2]); // Load file needed to query
                q.load("\0", argv[3]);

                // Get the range queries of the file .queries
                std::vector<std::vector<std::string>> range_queries = q.getRangeQueries();

                // Measure avg time for making the function 'range'
                std::vector<double> time_range_query;

                std::cout << "\n" << "============================================" << "\n\n";
                for (int i=0; i<range_queries.size(); i++) {
                    std::vector<uint64_t> res;      
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                    res = b4.range(range_queries.at(i).at(0), range_queries.at(i).at(1), range_queries.at(i).at(2));

                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_range_query.push_back(time_span.count());
                    
                    // Print the average of the results
                    float avg = accumulate(res.begin(), res.end(), 0.0) / res.size();
                    std::cout << range_queries.at(i).at(0) << " (" << range_queries.at(i).at(1) << " / " << range_queries.at(i).at(2) << ") Mean: " << avg << std::endl;
                }

                // Print the average time to make a query
                float avg = accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                /*********************/

                // Get the top k queries of the file .queries
                std::vector<std::vector<std::string>> top_queries = q.getTopKQueries();

                // Measure avg time for making the function 'topk'
                std::vector<double> time_topk_query;

                for (int i=0; i<top_queries.size(); i++) {
                    std::vector<std::pair<uint64_t, uint32_t>> res;
                    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
                    
                    res = b4.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
                    
                    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
                    time_topk_query.push_back(time_span.count());

                    // Print the k-th elements
                    std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
                }

                // Print the average time to make a query
                avg = accumulate(time_topk_query.begin(), time_topk_query.end(), 0.0) / time_topk_query.size();
                std::cout << "Average time to make queries: " << avg << " seconds." << std::endl;
                std::cout << "\n" << "============================================" << "\n\n";

                break;
            }
        }
    }
}