source:
	wget -O series.txt.gz "http://xor.di.unipi.it/~rossano/time_series.txt.gz"
	gunzip series.txt.gz

compile:
	g++ -O3 -std=c++11 -o serialize serialize.cpp -lboost_serialization
	./serialize
	rm serialize
	g++ -O3 -std=c++11 -o query query.cpp -lboost_serialization
	
all:
	wget -O series.txt.gz "http://xor.di.unipi.it/~rossano/time_series.txt.gz"
	gunzip series.txt.gz	
	g++ -O3 -std=c++11 -o serialize serialize.cpp -lboost_serialization
	./serialize
	rm series.txt
	rm serialize
	g++ -O3 -std=c++11 -o query query.cpp -lboost_serialization
