/*
 * NameGenerator.h
 *
 *  Created on: Jul 12, 2012
 *      Author: leiterboss
 */

#ifndef NAMEGENERATOR_H_
#define NAMEGENERATOR_H_

#include <vector>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <utility>
#include <set>

#define N 1
#define NUM_LETTERS 27	// Including end of word

using namespace std;

class NameGenerator {
private:
	map<string, double> startingFreqs;
	map<string, map<string,double> > ngraphFreqs;
	set<string> originalNames;
	int order, similarity;
	//int nGraphSize;

	void addEntry(const char* prev, string next);
	void addStartingEntry(const char* prev);
	void calculateProbabilities();
	string toLowercase(string s);
	string toUppercase(string s);
	string getNextPiece(string s);
	string getStartingPiece();
	int longestCommonSubstring(string& str1, string& str2);
	int maximumOriginalSimilarity(string& s1);

public:
	NameGenerator(string path, int order, int similarity);
	void printTable();
	string generateName();
	virtual ~NameGenerator();
};

#endif /* NAMEGENERATOR_H_ */
