# Time series indexing (Big data project)
This is the project I have done for the course of Lab of Big Data (2016--2017).

## Libraries
* [boost](http://www.boost.org) - library used to serialize
* [sdsl](https://github.com/simongog/sdsl-lite) - library implementing succinct data structures

## Tecnologies
* make
* g++11
* wget
* gunzip

##Usage
Download this project and it is possible to use the following commands:

```
$ make source (to download the dataset and create some necessary directory)
$ make index (to build an index, it is possible to specify the implementation to use, 'make index BASELINE=0')
$ make generate (to generate a number of queries, it is possible to specify the number of queries that has to be generated, 'make generate NUM_QUERY=100')
$ make query (to answer the query generated, it is possible to specify the implementation to use, 'make query BASELINE=0')
```