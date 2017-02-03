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

template<typename BaselineNumber>
float executeRangeQueries(BaselineNumber& b, std::vector<std::vector<std::string>> range_queries) {
    // Measure avg time for executing the function 'range'
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
    
    std::cout << "\n" << "============================================" << "\n\n";

    // Print the average time to make a query
    return accumulate(time_range_query.begin(), time_range_query.end(), 0.0) / time_range_query.size();
}

template<typename BaselineNumber>
std::tuple<float,float,float> executeTopKQueries(BaselineNumber& b, std::vector<std::vector<std::string>> top_queries) {
    // Measure avg time for making the function 'topk'
    std::vector<double> time_topk_query_5;
    std::vector<double> time_topk_query_10;
    std::vector<double> time_topk_query_20;

    std::cout << "\n" << "============================================" << "\n\n";
    for (int i=0; i<top_queries.size(); i++) {
        std::vector<std::pair<uint64_t, uint32_t>> res;
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        
        res = b.topKRange(top_queries.at(i).at(0), top_queries.at(i).at(1), top_queries.at(i).at(2), stoi(top_queries.at(i).at(3)));
        
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
        
        switch (stoi(top_queries.at(i).at(3))) { 
            case 5: 
                time_topk_query_5.push_back(time_span.count());
                break;
            case 10:
                time_topk_query_10.push_back(time_span.count());
                break;
            case 20:
                time_topk_query_20.push_back(time_span.count());
                break;
        }

        // Print the k-th elements
        std::cout << top_queries.at(i).at(0) << " (" << top_queries.at(i).at(1) << " / " << top_queries.at(i).at(2) << ") " << top_queries.at(i).at(3) << "-th element: " << res[res.size()-1].first << std::endl;
    }

    std::cout << "\n" << "============================================" << "\n\n";

    // Print the average time to make a query
    return std::tuple<float,float,float>(accumulate(time_topk_query_5.begin(), time_topk_query_5.end(), 0.0) / time_topk_query_5.size(), accumulate(time_topk_query_10.begin(), time_topk_query_10.end(), 0.0) / time_topk_query_10.size(), accumulate(time_topk_query_20.begin(), time_topk_query_20.end(), 0.0) / time_topk_query_20.size());
}

template<typename BaselineNumber, typename LoadFileName1, typename LoadFileName2, typename LoadFileName3, typename LoadFileName4>
void execute(BaselineNumber& b, const LoadFileName1& lfn1, const LoadFileName2& lfn2, const LoadFileName3& lfn3, const LoadFileName4& lfn4) {
    Query q;

    b.load("\0", lfn1); // Load file needed to query
    q.load("\0", lfn2, lfn3, lfn4);

    std::vector<std::vector<std::string>> range_queries_b = q.getRangeQueriesB();
    float avg_range_query_b = executeRangeQueries(b, range_queries_b);

    std::vector<std::vector<std::string>> range_queries_m = q.getRangeQueriesM();
    float avg_range_query_m = executeRangeQueries(b, range_queries_m);

    std::vector<std::vector<std::string>> range_queries_s = q.getRangeQueriesS();
    float avg_range_query_s = executeRangeQueries(b, range_queries_s);

    /*********************/

    std::vector<std::vector<std::string>> top_queries_b = q.getTopKQueriesB();
    std::tuple<float,float,float> avg_topk_query_b = executeTopKQueries(b, top_queries_b);

    std::vector<std::vector<std::string>> top_queries_m = q.getTopKQueriesM();
    std::tuple<float,float,float> avg_topk_query_m = executeTopKQueries(b, top_queries_m);

    std::vector<std::vector<std::string>> top_queries_s = q.getTopKQueriesS();
    std::tuple<float,float,float> avg_topk_query_s = executeTopKQueries(b, top_queries_s);
    
    std::cout << "\n" << "============================================" << "\n\n";
    std::cout << "Average time to make big range queries: " << avg_range_query_b << " seconds." << std::endl;
    std::cout << "Average time to make medium range queries: " << avg_range_query_m << " seconds." << std::endl;
    std::cout << "Average time to make small range queries: " << avg_range_query_s << " seconds." << std::endl;
    std::cout << "\n" << "============================================" << "\n\n";
    std::cout << "Average time to make big top k queries: " << "k=5: " << std::get<0>(avg_topk_query_b) << " seconds." << " k=10: " << std::get<1>(avg_topk_query_b) << " seconds." << " k=20: " << std::get<2>(avg_topk_query_b) << " seconds." << std::endl;
    std::cout << "Average time to make medium top k queries: " << "k=5: " << std::get<0>(avg_topk_query_m) << " seconds." << " k=10: " << std::get<1>(avg_topk_query_m) << " seconds." << " k=20: " << std::get<2>(avg_topk_query_m) << " seconds." << std::endl;
    std::cout << "Average time to make small top k queries: " << "k=5: " << std::get<0>(avg_topk_query_s) << " seconds." << " k=10: " << std::get<1>(avg_topk_query_s) << " seconds." << " k=20: " << std::get<2>(avg_topk_query_s) << " seconds." << std::endl;
    std::cout << "\n" << "============================================" << "\n\n";
}

int main(int argc, char* argv[]) {
	Query q;

    if (argc == 6) {
        switch (atoi(argv[1])) { // Choose which implementation to use
            case 0: {
                Baseline b;
                execute(b, argv[2], argv[3], argv[4], argv[5]);   

                break;
            }
            case 1: {
                Baseline1 b1;
                execute(b1, argv[2], argv[3], argv[4], argv[5]);

                break;
            }
            case 2: {
                Baseline2 b2;
                execute(b2, argv[2], argv[3], argv[4], argv[5]);

                break;
            }
            case 3: {
                Baseline3 b3;
                execute(b3, argv[2], argv[3], argv[4], argv[5]);

                break;
            }
            case 4: {
                Baseline4 b4;
                execute(b4, argv[2], argv[3], argv[4], argv[5]);

                break;
            }
        }
    }
}