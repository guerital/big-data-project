source:
	wget -O series.txt.gz "http://xor.di.unipi.it/~rossano/time_series.txt.gz"
	gunzip series.txt.gz

index:
	g++ -O3 -std=c++11 -o build_index build_index.cpp -lboost_serialization
	./build_index 0 series.txt dataset_0.id
	rm build_index
	
generate:
	g++ -O3 -std=c++11 -o build_query_sets build_query_sets.cpp -lboost_serialization
	./build_query_sets series.txt 100 .queries
	rm build_query_sets

query:
	g++ -O3 -std=c++11 -o run_queries run_queries.cpp -lboost_serialization
	./run_queries 0 dataset_0.id .queries
	rm run_queries