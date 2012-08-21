/*
 * PlanetGraphics.cpp
 *
 *  Created on: Aug 21, 2012
 *      Author: leiterboss
 */

#include "PlanetGraphics.h"

namespace planet {

PlanetGraphics::~PlanetGraphics() {
	// TODO Auto-generated destructor stub
}


void PlanetGraphics::exportPoints(string path, int mode, int copiesW, int copiesH, float heightMod) {
	  ofstream f;
	  int totalX = this->planet->getSizeOrder() + (copiesH - 1)*(this->planet->getSizeOrder() - 1);
	  int totalZ = 2*(this->planet->getSizeOrder()) - 1 + (copiesW - 1)*(2*(this->planet->getSizeOrder() - 1));
	//  VECTOR normal;
	//  POINT center;

//	  VECTOR normals[this->planet->getSizeOrder()][this->planet->getSizeOrder()];

	  f.open (path.c_str());

	   f.setf(ios::fixed, ios::floatfield);
	   f.setf(ios::showpoint);


	   f << "mtllib mats.mtl" << endl << endl << /*"usemtl terrain" << */endl << "s 1" << endl << "o terrain" << endl;

		for(int i = 0; i < totalX; i++) {
			for(int j = 0; j < totalZ; j++) {
				f << "v " 	<< i - (totalX - 1)/2 << " "
							<< setprecision(10) << setw(10) <<  this->planet->getHeight(i,j)*heightMod << " "
							<< j - (totalZ - 1)/2 << endl;
			}
		}

	f << endl << endl;

	f << 	"v " << 0 - (totalX - 1)/2 << " 0 " << 0 - (totalZ - 1)/2 << endl <<
			"v " << 0 - (totalX - 1)/2 << " 0 " << totalZ - 1 - (totalZ - 1)/2  << endl <<
			"v " << totalX - 1 - (totalX - 1)/2 << " 0 " << totalZ - 1 - (totalZ - 1)/2  << endl <<
					"v " << totalX - 1 - (totalX - 1)/2 << " 0 " << 0 - (totalZ - 1)/2 << endl;


	f << endl << endl;

	int typeTerrain;

	for(int i = 0; i < this->planet->getSizeOrder() + (copiesH - 1)*(this->planet->getSizeOrder() - 1) - 1/*copiesH*(this->planet->getSizeOrder())-1*/; i++) {
		for(int j = 0; j < 2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(2*(this->planet->getSizeOrder() - 1)); j++) {
			if (j != 0) {

				if (mode == MODE_HEIGHT) {
					switch (typeTerrain) {
						case 0: f << "usemtl vhighterrain" << endl; break;
						case 1: f << "usemtl highterrain" << endl; break;
						case 2: f << "usemtl medterrain" << endl; break;
						case 3: f << "usemtl lowterrain" << endl; break;
						case 4: f << "usemtl vlowterrain" << endl; break;
						//case 5: f << "usemtl countryMarker" << endl; break;
					}
				}
				else if (mode == MODE_MOISTURE) {
					switch (typeTerrain) {
						case 0: f << "usemtl vhighmoisture" << endl; break;
						case 1: f << "usemtl highmoisture" << endl; break;
						case 2: f << "usemtl medmoisture" << endl; break;
						case 3: f << "usemtl lowmoisture" << endl; break;
						case 4: f << "usemtl vlowmoisture" << endl; break;
					}
				}
				else if (mode == MODE_COUNTRIES) {
					if (typeTerrain == 0) f << "usemtl nocountry" << endl;
					else if (typeTerrain == 1) f << "usemtl nocountry" << endl;
					else f << "usemtl country" << typeTerrain - 2 << endl;
				}

				f << "f "
							<< i*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 << /*"//" << i*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 <<*/ " "
							<< (i+1)*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 << /*"//" << (i+1)*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 <<*/ " "
							<< (i+1)*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j - 1 + 1 << /*"//" << (i+1)*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j - 1 + 1 <<*/ endl;
			}
			if (j != 2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(2*this->planet->getSizeOrder() - 1) - 1) {
				float curHeight = (this->planet->getHeight(i,j) + this->planet->getHeight(i+1,j) + this->planet->getHeight(i,j+1)) / 3;
				float curMoisture = (this->planet->getMoisture(i,j) + this->planet->getMoisture(i+1,j) + this->planet->getMoisture(i,j+1)) / 3;
				int countryNum;
				if (this->planet->getCountry(i,j) < 0 && this->planet->getCountry(i+1,j) < 0 && this->planet->getCountry(i,j+1) < 0)
					countryNum = COUNTRY_NONE;
				else {
					if (this->planet->getCountry(i,j) > this->planet->getCountry(i+1,j) && this->planet->getCountry(i,j) > this->planet->getCountry(i,j+1)) countryNum = this->planet->getCountry(i,j);
					else if (this->planet->getCountry(i+1,j) > this->planet->getCountry(i,j+1)) countryNum = this->planet->getCountry(i+1,j);
					else countryNum = this->planet->getCountry(i,j+1);
				}

				if (mode == MODE_HEIGHT) {

					if (curHeight * this->planet->getMaxHeight() > 5000) typeTerrain = 0;
					else if (curHeight * this->planet->getMaxHeight() > 3500) typeTerrain = 1;
					else if (curHeight * this->planet->getMaxHeight() > 2000) typeTerrain = 2;
					else if (curHeight * this->planet->getMaxHeight() > 50) typeTerrain = 3;
					else typeTerrain = 4;

					//if (isCountry) typeTerrain = 5;

					switch (typeTerrain) {
						case 0: f << "usemtl vhighterrain" << endl; break;
						case 1: f << "usemtl highterrain" << endl; break;
						case 2: f << "usemtl medterrain" << endl; break;
						case 3: f << "usemtl lowterrain" << endl; break;
						case 4: f << "usemtl vlowterrain" << endl; break;
					//	case 5: f << "usemtl countryMarker" << endl; break;
					}
				}
				else if (mode == MODE_MOISTURE) {
					if (curMoisture > MOISTURE_VHIGH) typeTerrain = 0;
					else if (curMoisture > MOISTURE_HIGH) typeTerrain = 1;
					else if (curMoisture > MOISTURE_MED) typeTerrain = 2;
					else if (curMoisture > MOISTURE_LOW) typeTerrain = 3;
					else typeTerrain = 4; // MOISTURE_VLOW

					switch (typeTerrain) {
						case 0: f << "usemtl vhighmoisture" << endl; break;
						case 1: f << "usemtl highmoisture" << endl; break;
						case 2: f << "usemtl medmoisture" << endl; break;
						case 3: f << "usemtl lowmoisture" << endl; break;
						case 4: f << "usemtl vlowmoisture" << endl; break;
					}
				}
				else if (mode == MODE_COUNTRIES) {
					if (countryNum == COUNTRY_NONE) typeTerrain = 0;
					else if (countryNum == COUNTRY_WATER) typeTerrain = 1;
					else typeTerrain = (countryNum % 10) + 2;

					if (typeTerrain == 0) f << "usemtl nocountry" << endl;
					else if (typeTerrain == 1) f << "usemtl nocountry" << endl;
					else f << "usemtl country" << typeTerrain - 2 << endl;
				}

				f << "f "
							<< i*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 << /*"//" << i*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 << */" "
							<< i*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 + 1 << /*"//" << i*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 + 1 <<*/ " "
							<< (i+1)*(2*this->planet->getSizeOrder() - 1 + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 << /*"//" << (i+1)*(this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1)) + j + 1 <<*/ endl;
			}
		}
	}

	f << endl << "usemtl sea" << endl;

	int npnts = (2*this->planet->getSizeOrder() - 1)*this->planet->getSizeOrder() + 1;
	f << "f " << npnts << " " << npnts + 1 << " " << npnts + 2 << " " << npnts + 3 << endl;

	  f.close();
}

void PlanetGraphics::exportPointsGlobe(string path, int mode, float heightMod) {
	  ofstream f;
	  VECTOR pnt;
//	  VECTOR normals[this->planet->getSizeOrder()][this->planet->getSizeOrder()];

	  // Find point under water which opposite is also under water (if it exists)
/*	  polex = poley = -1;
	  poley = 0;
	  int cont, maxCont;
	  maxCont = 0;
	  for(int i = 0; i < this->planet->getSizeOrder(); i++) {
		  cont = 0;
		  for(int j = 0; j < 2*(this->planet->getSizeOrder() - 1); j++) {
			  if (this->planet->getTileType(i,j) == TYPE_WATER
					//  && this->planet->getTileType(i+(this->planet->getSizeOrder()-1)/2, j+(this->planet->getSizeOrder()-1)/2) == TYPE_WATER) {
				  //polex = i; poley = j;
				  cont++;
			  }
		  }
		  if (cont > maxCont) polex = i;
	  }
	  if (polex == -1) return; // Pole doesn't exist, abort
*/
	  f.open (path.c_str());

	   f.setf(ios::fixed, ios::floatfield);
	   f.setf(ios::showpoint);

	   f << "mtllib mats.mtl" << endl << endl << endl << "s 1" << endl << "o terrain" << endl;


		for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
			pnt = this->flatToSphere(0,j,0,0,this->planet->getHeight(0,j)*heightMod);
			f << "v " 	<< pnt.x << " "
						<< pnt.y << " "
						<< pnt.z << endl;
		}
		for(int i = 0; i < this->planet->getSizeOrder(); i++) {
			for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
				pnt = this->flatToSphere(i+1,j,0,0,this->planet->getHeight(i,j)*heightMod);
				f << "v " 	<< pnt.x << " "
							<< pnt.y << " "
							<< pnt.z << endl;
			}
		}
		for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
			pnt = this->flatToSphere(this->planet->getSizeOrder()+1,j,0,0,this->planet->getHeight(this->planet->getSizeOrder()-1,j)*heightMod);
			f << "v " 	<< pnt.x << " "
						<< pnt.y << " "
						<< pnt.z << endl;
		}

