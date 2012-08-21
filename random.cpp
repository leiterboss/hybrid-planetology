/*
 * random
 *
 *  Created on: Jun 26, 2012
 *      Author: leiterboss
 */

#include "random.h"

float uniformDistValue(int begin, int end) {
	if (begin >= end) end = begin + 1;


	float ret = rand() % (end - begin) + begin;

	  return ret;
}

void initRandom() {
	srand ( time(NULL) );
}
