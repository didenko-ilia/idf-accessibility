# Accesibility

Create an empty folder FOLD (name it as you want) where you will put all the data related to the project.

## Open Street Planner Setup
Before running the software, you must have a running version of Open Trip Planner (OTP) on a machine.
For OTP installation refer to the [Basic Tutorial](http://docs.opentripplanner.org/en/latest/Basic-Tutorial/) using the [GTFS data](https://data.iledefrance-mobilites.fr/explore/dataset/offre-horaires-tc-gtfs-idf/information/) from IdF-Mobility and the [OSM data](http://download.geofabrik.de/europe/france/ile-de-france.html) for the Ile-de-France region. 

After you download the jar file of OTP, place it into FOLD.
As for the GTFS data of Ile de France, you need to download a csv file where you will find the link to download a zip file. Place this zip file in FOLD. As for Open Street Map (OSM) data, you need to download a pbf file, from the website indicated above, and place it into FOLD.

Run OTP as explained in the tutorial. If you get an "Out of memory" exception, try to increase the memory limits, specified in the parameters. For instance, I needed to give 8GB to OTP to make it run for Île de France.

## Library Installation
The script uses two external libraries: libcurl and jansson. To instal them (on Linux):
1. Download the libraries from the official website ([libcurl](https://curl.se/download.html), [jansson](http://digip.org/jansson/releases/)). Extract them into a folder of your choice. I've used libcurl 7.74.0 and jansson 2.11
2. For each of the two libraries above, go to their respective folder. Then, use the following commands :
```{cmd}
./configure
make 
make check
sudo make install
```

Then, install again libcurl (true, it is redundant, but if I did not install the two versions of libcurl, I had problem in compilation and/or linking phase)
```{cmd}
sudo apt-get install libcurl3
```

## Compilation
Compile script.cpp. On Ubuntu 18.04, the following command on the command line worked:
```{cmd}
cd FOLD
gcc script.cpp -lcurl -ljansson -lstdc++ -o script
```

## Usage
```
./script addr
```
Where addr is an IP:PORT pair of the OTP instances running on the IDF data (if you followed the basic tutorial of OTP, it should be localhost:8080.  
The script takes the position from a [dataset](https://datanova.laposte.fr/explore/dataset/laposte_hexasmal/) of LaPoste. It takes the data for the department codes that are of interest to us (i.e. Ile-de-France) and uses the included coordinates to calculate accessibility. Accessibility of A is calculated as the average travel time from all other point to the point A. A couple of gps coordinates have been modified by hand since they were in inaccessible places (rivers, lakes, airport).

## Visualization
Visualization is done in Python using folium. It uses geo data form a gouverment [dataset](https://www.data.gouv.fr/en/datasets/apur-communes-ile-de-france/#_) and populates with the accessibility values from the script to build an gradient map of accessibility