/*
		for(int i = 0; i < this->planet->getSizeOrder() + (copiesH - 1)*(this->planet->getSizeOrder() - 1); i++) {
			for(int j = 0; j < this->planet->getSizeOrder() + (copiesW - 1)*(this->planet->getSizeOrder() - 1); j++) {
				center.x = i; center.y = j; center.z = this->planet->getHeight(i,j);
				normal = this->planet->calculateVertexNormal(center);
				f << "vn " << normal.x << " " << normal.y << " " << normal.z << endl;
				normals[i][j] = normal;
			}
		}
*/
	f << endl << endl;

	for(int i = 0; i < this->planet->getSizeOrder()+2; i++) {
		for(int j = 0; j < 2*(this->planet->getSizeOrder()-1); j++) {
			pnt = this->flatToSphere(i,j,0,0,0);
			f << "v " 	<< pnt.x << " "
						<< pnt.y << " "
						<< pnt.z << endl;
		}
	}

	f << endl << endl;

	int typeTerrain;

	for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
				if (j != 0) {
					if (mode == MODE_HEIGHT) {
						switch (typeTerrain) {
							case 0: f << "usemtl vhighterrain" << endl; break;
							case 1: f << "usemtl highterrain" << endl; break;
							case 2: f << "usemtl medterrain" << endl; break;
							case 3: f << "usemtl lowterrain" << endl; break;
							case 4: f << "usemtl vlowterrain" << endl; break;
						}
					}
					else if (mode == MODE_MOISTURE) {
						switch (typeTerrain) {
							case 0: f << "usemtl vhighmoisture" << endl; break;
							case 1: f << "usemtl highmoisture" << endl; break;
							case 2: f << "usemtl medmoisture" << endl; break;
							case 3: f << "usemtl lowmoisture" << endl; break;
							case 4: f << "usemtl vlowmoisture" << endl; break;
						}
					}

					f << "f "	<< j + 1 << " "
								<< 2*(this->planet->getSizeOrder())-1 + j + 1 << " "
								<< 2*(this->planet->getSizeOrder())-1 + j - 1 + 1 << endl;
				}
				if (j != 2*(this->planet->getSizeOrder())-2) {
					float curHeight = (this->planet->getHeight(0,j) + this->planet->getHeight(1,j) + this->planet->getHeight(0,j+1)) / 3;
					float curMoisture = (this->planet->getMoisture(0,j) + this->planet->getMoisture(1,j) + this->planet->getMoisture(0,j+1)) / 3;

					if (mode == MODE_HEIGHT) {
						if (curHeight * this->planet->getMaxHeight() > 5000) typeTerrain = 0;
						else if (curHeight * this->planet->getMaxHeight() > 3500) typeTerrain = 1;
						else if (curHeight * this->planet->getMaxHeight() > 2000) typeTerrain = 2;
						else if (curHeight * this->planet->getMaxHeight() > 50) typeTerrain = 3;
						else typeTerrain = 4;

						switch (typeTerrain) {
							case 0: f << "usemtl vhighterrain" << endl; break;
							case 1: f << "usemtl highterrain" << endl; break;
							case 2: f << "usemtl medterrain" << endl; break;
							case 3: f << "usemtl lowterrain" << endl; break;
							case 4: f << "usemtl vlowterrain" << endl; break;
						}
					}
					else if (mode == MODE_MOISTURE) {
						if (curMoisture > MOISTURE_VHIGH) typeTerrain = 0;
						else if (curMoisture > MOISTURE_HIGH) typeTerrain = 1;
						else if (curMoisture > MOISTURE_MED) typeTerrain = 2;
						else if (curMoisture > MOISTURE_LOW) typeTerrain = 3;
						else typeTerrain = 4; // MOISTURE_VLOW

						switch (typeTerrain) {
							case 0: f << "usemtl vhighmoisture" << endl; break;
							case 1: f << "usemtl highmoisture" << endl; break;
							case 2: f << "usemtl medmoisture" << endl; break;
							case 3: f << "usemtl lowmoisture" << endl; break;
							case 4: f << "usemtl vlowmoisture" << endl; break;
						}
					}

					f << "f "
								<< j + 1 << " "
								<< j + 1 + 1 << " "
								<< 2*(this->planet->getSizeOrder())-1 + j + 1 << endl;
				}
			}

	for(int i = 0; i < this->planet->getSizeOrder()-1; i++) {
		for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
			if (j != 0) {
				if (mode == MODE_HEIGHT) {
					switch (typeTerrain) {
						case 0: f << "usemtl vhighterrain" << endl; break;
						case 1: f << "usemtl highterrain" << endl; break;
						case 2: f << "usemtl medterrain" << endl; break;
						case 3: f << "usemtl lowterrain" << endl; break;
						case 4: f << "usemtl vlowterrain" << endl; break;
					}
				}
				else if (mode == MODE_MOISTURE) {
					switch (typeTerrain) {
						case 0: f << "usemtl vhighmoisture" << endl; break;
						case 1: f << "usemtl highmoisture" << endl; break;
						case 2: f << "usemtl medmoisture" << endl; break;
						case 3: f << "usemtl lowmoisture" << endl; break;
						case 4: f << "usemtl vlowmoisture" << endl; break;
					}
				}

				f << "f "	<< i*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder()-1 << " "
							<< (i+1)*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder()-1 << " "
							<< (i+1)*(2*(this->planet->getSizeOrder())-1) + j - 1 + 1 + 2*this->planet->getSizeOrder()-1 << endl;
			}
			if (j != 2*(this->planet->getSizeOrder())-2) {
				float curHeight = (this->planet->getHeight(i,j) + this->planet->getHeight(i+1,j) + this->planet->getHeight(i,j+1)) / 3;
				float curMoisture = (this->planet->getMoisture(i,j) + this->planet->getMoisture(i+1,j) + this->planet->getMoisture(i,j+1)) / 3;

				if (mode == MODE_HEIGHT) {
					if (curHeight > 0.8) typeTerrain = 0;
					else if (curHeight > 0.6) typeTerrain = 1;
					else if (curHeight > 0.3) typeTerrain = 2;
					else if (curHeight > 0.02) typeTerrain = 3;
					else typeTerrain = 4;

					switch (typeTerrain) {
						case 0: f << "usemtl vhighterrain" << endl; break;
						case 1: f << "usemtl highterrain" << endl; break;
						case 2: f << "usemtl medterrain" << endl; break;
						case 3: f << "usemtl lowterrain" << endl; break;
						case 4: f << "usemtl vlowterrain" << endl; break;
					}
				}
				else if (mode == MODE_MOISTURE) {
					if (curMoisture > MOISTURE_VHIGH) typeTerrain = 0;
					else if (curMoisture > MOISTURE_HIGH) typeTerrain = 1;
					else if (curMoisture > MOISTURE_MED) typeTerrain = 2;
					else if (curMoisture > MOISTURE_LOW) typeTerrain = 3;
					else typeTerrain = 4; // MOISTURE_VLOW

					switch (typeTerrain) {
						case 0: f << "usemtl vhighmoisture" << endl; break;
						case 1: f << "usemtl highmoisture" << endl; break;
						case 2: f << "usemtl medmoisture" << endl; break;
						case 3: f << "usemtl lowmoisture" << endl; break;
						case 4: f << "usemtl vlowmoisture" << endl; break;
					}
				}

				f << "f "
							<< i*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder()-1 << " "
							<< i*(2*(this->planet->getSizeOrder())-1) + j + 1 + 1 + 2*this->planet->getSizeOrder()-1 << " "
							<< (i+1)*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder()-1 << endl;
			}
		}
	}

	for(int j = 0; j < 2*(this->planet->getSizeOrder())-1; j++) {
				if (j != 0) {
					if (mode == MODE_HEIGHT) {
						switch (typeTerrain) {
							case 0: f << "usemtl vhighterrain" << endl; break;
							case 1: f << "usemtl highterrain" << endl; break;
							case 2: f << "usemtl medterrain" << endl; break;
							case 3: f << "usemtl lowterrain" << endl; break;
							case 4: f << "usemtl vlowterrain" << endl; break;
						}
					}
					else if (mode == MODE_MOISTURE) {
						switch (typeTerrain) {
							case 0: f << "usemtl vhighmoisture" << endl; break;
							case 1: f << "usemtl highmoisture" << endl; break;
							case 2: f << "usemtl medmoisture" << endl; break;
							case 3: f << "usemtl lowmoisture" << endl; break;
							case 4: f << "usemtl vlowmoisture" << endl; break;
						}
					}

					f << "f "	<< (this->planet->getSizeOrder()-1)*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder() - 1<< " "
								<< (this->planet->getSizeOrder())*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder() - 1 << " "
								<< (this->planet->getSizeOrder())*(2*(this->planet->getSizeOrder())-1) + j - 1 + 1 + 2*this->planet->getSizeOrder() - 1 << endl;
				}
				if (j != 2*(this->planet->getSizeOrder())-2) {
					float curHeight = (this->planet->getHeight(this->planet->getSizeOrder()-2,j) + this->planet->getHeight(this->planet->getSizeOrder()-1,j) + this->planet->getHeight(this->planet->getSizeOrder()-2,j+1)) / 3;
					float curMoisture = (this->planet->getMoisture(this->planet->getSizeOrder()-2,j) + this->planet->getMoisture(this->planet->getSizeOrder()-1,j) + this->planet->getMoisture(this->planet->getSizeOrder()-2,j+1)) / 3;

					if (mode == MODE_HEIGHT) {
						if (curHeight * this->planet->getMaxHeight() > 5000) typeTerrain = 0;
						else if (curHeight * this->planet->getMaxHeight() > 3500) typeTerrain = 1;
						else if (curHeight * this->planet->getMaxHeight() > 2000) typeTerrain = 2;
						else if (curHeight * this->planet->getMaxHeight() > 50) typeTerrain = 3;
						else typeTerrain = 4;

						switch (typeTerrain) {
							case 0: f << "usemtl vhighterrain" << endl; break;
							case 1: f << "usemtl highterrain" << endl; break;
							case 2: f << "usemtl medterrain" << endl; break;
							case 3: f << "usemtl lowterrain" << endl; break;
							case 4: f << "usemtl vlowterrain" << endl; break;
						}
					}
					else if (mode == MODE_MOISTURE) {
						if (curMoisture > 0.8) typeTerrain = 0;
						else if (curMoisture > 0.6) typeTerrain = 1;
						else if (curMoisture > 0.4) typeTerrain = 2;
						else if (curMoisture > 0.2) typeTerrain = 3;
						else typeTerrain = 4;

						switch (typeTerrain) {
							case 0: f << "usemtl vhighmoisture" << endl; break;
							case 1: f << "usemtl highmoisture" << endl; break;
							case 2: f << "usemtl medmoisture" << endl; break;
							case 3: f << "usemtl lowmoisture" << endl; break;
							case 4: f << "usemtl vlowmoisture" << endl; break;
						}
					}

					f << "f "
								<< (this->planet->getSizeOrder()-1)*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder() - 1 << " "
								<< (this->planet->getSizeOrder()-1)*(2*(this->planet->getSizeOrder())-1) + j + 1 + 1 + 2*this->planet->getSizeOrder() - 1 << " "
								<< (this->planet->getSizeOrder())*(2*(this->planet->getSizeOrder())-1) + j + 1 + 2*this->planet->getSizeOrder() - 1 << endl;
				}
			}

	f << endl << "usemtl sea" << endl;

	int npnts = (2*this->planet->getSizeOrder()-1)*(this->planet->getSizeOrder()+2);
	for(int i = 0; i < this->planet->getSizeOrder()+1; i++) {
			int j;
			for(j = 0; j < 2*(this->planet->getSizeOrder()-1)+1; j++) {
				f << "f "	<< i*2*(this->planet->getSizeOrder()-1) + j + 1 + npnts << " "
						<< (i+1)*2*(this->planet->getSizeOrder()-1) + j + 1 + npnts << " "
						<< (i+1)*2*(this->planet->getSizeOrder()-1) + j - 1 + 1 + npnts << " "
						<< endl;
				if (j != 2*(this->planet->getSizeOrder()-1)) {
					f << "f "
								<< i*2*(this->planet->getSizeOrder()-1) + j + 1 + npnts << " "
								<< i*2*(this->planet->getSizeOrder()-1) + j + 1 + 1 + npnts << " "
								<< (i+1)*2*(this->planet->getSizeOrder()-1) + j + 1 + npnts << " "
								<< endl;
				}
			}
		}

	  f.close();
}


