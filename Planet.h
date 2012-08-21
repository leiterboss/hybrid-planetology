/*
 * Map.h
 *
 *  Created on: Jun 25, 2012
 *      Author: leiterboss
 */

#ifndef MAP_H_
#define MAP_H_

#include <cmath>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include "random.h"
//#include <vector>
#include <fstream>
#include <sstream>
//#include <string>
#include <set>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>

using namespace std;

namespace planet {

#define DEFAULT_MAP_SIZE 2			// Power of 2 plus one
#define MAX_MAP_SIZE 1025			// Ditto
#define STARTING_MIN_HEIGHT -10		// Minimum height for terrain generation algorithm (can be adjusted at the end)
#define STARTING_MAX_HEIGHT 10		// Maximum " " "
#define MOISTURE_CONSTANT 0.95		// Moisture propagation constant (0 to 1)

#define PI 3.1415926535897932

// Map visualization modes
#define MODE_HEIGHT 0
#define MODE_MOISTURE 1
#define MODE_COUNTRIES 2

// Kinds of terrain (tiles)
#define TYPE_WATER 0
#define TYPE_OTHER 1

// Moisture thresholds
#define MOISTURE_VHIGH 0.9
#define MOISTURE_HIGH 0.6
#define MOISTURE_MED 0.4
#define MOISTURE_LOW 0.2
//#define MOISTURE_VLOW 0

// Territory types
#define COUNTRY_WATER -2
#define COUNTRY_NONE -1

struct VECTOR {float x,y,z;};
struct stats {float maxHeight; float minHeight;};
#define POINT VECTOR

extern VECTOR makeVector(POINT p1, POINT p2);
extern VECTOR addVectors(VECTOR v1, VECTOR v2);
extern VECTOR crossProduct(VECTOR v1, VECTOR v2);
extern VECTOR normalize(VECTOR v);

// Average earth radius: 6371 km
// Earth area (estimation): 4*Pi*(6371)^2 = 51e7 km^2
// Map area: (N km / tile * this->size tiles)^2 = 51e7 km^2
// 		N km / tile = sqrt((51e7) / (this->size^2))
//		e.g. size = 2^8  ->  N ~ 88 km / tile

class Planet {
private:
	friend class boost::serialization::access;
	vector<vector<float> > heightMap;
	vector<vector<float> > moisture;
	vector<vector<float> > countries;
	int size;
	float roughness;
	float minHeight, maxHeight;	// For generation purposes only
	float actualMaxHeight;
	float relativeWaterLevel;
	float absoluteWaterLevel;
	int numCountries;
	pair<short,short> *capitalCities;

	set<pair<short,short> > *countryTerritory;

