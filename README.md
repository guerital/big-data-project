# Time series indexing (Big data project)
Project done for the course of Lab of Big Data (2016--2017). 

## Libraries
* [boost](http://www.boost.org) - library used to serialize the indexes
* [sdsl](https://github.com/simongog/sdsl-lite) - library implementing succinct data structures

## Tecnologies
* make
* g++11
* wget
* gunzip

##Usage
After the downloading of this repository, it is possibile tu use following commands:

1. To download the dataset and create some needed directory
```
$ make source
``` 

2. To build an index, it is possibile to choose the implementation to use
``` 
$ make index BASELINE=0
``` 
 
3. To generate a number of queries, it is possible to specify the number of queries that has to be generated
```
$ make generate NUM_QUERY=100
``` 

4. To answer the query generated, it is possible to specify the implementation to use
``` 
$ make query BASELINE=0
```