VECTOR makeVector(POINT p1, POINT p2) {
	VECTOR v; v.x = p2.x - p1.x; v.y = p2.y - p1.y; v.z = p2.z - p1.z; return v;
}
VECTOR addVectors(VECTOR v1, VECTOR v2) {
	VECTOR v; v.x = v1.x + v2.x; v.y = v1.y + v2.y; v.z = v1.z + v2.z;
	return v;
}
VECTOR crossProduct(VECTOR v1, VECTOR v2) {
	VECTOR v;
	v.x = v1.y*v2.z - v1.z*v2.y;
	v.y = v1.z*v2.x - v1.x*v2.z;
	v.z = v1.x*v2.y - v1.y*v2.x;
	return v;
}
VECTOR scale(float a, VECTOR v) {
	v.x *= a; v.y *= a; v.z *= a;
	return v;
}
VECTOR normalize(VECTOR v) {
	float norm = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
	v.x /= norm; v.y /= norm; v.z /= norm;
	return v;
}

POINT PlanetGraphics::flatToSphere(int x, int y, int px, int py, float h) {
	VECTOR ret, retN;
	float R = this->planet->getSizeOrder() / (2 * PI);
	float dTheta = PI / (this->planet->getSizeOrder()+1);
	float dPhi = - PI / (this->planet->getSizeOrder()-1);
	ret.x = 0; ret.y = R; ret.z = 0;

	//ret.x = R * sin(dPhi*x) * sin(dTheta*y);
	//ret.y = R * cos(dTheta*y);
	//ret.z = R * cos(dPhi*x) * sin(dTheta*y);
	ret.x = R * sin(dPhi*y) * sin(dTheta*x);
	ret.y = R * cos(dTheta*x);
	ret.z = R * cos(dPhi*y) * sin(dTheta*x);

	retN = normalize(ret);
	ret = addVectors(ret,scale(h,retN));

	return ret;
}

} /* namespace planet */
