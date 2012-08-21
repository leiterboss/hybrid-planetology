/*
 * PlanetGraphics.h
 *
 *  Created on: Aug 21, 2012
 *      Author: leiterboss
 */

#ifndef PLANETGRAPHICS_H_
#define PLANETGRAPHICS_H_

#include "Planet.h"
#include <boost/serialization/string.hpp>
#include <fstream>
#include <sstream>

namespace planet {

class PlanetGraphics {
private:
	Planet* planet;
	POINT flatToSphere(int x, int y, int px, int py, float h);
	float getRadius(int x, int y, int polex, int poley) {
		POINT p = this->flatToSphere(x,y,polex,poley,this->planet->getHeight(x,y));
		return sqrt(p.x*p.x + p.y*p.y + p.z*p.z) * 2 * PI / this->planet->getSizeOrder();
	}
public:
	PlanetGraphics(Planet* p) {this->planet = p;};
	virtual ~PlanetGraphics();
	void exportPoints( string path, int mode = MODE_HEIGHT, int copiesW = 1, int copiesH = 1, float heightMod = 5);
	void exportPointsGlobe(string path, int mode = MODE_HEIGHT, float heightMod = 5);
};

} /* namespace planet */
#endif /* PLANETGRAPHICS_H_ */