	void diamondStep(int);
	void squareStep(int);
	void averageDiamondHeight(int, int, int, int);
	void averageSquareHeight(int, int, int, int);
	void setHeight(int x, int y, float val) {
			if (x < this->size && y < 2*this->size) {
				if (x == 0)
					for(int i = 0; i < 2*this->size; i++) {
						this->heightMap[0][i] = val;
					}
				else if (x == this->size - 1)
					for(int i = 0; i < 2*this->size; i++)
						this->heightMap[this->size-1][i] = val;
				else if (y == 0 || y == 2*this->size -1)
					this->heightMap[x][0] = this->heightMap[x][2*(this->size-1)] = val;
				else this->heightMap[x][y] = val;
			}
		}
	void setMoisture(int x, int y, float val) {
		if (x < this->size && y < 2*this->size) {
			if (x == 0)
				for(int i = 0; i < 2*this->size; i++) {
					this->moisture[0][i] = val;
				}
			else if (x == this->size - 1)
				for(int i = 0; i < 2*this->size; i++)
					this->moisture[this->size-1][i] = val;
			else if (y == 0 || y == 2*this->size -1)
				this->moisture[x][0] = this->moisture[x][2*(this->size-1)] = val;
			else this->moisture[x][y] = val;
		}
	}
	void setCountry(int x, int y, int val) {
			if (x < this->size && y < 2*this->size) {
				if (x == 0)
					for(int i = 0; i < 2*this->size; i++) {
						this->countries[0][i] = val;
					}
				else if (x == this->size - 1)
					for(int i = 0; i < 2*this->size; i++)
						this->countries[this->size-1][i] = val;
				else if (y == 0 || y == 2*this->size -1)
					this->countries[x][0] = this->countries[x][2*(this->size-1)] = val;
				else this->countries[x][y] = val;
			}
		}
	int getNormalizedY(int y) {
		return (y + 2*(this->size - 1)) % (2*(this->size - 1));
	}
	void addHeight(int x, int y, float val) {
		this->setHeight(x,y,this->getHeight(x,y) + val);

	}
	float averageAndAddRandom(int x, int y, int stepx, int stepy, int iter, bool diamond);
	void smoothHeightMap(float k, int iters);
	VECTOR calculateVertexNormal(POINT);
	POINT getOceanMarker();
	void getDistanceToLand(int,int,int**);
	void normalizeHeights();
	void calculateStats();
	void calculateMoisture();
	int getDistanceToWater(int,int);
	pair<short,short> getRandomFromSet(set<pair<short,short> > &s);
	bool updateBorders(
			set<pair<short,short> > *borders,
			int curCountry,
			pair<short,short> acquired,
			set<short> &expandable,
			vector<vector<set<short> > > &candidateCountries);
	void populate();
	short closestCountry(int x, int y);
	void readHeightMap(string path, float seaLevel, int numCountries, int realMaxHeight);
	void readBinaryMap(string path);
	void generateMap(
			float smoothing = 0.5,
			int smoothIters = 3,
			int seed = 0);

public:
	Planet(
			int size = DEFAULT_MAP_SIZE,		// Size of map's side = 2^siz - 1
			float roughness = 0.75,				// Roughness of landscape: values between 0 and 1
			int maxHeight = 8000,				// Height of the highest point
			float relativeWaterLevel = 0.4,		// Percentage of water in world, between 0 and 1
			int nCountries = 100,				// Number of countries in world
			float smoothing = 0.5,				// Smoothing magnitude, between 0 and 1: 0 -> very smoothed, 1 -> not smoothed at all
			int smoothIters = 3,				// Iterations of smoothing algorithm
			int seed = 0						// Random seed
			);
	Planet(	string path,						// Path to file (height map or binary)
			float seaLevel=0.6,					// Percentage of water in world, between 0 and 1
			int numCountries=100,				// Number of countries in world
			int maxHeight=8000					// Height of the highest point (only in height map)
			);
	virtual ~Planet();
	void drawToFile();
	float calculateMedianHeight(float);
	int getTileType(int,int);
	void saveToFile(string);
	void saveHeightMap(string path);
	bool isNull() {return (this->size == -1);}
	float getHeight(int x, int y, vector<vector<float> > *vec = (vector<vector<float> >*) 0) {
		if (vec == (vector<vector<float> >*) 0) vec = &this->heightMap;
		return (*vec)[x][(y + 2*(this->size - 1)) % (2*(this->size - 1))];
	}
	float getMoisture(int x, int y, vector<vector<float> > *vec = (vector<vector<float> >*) 0) {
		if (vec == (vector<vector<float> >*) 0) vec = &this->moisture;
		return (*vec)[x][(y + 2*(this->size - 1)) % (2*(this->size - 1))];
	}
	int getCountry(int x, int y, vector<vector<float> > *vec = (vector<vector<float> >*) 0) {
		if (vec == (vector<vector<float> >*) 0) vec = &this->countries;
		if (this->getTileType(x,y) == TYPE_WATER) return COUNTRY_WATER;
		else return (*vec)[x][this->getNormalizedY(y)];
	}
	int getSizeOrder() {return this->size;}
	float getMaxHeight() {return this->actualMaxHeight;}
};
}

#endif /* MAP_H_ */
