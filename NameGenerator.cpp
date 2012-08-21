/*
 * NameGenerator.cpp
 *
 *  Created on: Jul 12, 2012
 *      Author: leiterboss
 */

#include "NameGenerator.h"

NameGenerator::NameGenerator(string path, int ord, int simil) {
	ifstream sample;
	string word;
	const char* letters;

	this->order = ord;
	this->similarity = simil;

	this->ngraphFreqs.clear();
	this->originalNames.clear();

	sample.open(path.c_str());
    while ( sample.good() ) {
        getline(sample,word);
        if (word.compare("") == 0) break;

        this->originalNames.insert(toLowercase(word));

        word = toLowercase(word) + "{";
        letters = word.c_str();

        this->addStartingEntry(letters);
        for(unsigned int i = 0; i < word.size() - this->order; i++) {
        	string n; n.assign(letters + i + this->order, letters + i + this->order + 1);
        	this->addEntry(letters+i, n);
        }
    }
    this->calculateProbabilities();
}

void NameGenerator::addStartingEntry(const char* prev) {
	string key;
	key.assign(prev,prev+this->order);
	map<string,double>::iterator target, target2;
	target = this->startingFreqs.find(key);

	if (target == this->startingFreqs.end()) {
		this->startingFreqs.insert(make_pair(key, 1 ));
	}
	else {
		target->second++;
	}
}

void NameGenerator::addEntry(const char* prev, string next) {
	string key;
	key.assign(prev,prev+this->order);
	map<string,map<string,double> >::iterator target, target2;
	target = this->ngraphFreqs.find(key);

	if (target == this->ngraphFreqs.end()) {
		map<string,double> fr;
		fr.insert(make_pair(next,1));
		this->ngraphFreqs.insert(make_pair(key, fr ));
	}
	else {
		map<string,double>::iterator target2 = target->second.find(next);
		if (target2 == target->second.end()) {
			target->second.insert(make_pair(next,1));
		}
		else {
			target2->second++;
		}
	}
}

void NameGenerator::calculateProbabilities() {
	map<string,map<string,double> >::iterator iter = this->ngraphFreqs.begin();
	int count;

	while (iter != this->ngraphFreqs.end()) {
		count = 0;
		map<string,double>::iterator iter2 = iter->second.begin();
		while (iter2 != iter->second.end()) {
			count += iter2->second;
			iter2++;
		}

		iter2 = iter->second.begin();
		while (iter2 != iter->second.end()) {
			iter2->second /= count;
			iter2++;
		}

		iter++;
	}

	map<string,double>::iterator iter3 = this->startingFreqs.begin();
	count = 0;

	while (iter3 != this->startingFreqs.end()) {
		count += iter3->second;
		iter3++;
	}

	iter3 = this->startingFreqs.begin();

	while (iter3 != this->startingFreqs.end()) {
		iter3->second /= count;
		iter3++;
	}
}

string NameGenerator::generateName() {
	int spaceCounter = 0;

	string prev = "{";
	string cur = this->getStartingPiece();
	string ret = "";
	ret += cur;
	prev = ret.substr(ret.size() - this->order);
	while (prev.compare("{") != 0) {
		cur = this->getNextPiece(prev);

		if (cur.compare(" ") == 0) spaceCounter++;

		if (spaceCounter > 1 || cur.compare("{") == 0) break;

		ret += cur;
		if (cur.compare("?") == 0) return "ERROR";
		prev = ret.substr(ret.size() - this->order);
	}
	if (	this->originalNames.find(ret) != this->originalNames.end()
			|| ret.size() < 4
			|| this->maximumOriginalSimilarity(ret) > this->similarity) {
		ret = generateName();
	}
	return ret;
}

string NameGenerator::getStartingPiece() {
	float rnd = (float) rand() / double(RAND_MAX);
	map <string,double>::iterator iter = this->startingFreqs.begin();

	while (iter != this->startingFreqs.end()) {
		rnd -= iter->second;
		if (rnd < 0.0001) return iter->first;
		iter++;
	}
	return "?";
}

string NameGenerator::getNextPiece(string prev) {
	float rnd = (float) rand() / double(RAND_MAX);
	map<string,map<string,double> >::iterator iter;
	map <string,double>::iterator iter2;

	iter = this->ngraphFreqs.find(prev);

	if (iter == this->ngraphFreqs.end()) return "?";

	iter2 = iter->second.begin();
	while (iter2 != iter->second.end()) {
		rnd -= iter2->second;
		if (rnd < 0.0001) return iter2->first;
		iter2++;
	}

	return "?";
}

void NameGenerator::printTable() {
	map<string,map<string,double> >::iterator iter = this->ngraphFreqs.begin();

	while (iter != this->ngraphFreqs.end()) {
		map<string,double>::iterator iter2 = iter->second.begin();
		cout << iter->first << endl;
		while (iter2 != iter->second.end()) {
			cout << " " << iter2->first << ": " << iter2->second << endl;
			iter2++;
		}

		iter++;
	}
}

string NameGenerator::toLowercase(string s) {
	transform(s.begin(), s.end(),s.begin(), ::tolower);
	return s;
}

string NameGenerator::toUppercase(string s) {
	transform(s.begin(), s.end(),s.begin(), ::toupper);
	return s;
}

int NameGenerator::maximumOriginalSimilarity(string& s1) {
	set<string>::iterator iter = this->originalNames.begin();
	int max,cur;
	string s;
	max = 0;

	while (iter != this->originalNames.end()) {
		s = *iter;
		cur = this->longestCommonSubstring(s1,s);
		if (cur > max) max = cur;

		iter++;
	}
	return max;
}

int NameGenerator::longestCommonSubstring(string& str1, string& str2)
{
     if(str1.empty() || str2.empty())
     {
          return 0;
     }

     int *curr = new int [str2.size()];
     int *prev = new int [str2.size()];
     int *swap;
     int maxSubstr = 0;

     for(unsigned int i = 0; i<str1.size(); ++i)
     {
          for(unsigned int j = 0; j<str2.size(); ++j)
          {
               if(str1[i] != str2[j])
               {
                    curr[j] = 0;
               }
               else
               {
                    if(i == 0 || j == 0)
                    {
                         curr[j] = 1;
                    }
                    else
                    {
                         curr[j] = 1 + prev[j-1];
                    }
                    //The next if can be replaced with:
                    //maxSubstr = max(maxSubstr, curr[j]);
                    //(You need algorithm.h library for using max())
                    if(maxSubstr < curr[j])
                    {
                         maxSubstr = curr[j];
                    }
               }
          }
          swap=curr;
          curr=prev;
          prev=swap;
     }
     delete [] curr;
     delete [] prev;
     return maxSubstr;
}

NameGenerator::~NameGenerator() {
	// TODO Auto-generated destructor stub
}

