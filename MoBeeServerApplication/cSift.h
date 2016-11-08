#pragma once

#include <vector>
#include <string>
//#include "cDatabase.h"

typedef struct KeypointSt 
{
	float row, col;             /* Subpixel location of keypoint. */
	float scale, ori;           /* Scale and orientation (range [-PI,PI]) */
	unsigned char *descrip; 
	//void *descrip;     /* Vector of descriptor values */
	struct KeypointSt *next;    /* Pointer to next keypoint in list. */
} *Keypoint;

using namespace std;

class cSift
{
	public:
		cSift(void);
		~cSift(void);
		vector<string> explode( const string &delimiter, const string &explodeme);
        Keypoint readKeypointFromDB(string strKeypoint);
		Keypoint readKeypointFromFile(string filename);
		int DistSquared(Keypoint k1, Keypoint k2);
		Keypoint CheckForMatch(Keypoint key, Keypoint klist);
		int FindMatches(Keypoint keys1, Keypoint keys2);
		void SkipComments(FILE *fp);
		string getObjectName(string object_id);
		//cDatabase *mDatabase;


		

};
