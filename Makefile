BASELINE = 4

source:
	wget -O series.txt.gz "http://xor.di.unipi.it/~rossano/time_series.txt.gz"
	gunzip series.txt.gz

index:
	g++ -O3 -I ~/include -L ~/lib -std=c++11 -o build_index build_index.cpp -lboost_serialization -lsdsl	
	./build_index $(BASELINE) series.txt dataset_$(BASELINE).id
	rm build_index
	
generate:
	g++ -O3 -std=c++11 -o build_query_sets build_query_sets.cpp -lboost_serialization
	./build_query_sets series.txt 100 .queries
	rm build_query_sets

query:
	g++ -O3 -I ~/include -L ~/lib -std=c++11 -o run_queries run_queries.cpp -lboost_serialization -lsdsl
	./run_queries $(BASELINE) dataset_$(BASELINE).id .queries
	rm run_queries
