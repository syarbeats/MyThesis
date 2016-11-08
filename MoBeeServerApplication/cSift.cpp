#include "cSift.h"
#include <iostream>




cSift::cSift(void)
{
	//mDatabase = new cDatabase();
}

cSift::~cSift(void)
{
	//mDatabase;
}

void cSift::SkipComments(FILE *fp)
{
    int ch;

    fscanf(fp," ");      /* Skip white space. */
    while ((ch = fgetc(fp)) == '#') {
      while ((ch = fgetc(fp)) != '\n'  &&  ch != EOF)
	;
      fscanf(fp," ");
    }
    ungetc(ch, fp);      /* Replace last character read. */
}

string cSift::getObjectName(string object_id)
{
	return "";
}

Keypoint cSift::readKeypointFromDB(string strKeypoint)
{
			 
	        string descriptor;
			string::size_type idx;
			string::size_type idy;
			int count=0;
			int begin=0;
			string dimension,feature;

	         int i, j, num, len, val;
			 Keypoint k, keys = NULL;
			 
			 
			 len=128;
	
	         while(!strKeypoint.empty())
		     //while(count < 3)
			 {
				
				idx = strKeypoint.find('\n');
                

				if(count==0)
				{
					string::size_type begIdx, endIdx;
					dimension = strKeypoint.substr(0, idx);
					//cout << "dimension :" << dimension << endl;

					vector<string> v = explode(" ", dimension);
					//for(int i=0; i<v.size(); i++)
					   //cout <<i << " ["<< v[i] <<"] " <<endl;

					num = atoi(v[0].c_str());
					//len = atoi(v[1].c_str());
					//cout << num << endl;
					
				}
				else if(count%2==0)
				{
					descriptor = strKeypoint.substr(0, idx);
					//cout << "Descriptor :" << descriptor << endl;
					k->descrip = (unsigned char*)malloc(len);
					vector<string> v = explode(" ", descriptor);

					for (j = 0; j < len; j++) 
					{
						k->descrip[j] =  (unsigned char) atoi(v[j].c_str());
					}
				
				}
				else
				{
					feature = strKeypoint.substr(0, idx); 
					//cout << "feature :" << feature << endl;
                    k = (Keypoint) malloc(sizeof(struct KeypointSt));
				    k->next = keys;
				    keys = k;

					vector<string> v = explode(" ", feature);
					//cout << "vector size: " << v.size() << endl;
					k->row   = atof(v[0].c_str());
					k->col   = atof(v[1].c_str());
				    k->scale = atof(v[2].c_str());
				    k->ori   = atof(v[3].c_str());
				}
				
				//cout <<"descriptor :" << descriptor <<endl;
				begin = idx+1;
				strKeypoint = strKeypoint.substr(begin);
				count++;


			 }

			 /*for(k=keys; k!=NULL; k=k->next)
			 {
				 cout <<" " <<  k->row <<","<<k->col <<","<<  k->scale <<","<<k->ori <<endl ;
			     
				 for(int i=0; i<len ;i++)
					cout << k->descrip[i];
				 cout <<endl;
			 }
			 */
			 
			 return keys;

}

Keypoint cSift::readKeypointFromFile(string filename)
{
	FILE *file;

	file = fopen (filename.c_str(), "r");
    if (! file)
	    cout << "Could not open file: " << filename <<endl;

	int i, j, num, len, val;
    Keypoint k, keys = NULL;
    string PATH1,PATH2,PATH3,PATH4,PATH5,PATH6;

    fscanf(file," ");
	fscanf(file,"%s", &PATH1);

    if (fscanf(file, "%d %d", &num, &len) != 2)
	    cout << "Invalid keypoint file beginning." <<endl;

    if (len != 128)
	    cout << "Keypoint descriptor length invalid (should be 128)." << endl;

    for (i = 0; i < num; i++) 
	{
		  /* Allocate memory for the keypoint. */
		  k = (Keypoint) malloc(sizeof(struct KeypointSt));
		  k->next = keys;
		  keys = k;
		  k->descrip = (unsigned char*)malloc(len);

		  if (fscanf(file, "%f %f %f %f", &(k->row), &(k->col), &(k->scale), &(k->ori)) != 4)
				cout << "Invalid keypoint file format." << endl;

		  for (j = 0; j < len; j++) 
		  {
				if (fscanf(file, "%d", &val) != 1 || val < 0 || val > 255)
					cout << "Invalid keypoint file value." <<endl;
		
				k->descrip[j] = (unsigned char) val;
		  }
	}
    
	return keys;
}

vector<string> cSift::explode( const string &delimiter, const string &str)
{
    vector<string> arr;

    int strleng = str.length();
    int delleng = delimiter.length();
    if (delleng==0)
        return arr;//no change

    int i=0;
    int k=0;
    while( i<strleng )
    {
        int j=0;
        while (i+j<strleng && j<delleng && str[i+j]==delimiter[j])
            j++;
        if (j==delleng)//found delimiter
        {
            arr.push_back(  str.substr(k, i-k) );
            i+=delleng;
            k=i;
        }
        else
        {
            i++;
        }
    }
    arr.push_back(  str.substr(k, i-k) );
    return arr;
}

int cSift::FindMatches(Keypoint keys1, Keypoint keys2)
{
    Keypoint k, match;
    
    int count = 0;

    /* Match the keys in list keys1 to their best matches in keys2.
    */
    for (k= keys1; k != NULL; k = k->next) {
      match = CheckForMatch(k, keys2);  

      /* Draw a line on the image from keys1 to match.  Note that we
	 must add row count of first image to row position in second so
	 that line ends at correct location in second image.
      */
      if (match != NULL) {
	count++;
	//DrawLine(result, (int) k->row, (int) k->col,(int) (match->row + im1->rows), (int) match->col);
      }
    }

    /* Write result image to standard output. */
    //WritePGM(stdout, result);
    fprintf(stderr,"Found %d matches.\n", count);
	
	return count;
}


/* This searches through the keypoints in klist for the two closest
   matches to key.  If the closest is less than 0.6 times distance to
   second closest, then return the closest match.  Otherwise, return
   NULL.
*/
Keypoint cSift::CheckForMatch(Keypoint key, Keypoint klist)
{
    int dsq, distsq1 = 100000000, distsq2 = 100000000;
    Keypoint k, minkey = NULL;

    /* Find the two closest matches, and put their squared distances in
       distsq1 and distsq2.
    */
    for (k = klist; k != NULL; k = k->next) {
      dsq = DistSquared(key, k);

      if (dsq < distsq1) {
	distsq2 = distsq1;
	distsq1 = dsq;
	minkey = k;
      } else if (dsq < distsq2) {
	distsq2 = dsq;
      }
    }

    /* Check whether closest distance is less than 0.6 of second. */
    if (10 * 10 * distsq1 < 6 * 6 * distsq2)
      return minkey;
    else return NULL;
}


/* Return squared distance between two keypoint descriptors.
*/
int cSift::DistSquared(Keypoint k1, Keypoint k2)
{
    int i, dif, distsq = 0;
    unsigned char *pk1, *pk2;

    pk1 = k1->descrip;
    pk2 = k2->descrip;

    for (i = 0; i < 128; i++) {
      dif = (int) *pk1++ - (int) *pk2++;
      distsq += dif * dif;
    }
    return distsq;
}

