#include "Planet.h"
#include "PlanetGraphics.h"
#include "NameGenerator.h"
#include "random.h"
#include <set>
#include <utility>

using namespace planet;

int main() {
	  initRandom();

	// size, roughness, maxh, sealevel, number countries, smooth K, smooth iters, seed
	Planet m = Planet(8,0.6,8000,0.5,200,0.3,5,704976229);
	Planet m2 = Planet("/home/leiterboss/Desktop/realMapOut.bmp",0.5);
	PlanetGraphics pg1 = PlanetGraphics(&m);
	PlanetGraphics pg2 = PlanetGraphics(&m2);
	//m->saveHeightMap("/home/leiterboss/Desktop/realMapOut.bmp");
	//m->saveToFile("/home/leiterboss/Desktop/realMapOut.bin");
	//	m->saveToFile("/home/leiterboss/Desktop/map.bin");
//Map* m = new Map("/home/leiterboss/Desktop/map.bin");
	//srand(2324);
	//NameGenerator* ng = new NameGenerator("/home/leiterboss/countryNamesES.txt",2,6);

	//int NAMES = 30;
	//for(int i = 0; i < NAMES; i++)
	//	cout << ng->generateName() << endl;

	//m->exportPoints("/home/leiterboss/Desktop/mapH.obj",MODE_HEIGHT,1,1,9);
	//m->exportPoints("/home/leiterboss/Desktop/mapM.obj",MODE_MOISTURE,1,1,7);
	pg1.exportPoints("/home/leiterboss/Desktop/mapC.obj",MODE_COUNTRIES,1,1,7);
	pg2.exportPoints("/home/leiterboss/Desktop/mapC2.obj",MODE_COUNTRIES,1,1,7);
	//m->exportPointsGlobe("/home/leiterboss/Desktop/mapG.obj",MODE_HEIGHT,1);
    //system( "/usr/bin/g3dviewer /home/leiterboss/Desktop/mapH.obj &");
    //system( "/usr/bin/g3dviewer /home/leiterboss/Desktop/mapM.obj &");
    system( "/usr/bin/g3dviewer /home/leiterboss/Desktop/mapC.obj &");
    system( "/usr/bin/g3dviewer /home/leiterboss/Desktop/mapC2.obj &");
    //system( "/usr/bin/g3dviewer /home/leiterboss/Desktop/mapG.obj &");
}
