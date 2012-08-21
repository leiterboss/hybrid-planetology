#include "Planet.h"

namespace planet {

Planet::~Planet() {}

Planet::Planet(string path, float seaLevel, int numCountries, int maxHeight) {
	ifstream ifs(path.c_str(), ios::binary);
	this->size = -1;
	if (ifs.good()) {
		char header;
		ifs.read((char*)&(header),sizeof(char));

		if (header=='X')
			this->readBinaryMap(path);
		else if (header=='B') {
			this->readHeightMap(path,seaLevel,numCountries,maxHeight);
		}
		ifs.close();
	}
}

void Planet::readBinaryMap(string path) {
	ifstream ifs(path.c_str(), ios::binary);
	char dummy;
	if (ifs.good()) {
		ifs.read((char*)&(dummy),sizeof(char));	// Header character, discard
		ifs.read((char*)&(this->size),sizeof(this->size));
		ifs.read((char*)&(this->roughness),sizeof(this->roughness));
		ifs.read((char*)&(this->minHeight),sizeof(this->minHeight));
		ifs.read((char*)&(this->maxHeight),sizeof(this->maxHeight));
		ifs.read((char*)&(this->actualMaxHeight),sizeof(this->actualMaxHeight));
		ifs.read((char*)&(this->relativeWaterLevel),sizeof(this->relativeWaterLevel));
		ifs.read((char*)&(this->absoluteWaterLevel),sizeof(this->absoluteWaterLevel));
		ifs.read((char*)&(this->numCountries),sizeof(this->numCountries));

		this->capitalCities = new pair<short,short>[this->numCountries];

		vector<float> v = vector<float>(2*this->size,0);
		this->heightMap =  vector<vector<float> >(this->size,v);

		v = vector<float>(2*this->size,0);
		this->moisture = vector<vector<float> >(this->size,v);

		v = vector<float>(2*this->size,COUNTRY_NONE);
		this->countries = vector<vector<float> >(this->size,v);

		for(int i = 0; i < this->numCountries; i++) {
			ifs.read((char*)&(this->capitalCities[i].first),sizeof(this->capitalCities[i].first));
			ifs.read((char*)&(this->capitalCities[i].second),sizeof(this->capitalCities[i].second));
		}

		for(int i = 0; i < this->size; i++)
			for(int j = 0; j < 2*(this->size-1); j++) {
				ifs.read((char*)&(this->heightMap[i][j]),sizeof(this->heightMap[i][j]));
				ifs.read((char*)&(this->moisture[i][j]),sizeof(this->moisture[i][j]));
				ifs.read((char*)&(this->countries[i][j]),sizeof(this->countries[i][j]));
			}
		ifs.close();
	}
}

void Planet::readHeightMap(string path, float seaLevel, int nCountries, int realMaxHeight) {
	ifstream ifs(path.c_str(), ios::binary);
	int offsetImage, width, height;
	short color = 0;
	short levels[256];
	/*int max = -1;
	int maxC = -1;*/

/*	for(int i = 0; i < 256; i++)
		levels[i] = 0;
*/
	if (ifs.good()) {
		ifs.ignore(10);
		ifs.read((char*)&(offsetImage),sizeof(int));
		ifs.ignore(4);
		ifs.read((char*)&(width),sizeof(int));
		ifs.read((char*)&(height),sizeof(int));
		ifs.ignore(offsetImage-10-4-3*sizeof(int));

		this->size = height + 1;
		this->minHeight = 0;
		this->maxHeight = 1;
		this->actualMaxHeight = realMaxHeight;
		this->numCountries = nCountries;
		this->relativeWaterLevel = seaLevel;
		this->capitalCities = new pair<short,short>[this->numCountries];

		vector<float> v = vector<float>(2*this->size,0);
		this->heightMap =  vector<vector<float> >(this->size,v);

		v = vector<float>(2*this->size,0);
		this->moisture =  vector<vector<float> >(this->size,v);

		v = vector<float>(2*this->size,COUNTRY_NONE);
		this->countries =  vector<vector<float> >(this->size,v);

		for(int i = height; i >= 0; i--)
			for(int j = width - 1; j >= 0; j--) {
				ifs.read((char*)&(color),sizeof(char));
				levels[color]++;
				this->setHeight(i,j, (float) color / 255);
			}
		ifs.close();
/*
		for(int i = 0; i < 256; i++)
			if (levels[i] > maxC) {max = i; maxC = levels[i];}
	*/
		this->normalizeHeights();
		this->calculateMoisture();
		this->calculateStats();
		this->populate();
	}
}

Planet::Planet(
		int siz,					// Size of map's side = 2^siz - 1
		float rough,				// Roughness of landscape: values between 0 and 1
		int maxHeight,				// Height of the highest point
		float waterLevel,			// Percentage of water in world, between 0 and 1
		int nCountries,				// Number of countries in world
		float smoothing,			// Smoothing magnitude, between 0 and 1: 0 -> very smoothed, 1 -> not smoothed at all
		int smoothIters,			// Iterations of smoothing algorithm
		int seed					// Random seed
		)
{
	this->size = (1 << siz) + 1;
	this->roughness = rough;
	this->minHeight = STARTING_MIN_HEIGHT;
	this->maxHeight = STARTING_MAX_HEIGHT;
	this->relativeWaterLevel = waterLevel;
	this->actualMaxHeight = maxHeight;
	this->numCountries = nCountries;
	this->absoluteWaterLevel = -1;

	this->capitalCities = new pair<short,short>[this->numCountries];

	vector<float> v = vector<float>(2*this->size,0);
	this->heightMap =  vector<vector<float> >(this->size,v);

	v = vector<float>(2*this->size,0);
	this->moisture = vector<vector<float> >(this->size,v);

	v = vector<float>(2*this->size,COUNTRY_NONE);
	this->countries = vector<vector<float> >(this->size,v);

	this->generateMap(smoothing, smoothIters, seed);
}

void Planet::generateMap(float smoothing, int smoothIters, int seed) {
	if (seed == 0) seed = rand();
	srand(seed);
	cout << "Seed: " << seed << endl;

	for(int i = 0; i < log2(this->size-1); i++) {
		this->diamondStep(i);
		this->squareStep(i);
	}

	for(int i = 1; i < this->size - 1; i++)
		for(int j = 1; j < 2*(this->size) - 1; j += 2) {
			float rnd= uniformDistValue(this->minHeight,this->maxHeight);
			float avg =
					this->getHeight(i-1,j-1) / 6 +
					this->getHeight(i-1,j+1) / 6 +
					this->getHeight(i,j-1) / 6 +
					this->getHeight(i,j+1) / 6 +
					this->getHeight(i+1,j-1) / 6 +
					this->getHeight(i+1,j+1) / 6;
			this->setHeight(i, j, avg + pow(this->roughness,(int) log2(this->size-1)) * rnd);
		}

	this->smoothHeightMap(smoothing,smoothIters);
	this->normalizeHeights();
	this->calculateMoisture();
	this->calculateStats();
	this->populate();
}

void Planet::populate() {
	set<pair<short,short> > borders[this->numCountries];
	set<pair<short,short> > freeTiles;
	set<short> expandableCountries;
	this->capitalCities = new pair<short,short>[this->numCountries];
	vector<set<short> > v = vector<set<short> >(2*this->size,set<short>());
	vector<vector<set<short> > > candidateCountries = vector<vector<set<short> > >(this->size,v);
	pair<short,short> nullPair(-1,-1);

	for(int i = 0; i < this->size; i++)
		for(int j = 0; j < 2*(this->size - 1); j++)
			if (this->getTileType(i,j) != TYPE_WATER)
				freeTiles.insert(make_pair(i,j));

	for(int i = 0; i < this->numCountries; i++) {
		pair<short, short> r = this->getRandomFromSet(freeTiles);
		freeTiles.erase(r);
		borders[i].insert(r);
		this->capitalCities[i] = r;
		this->setCountry(r.first,r.second,i);
		this->updateBorders(borders, i, r, expandableCountries, candidateCountries);
	}

	for(int i = 0; i < this->numCountries; i++)
		for(int j = 0; j < this->numCountries; j++) {
			borders[i].erase(this->capitalCities[j]);
			if (!borders[i].empty()) expandableCountries.insert(i);
		}

	while(!expandableCountries.empty()) {

		set<short>::const_iterator it(expandableCountries.begin());
		advance(it,rand() % expandableCountries.size());
		int curCountry = *it;

		if (borders[curCountry].size() == 0) {
			expandableCountries.erase(curCountry);
			continue;
		}
		pair<short, short> r = this->getRandomFromSet(borders[curCountry]);
		borders[curCountry].erase(r); freeTiles.erase(r);
		this->setCountry(r.first,r.second,curCountry);
		this->updateBorders(borders, curCountry, r, expandableCountries, candidateCountries);
	}

	while(!freeTiles.empty()) {
		pair<short, short> r = this->getRandomFromSet(freeTiles);
		freeTiles.erase(r);
		int country = this->closestCountry(r.first,r.second);
		this->setCountry(r.first,r.second,country);
	}
}

short Planet::closestCountry(int x, int y) {
	for(int i = 1; i < this->size; i++) {
		for(int j = -i; j <= i; j++) {
			if (x >= i && this->getCountry(x-i,y+j) > 0) return this->getCountry(x-i,y+j);
			if ((x+i) >= 0 && (x+i) < this->size)
				if (this->getCountry(x+i,y+j) > 0) return this->getCountry(x+i,y+j);
			if ((x+j) >= 0 && (x+j) < this->size) {
				if (this->getCountry(x+j,y-i) > 0) return this->getCountry(x+j,y-i);
				if (this->getCountry(x+j,y+i) > 0) return this->getCountry(x+j,y+i);
			}
		}
	}
	return COUNTRY_NONE;
}

bool Planet::updateBorders(
		set<pair<short,short> > *borders,
		int curCountry,
		pair<short,short> acquired,
		set<short> &expandable,
		vector<vector<set<short> > > &candidateCountries) {

	  set<pair<short,short> >::const_iterator it;
	  set<short>::const_iterator it2;
	  int x,y;
	  x = acquired.first; y = acquired.second;

//	  if (x != 0 && this->getCountry(x-1, y-1) == COUNTRY_NONE)	  borders[curCountry].insert(make_pair(x-1,y-1));
	  if (x != 0 && this->getCountry(x-1, y) == COUNTRY_NONE) {
		  borders[curCountry].insert(make_pair(x-1,y));
		  candidateCountries[x-1][y].insert(curCountry);
	  }
//	  if (x != 0 && this->getCountry(x-1, y+1) == COUNTRY_NONE)	  borders[curCountry].insert(make_pair(x-1,y+1));
	  if (this->getCountry(x, y-1) == COUNTRY_NONE) {
		  borders[curCountry].insert(make_pair(x,this->getNormalizedY(y-1)));
		  candidateCountries[x][this->getNormalizedY(y-1)].insert(curCountry);
	  }
	  if (this->getCountry(x, y+1) == COUNTRY_NONE) {
		  borders[curCountry].insert(make_pair(x,this->getNormalizedY(y+1)));
		  candidateCountries[x][this->getNormalizedY(y+1)].insert(curCountry);
	  }
//	  if (x != 2*(this->size-1) && this->getCountry(x+1, y-1) == COUNTRY_NONE)	  borders[curCountry].insert(make_pair(x+1,y-1));
	  if (x != this->size-1 && this->getCountry(x+1, y) == COUNTRY_NONE) {
		  borders[curCountry].insert(make_pair(x+1,y));
		  candidateCountries[x+1][y].insert(curCountry);
	  }
//	  if (x != 2*(this->size-1) && this->getCountry(x+1, y+1) == COUNTRY_NONE)	  borders[curCountry].insert(make_pair(x+1,y+1));

/*	  for(it2 = expandable.begin(); it2 != expandable.end(); it2++) {
		  int i = *it2;
		  for(it = borders[i].begin(); it != borders[i].end(); it++) {
			  borders[i].erase(acquired);
			  if (this->getCountry(it->first,it->second) != COUNTRY_NONE)
				  borders[i].erase(make_pair(it->first,it->second));
		  }
		  if (borders[i].empty())
			  expandable.erase(i);
	  }
*/
	  for(it2 = candidateCountries[x][y].begin(); it2 != candidateCountries[x][y].end(); it2++) {
		  short cnt = *it2;
		  borders[cnt].erase(acquired);
		//  if (this->getCountry(it->first,it->second) != COUNTRY_NONE)
		//	  borders[cnt].erase(make_pair(it->first,it->second));
		  if (borders[cnt].empty())
			  expandable.erase(cnt);
	  }

	  return !borders[curCountry].empty();
}

pair<short,short> Planet::getRandomFromSet(set<pair<short,short> > &s) {
	  set<pair<short,short> >::const_iterator it(s.begin());
	  int chosen = rand() % s.size();
	  advance(it,chosen);
	  pair<short,short> ret = *it;
	  return ret;
}

void Planet::calculateMoisture() {
	for(int i = 0; i < this->size-1; i++)
		for(int j = 0; j < 2*(this->size-1); j++) {
			this->setMoisture(i,j,pow(MOISTURE_CONSTANT,this->getDistanceToWater(i,j)));
		}
}

int Planet::getDistanceToWater(int x, int y) {
	if (this->getHeight(x,y) < 0) return 0;

	int farDist = log(MOISTURE_LOW) / log(MOISTURE_CONSTANT);
	for(int i = 1; i < farDist; i++) {
		for(int j = -i; j <= i; j++) {
			if (x >= i && this->getTileType(x-i,y+j) == TYPE_WATER) return i;
			if ((x+i) >= 0 && (x+i) < this->size)
				if (this->getTileType(x+i,y+j) == TYPE_WATER) return i;
			if ((x+j) >= 0 && (x+j) < this->size) {
				if (this->getTileType(x+j,y-i) == TYPE_WATER) return i;
				if (this->getTileType(x+j,y+i) == TYPE_WATER) return i;
			}
		}
	}
	return farDist+1;
}

int Planet::getTileType(int x, int y) {
	if (this->getHeight(x,y) < 0) return TYPE_WATER;
	return TYPE_OTHER;
}

void Planet::normalizeHeights() {
	float maxHeight = 0;
	float tmp;

	this->absoluteWaterLevel = this->calculateMedianHeight(this->relativeWaterLevel);

	this->addHeight(0,0,-this->absoluteWaterLevel);
	this->addHeight(this->size - 1,0,-this->absoluteWaterLevel);
	for(int i = 1; i < this->size - 1; i++)
		for(int j = 0; j < 2*(this->size-1); j++) {
			this->addHeight(i,j,-this->absoluteWaterLevel);
		}

	for(int i = 0; i < this->size; i++)
		for(int j = 0; j < 2*this->size - 1; j++) {
			tmp = this->getHeight(i,j);
			if (tmp > maxHeight) maxHeight = tmp;
		}

	// Normalizing the north pole
	this->setHeight(0,0,this->getHeight(0,0)/maxHeight);
	// Normalizing the south pole
	this->setHeight(this->size - 1,0,this->getHeight(this->size-1,0)/maxHeight);
	for(int i = 1; i < this->size - 1; i++)
		for(int j = 0; j < 2*this->size - 1; j++) {
			this->setHeight(i,j,this->getHeight(i,j)/maxHeight);
		}

}

float max(float a, float b, float c) {
	if (a > b && a > c) return a;
	if (b > a && b > c) return b;
	return c;
}


void Planet::smoothHeightMap(float k, int iters) {
	// Smooth matrix:
	float a = (1 - k) / 8;
	float avg;
	// [a a a]
	// [a k a]
	// [a a a]

	vector<vector<float> > tilesCopy =  vector<vector<float> >(this->heightMap);

	for(int n = 0; n < iters; n++) {
		avg = 0;
		for (int j = 0; j < 2*(this->size-1); j++)
			avg += this->getHeight(1,j, &tilesCopy);
		this->setHeight(0, 0, this->getHeight(0,0) * k + (avg / (2*this->size - 1)) * (1 - k));

		for(int i = 1; i < this->size-1; i++)
			for (int j = 0; j < 2*(this->size-1); j++) {
				float avg =
						this->getHeight(i-1,j-1, &tilesCopy) * a +
						this->getHeight(i-1,j, &tilesCopy) * a +
						this->getHeight(i-1,j+1, &tilesCopy) * a +
						this->getHeight(i,j-1, &tilesCopy) * a +
						this->getHeight(i,j, &tilesCopy) * k +
						this->getHeight(i,j+1, &tilesCopy) * a +
						this->getHeight(i+1,j-1, &tilesCopy) * a +
						this->getHeight(i+1,j, &tilesCopy) * a +
						this->getHeight(i+1,j+1, &tilesCopy) * a;
				this->setHeight(i, j, avg);
			}

		avg = 0;
		for (int j = 0; j < 2*(this->size-1); j++)
			avg += this->getHeight(this->size - 2,j, &tilesCopy);
		this->setHeight(this->size - 2, 0, this->getHeight(this->size - 2,0) * k + (avg / (2*this->size - 1)) * (1 - k));
	}
}

float Planet::calculateMedianHeight(float perc) {
	int tileNumber = (this->size-1)*(2*(this->size-1));
	float heights[tileNumber];
	for(int i = 0; i < this->size-1; i++)
		for(int j = 0; j < 2*(this->size-1); j++)
			heights[i*(2*(this->size-1))+j] = this->getHeight(i,j);

	float* first(&heights[0]);
	float* last(first + tileNumber);
	std::sort(first, last);

	int pos = (int) (tileNumber*perc);
	if (pos < 0) pos = 0;
	else if (pos >= tileNumber) pos = tileNumber - 1;

	return heights[pos];
}

void Planet::averageDiamondHeight(int centerx, int centery, int stepx, int stepy) {

	int centerxp = (centerx + stepx < this->size)?		centerx + stepx:centerx + stepx - this->size + 1;
	int centerxm = (centerx - stepx >= 0)?          	centerx - stepx:centerx - stepx + this->size - 1;
	int centeryp = (centery + stepy < 2*this->size)?	centery + stepy:centery + stepy - 2*this->size + 1;
	int centerym = (centery - stepy >= 0)?          	centery - stepy:centery - stepy + 2*this->size - 1;

	float partialSum = this->getHeight(centerxp, centeryp) +
			this->getHeight(centerxp, centerym) +
			this->getHeight(centerxm, centeryp) +
			this->getHeight(centerxm, centerym);
	partialSum /= 4;
	this->setHeight(centerx,centery,partialSum);
}

void Planet::averageSquareHeight(int centerx, int centery, int stepx, int stepy) {

		int centerxp = (centerx + stepx < this->size)?		centerx + stepx:centerx + stepx - this->size + 1;
		int centerxm = (centerx - stepx >= 0)?          	centerx - stepx:centerx - stepx + this->size - 1;
		int centeryp = (centery + stepy < 2*this->size)?		centery + stepy:centery + stepy - 2*this->size + 1;
		int centerym = (centery - stepy >= 0)?          	centery - stepy:centery - stepy + 2*this->size - 1;

	float partialSum = this->getHeight(centerxp, centery) +
			this->getHeight(centerxm, centery) +
			this->getHeight(centerx, centeryp) +
			this->getHeight(centerx, centerym);
	partialSum /= 4;
	this->setHeight(centerx,centery,partialSum);
}

void Planet::diamondStep(int iter) {
	int centerx, centery, basex, basey;
	int sizem1 = this->size - 1;
	int stridex = sizem1 / (1 << iter); //(iter+1);
	int stridey = stridex * 2;
	basex = stridex / 2; //sizem1 / (2*(iter+1));
	basey = stridey / 2;

	for(int i = 0; i < (1<<iter); i++) {
		for(int j = 0; j < (1<<(iter)); j++) {
				centerx = basex + i * stridex;
				centery = basey + j * stridey;
				this->averageAndAddRandom(centerx,centery,stridex/2,stridey/2,iter,true);
		}
	}

}

void Planet::squareStep(int iter) {
	int centerx, centery, basex, basey;
	int sizem1 = this->size - 1;
	int stridex = sizem1 / (1 << iter);
	int stridey = stridex * 2;
	basex = stridex / 2;
	basey = 0;

	for(int i = 0; i < (1<<iter); i++) {
		for(int j = 0; j < (1<<(iter)) + 1; j++) {
			centerx = basex + i * stridex;
			centery = basey + j * stridey;
			this->averageAndAddRandom(centerx,centery,stridex/2,stridey/2,iter,false);
		}
	}
/*
	stringstream st;
	st << "/home/leiterboss/Desktop/mapH-s" << iter << ".obj";

	this->exportPoints(st.str(),1,1,1,3);
*/
	basex = 0;
	basey = stridey / 2; //sizem1 / (2*(iter+1));
	for(int i = 0; i < (1<<iter) + 1; i++) {
		for(int j = 0; j < (1<<(iter)); j++) {
			centerx = basex + i * stridex;
			centery = basey + j * stridey;
			this->averageAndAddRandom(centerx,centery,stridex/2,stridey/2,iter,false);
		}
	}
/*	stringstream st2;
	st2 << "/home/leiterboss/Desktop/mapH-ss" << iter << ".obj";

	this->exportPoints(st2.str(),1,1,1,3);
*/
}

float Planet::averageAndAddRandom(int centerx, int centery, int stepx, int stepy, int iter, bool diamond) {
	//if (this->getHeight(centerx,centery) != 0) return;
	float rnd;
	//if (this->getHeight(centerx,centery) != 0) rnd = 0;
	rnd= uniformDistValue(this->minHeight,this->maxHeight);

	if (diamond) this->averageDiamondHeight(centerx,centery,stepx,stepy);
	else this->averageSquareHeight(centerx,centery,stepx,stepy);

	this->addHeight(centerx,centery, pow(this->roughness,iter) * rnd);
	return rnd;
}

void Planet::drawToFile() {
   cout.setf(ios::fixed, ios::floatfield);
   cout.setf(ios::showpoint);
   //cout << setiosflags(ios::right);
	for(int i = 0; i < this->size; i++) {
		for(int j = 0; j < 2*this->size-1; j++) {
			cout << setprecision(8) << setw(6) <<  this->getHeight(i,j) << "   ";
		}
		cout << endl;
	}
	cout << endl;
}

POINT Planet::calculateVertexNormal(POINT center) {
	POINT up,upright,down,downleft,right,left;
	VECTOR v1,v2,ret;

	ret.x = 0; ret.y = 0; ret.z = 0;

	up.x = center.x - 1; up.y = center.y; up.z = this->getHeight(up.x,up.y);
	upright.x = center.x - 1; upright.y = center.y + 1; upright.z = this->getHeight(upright.x,upright.y);
	down.x = center.x + 1; down.y = center.y; down.z = this->getHeight(down.x,down.y);
	downleft.x = center.x + 1; downleft.y = center.y - 1; downleft.z = this->getHeight(downleft.x,downleft.y);
	right.x = center.x; right.y = center.y + 1; right.z = this->getHeight(right.x,right.y);
	left.x = center.x; left.y = center.y - 1; left.z = this->getHeight(left.x,left.y);

	v1 = makeVector(up,center); v2 = makeVector(upright,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	v1 = makeVector(upright,center); v2 = makeVector(right,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	v1 = makeVector(right,center); v2 = makeVector(down,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	v1 = makeVector(down,center); v2 = makeVector(downleft,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	v1 = makeVector(downleft,center); v2 = makeVector(left,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	v1 = makeVector(left,center); v2 = makeVector(up,center);
	v1 = crossProduct(v2,v1);
	ret = addVectors(ret,v1);

	ret = normalize(ret);

	return ret;
}

POINT Planet::getOceanMarker() {
	//int dists[this->size][this->size];
	POINT ret;

	//getDistanceToLand(0,0,dists);

	return ret;
}

void Planet::calculateStats() {
	float min,max;
	min = this->maxHeight;
	max = this->minHeight;
	for(int i = 0; i < this->size-1; i++)
		for(int j = 0; j < 2*(this->size-1); j++) {
			float curHeight = this->getHeight(i,j);
			if (curHeight > max) max = curHeight;
			if (curHeight < min) min = curHeight;
		}
	this->maxHeight = max; // Always 1 (normalized)
	this->minHeight = min;
}

void Planet::getDistanceToLand(int i, int j, int** dists) {

	if (i == this->size) return;
	if (this->getHeight(i,j) > this->absoluteWaterLevel) dists[i][j] = 0;
}

void Planet::saveToFile(string path) {
	ofstream ofs(path.c_str(), ios::binary);
	char fileHeader = 'X';

	ofs.write((char*)&(fileHeader),sizeof(char));
	ofs.write((char*)&(this->size),sizeof(this->size));
	ofs.write((char*)&(this->roughness),sizeof(this->roughness));
	ofs.write((char*)&(this->minHeight),sizeof(this->minHeight));
	ofs.write((char*)&(this->maxHeight),sizeof(this->maxHeight));
	ofs.write((char*)&(this->actualMaxHeight),sizeof(this->actualMaxHeight));
	ofs.write((char*)&(this->relativeWaterLevel),sizeof(this->relativeWaterLevel));
	ofs.write((char*)&(this->absoluteWaterLevel),sizeof(this->absoluteWaterLevel));
	ofs.write((char*)&(this->numCountries),sizeof(this->numCountries));

	for(int i = 0; i < this->numCountries; i++) {
		ofs.write((char*)&(this->capitalCities[i].first),sizeof(this->capitalCities[i].first));
		ofs.write((char*)&(this->capitalCities[i].second),sizeof(this->capitalCities[i].second));
	}

	for(int i = 0; i < this->size; i++)
		for(int j = 0; j < 2*(this->size-1); j++) {
			ofs.write((char*)&(this->heightMap[i][j]),sizeof(this->heightMap[i][j]));
			ofs.write((char*)&(this->moisture[i][j]),sizeof(this->moisture[i][j]));
			ofs.write((char*)&(this->countries[i][j]),sizeof(this->countries[i][j]));
		}
	ofs.close();
}

void Planet::saveHeightMap(string path) {
	ofstream ofs(path.c_str(), ios::binary);
	int height = this->size - 1;
	int width = 2 * height;
	char colors[height * width];
	char data[1500] = {	0x42, 0x4d, 0x36, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x04, 0x00, 0x00, 0x28, 0x00, 0x00, 0x00,
							width, width>>8, width>>16, width>>24, height, height>>8, height>>16, height>>24, 0x01, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x02, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00 };


	for(int i = 0; i < 256; i++) {
		data[54+4*i] = data[54+4*i+1] = data[54+4*i+2] = i;
		data[54+4*i+3] = 0;
	}

	ofs.write(data,1078);

	float divider = this->maxHeight - this->minHeight;

	int count = 0;
	for(int i = height-1; i >= 0; i--)
		for(int j = width - 1; j >= 0; j--) {
			//colors[count++] = (char) ((this->getHeight(i,j)>0?this->getHeight(i,j):0) * 255.0);
			colors[count++] = (char) ((this->getHeight(i,j) - this->minHeight) / divider * 255.0);
		}

	ofs.write(colors,count);
	ofs.close();
}
}
