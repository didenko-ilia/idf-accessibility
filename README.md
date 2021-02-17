# Accesibility

## Library Installation
The script uses two external libraries: libcurl and jansson. To instal them (on Linux):
1. Download the libraries from the official website ([libcurl](https://curl.se/download.html), [jansson](http://digip.org/jansson/releases/)). Extract them into a folder of your choice. I've used libcurl 7.74.0 and jansson 2.11
2. For each of the two libraries above, go to their respective folder. Then, use the following commands :
```{cmd}
./configure --prefix=DIR
make 
make check
make install
```
Where DIR = A directory used by the C++ compiler, ex. /usr/local

## Usage
```
./script addr
```
Where addr is an IP:PORT pair of the OTP instances running on the IDF data.  
The script takes the position from a [dataset](https://datanova.laposte.fr/explore/dataset/laposte_hexasmal/) of LaPoste. It takes the data for the department codes that are of interest to us (i.e. Ile-de-France) and uses the included coordinates to calculate accessibility. Accessibility of A is calculated as the average travel time from all other point to the point A. A couple of gps coordinates have been modified by hand since they were in inaccessible places (rivers, lakes, airport).

For OTP installation refer to the [Basic Tutorial](http://docs.opentripplanner.org/en/latest/Basic-Tutorial/) using the [GTFS data](https://data.iledefrance-mobilites.fr/explore/dataset/offre-horaires-tc-gtfs-idf/information/) from IdF-Mobility and the [OSM data](http://download.geofabrik.de/europe/france/ile-de-france.html) for the Ile-de-France region

## Visualization
Visualization is done in Python using folium. It uses geo data form a gouverment [dataset](https://www.data.gouv.fr/en/datasets/apur-communes-ile-de-france/#_) and populates with the accessibility values from the script to build an gradient map of accessibility
