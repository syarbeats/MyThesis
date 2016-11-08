//#include <shellapi.h>

#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>
#include <map>

#include "cSift.h"
#include "cDatabase.h"


#include <sys/timeb.h>    // ftime(), struct timeb

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include <winsock2.h>



#define DBHOST "tcp://127.0.0.1:3306"
#define USER "root"
#define PASSWORD ""
#define DATABASE "MOBEE"
#define SERVER "192.168.1.2"
#define MESSAGE "This message sent by server"

#define DEFAULT_PORT        95000
#define DEFAULT_BUFFER      4096

int    iPort      = DEFAULT_PORT; // Port to listen for clients on

BOOL   bInterface = FALSE,   // Listen on the specified interface
       bRecvOnly  = FALSE;   // Receive data only; don't echo back
char   szAddress[128];       // Interface to listen for clients on
string KeypointFile;
int           ret;
vector<string> v ;
char          szBuff[DEFAULT_BUFFER];
cSift *mSift;
cDatabase *mDatabase;
typedef map<string,int> siftMap;
typedef map<string,string> keypointMap;
siftMap siftObject;
keypointMap KeypointDataFromDB;
Keypoint KeypointFromFile=NULL, KeypointFromDB=NULL;
typedef map<string, int> mapMatchesKeypoint; 
mapMatchesKeypoint	KeypointMatched;
string ConvertImageFileToPGM;
string ExecuteSift;
string imsi,imei,latitude,longitude,date,OperatorName;
string tmpSift;


using namespace std;

typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

class StringTokenizer {

public:

   StringTokenizer(const string& s, const char* delim = NULL) :
      str_(s), count_(-1), begin_(0), end_(0) {

      if (!delim)
         delim_ = " \f\n\r\t\v";  //default to whitespace
      else
         delim_ = delim;

      // Point to the first token
      begin_ = str_.find_first_not_of(delim_);
      end_ = str_.find_first_of(delim_, begin_);
   }
 
   StringTokenizer( ) 
   {
		count_ =-1;
		begin_ = 0;
		end_ = 0;
   }

   void process(const string& s, const char* delim = NULL)
   {
       str_ = s;	

	   if (!delim)
			delim_ = " \f\n\r\t\v";  //default to whitespace
		else
			delim_ = delim;

		// Point to the first token
		begin_ = str_.find_first_not_of(delim_);
		end_ = str_.find_first_of(delim_, begin_);
   }


   size_t countTokens( ) {
     if (count_ >= 0) // return if we've already counted
       return(count_);

     string::size_type n = 0;
     string::size_type i = 0;

     for (;;) {
        // advance to the first token
        if ((i = str_.find_first_not_of(delim_, i)) == string::npos)
           break;
        // advance to the next delimiter
        i = str_.find_first_of(delim_, i+1);
        n++;
        if (i == string::npos)
          break;
     }
     return (count_ = n);
   }
   bool hasMoreTokens( ) {return(begin_ != end_);}
   void nextToken(string& s) {
     if (begin_ != string::npos && end_ != string::npos) {
        s = str_.substr(begin_, end_-begin_);
        begin_ = str_.find_first_not_of(delim_, end_);
        end_ = str_.find_first_of(delim_, begin_);
     }
     else if (begin_ != string::npos && end_ == string::npos)
     {
        s = str_.substr(begin_, str_.length( )-begin_);
        begin_ = str_.find_first_not_of(delim_, end_);
     }

   }

private:
   //StringTokenizer( ) {};
   string delim_;
   string str_;
   int count_;
   int begin_;
   int end_;
};



inline u32 timeGetCurrentTime()
{
#ifdef _WIN32
    _timeb t;
    _ftime(&t);
#else
    timeb t;
    ftime(&t);
#endif

    return (u32)(t.time*1000+t.millitm);
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
    SOCKET        sock=(SOCKET)lpParam;
    char          sendBuff[DEFAULT_BUFFER];
    int           nLeft,
                  idx;
	string        argument;
	string url = DBHOST;
	const string user= USER;
	const string password=PASSWORD;
	const string database = DATABASE;


	mDatabase->connectToHost(url,user,password);
	mDatabase->setMySQLDatabase(database);
	string strArgumen;

	system(ExecuteSift.c_str());
	system(tmpSift.c_str());
		
		
      
	float totalMatchingTime;
	int maxMatches = 0;
	cout << "Matching process is starting...." << endl;
	{
			u32 basetime = timeGetCurrentTime();

			float retrieveTime;
			cout << "Retrieve all Keypoint from database...." << endl;
			{
				u32 basetime = timeGetCurrentTime();
				KeypointDataFromDB = mDatabase->getKeypointFromDB();
				retrieveTime = (timeGetCurrentTime()-basetime)*0.001f;
			}
			
			cout <<"Retrieve all Keypoint was done in " << retrieveTime << " seconds." << endl;
			cout << endl;

			float readingTime;
			cout << "Reading all Keypoint from file...." << endl;
			{
				u32 basetime = timeGetCurrentTime();
				KeypointFromFile = mSift->readKeypointFromFile(KeypointFile); 
				readingTime = (timeGetCurrentTime()-basetime)*0.001f;
			}

			cout <<"Reading all Keypoint from file was done in " << readingTime << " seconds." << endl;
			cout << endl;

			int matched;
			
			float fproctime;

			keypointMap::iterator pos;
			for (pos = KeypointDataFromDB.begin(); pos != KeypointDataFromDB.end(); ++pos) 
			{
				   //cout << pos->second << endl;
				   //cout << "Object ID :" << pos->first << endl;
				KeypointFromDB = mSift->readKeypointFromDB(pos->second);
				
				cout << "Find Matching between current object with database template...[" << pos->first <<"]" << endl;
				{
						u32 basetime = timeGetCurrentTime();
						matched = mSift->FindMatches(KeypointFromFile,KeypointFromDB);
						fproctime = (timeGetCurrentTime()-basetime)*0.001f;
				}

				KeypointMatched.insert(make_pair(pos->first,matched));
				maxMatches = (matched>maxMatches)? matched : maxMatches;

				cout <<"Matching process was done in " << fproctime << " seconds." << endl;
				cout << endl;

			}
			    
			totalMatchingTime = (timeGetCurrentTime()-basetime)*0.001f;
	}

	mapMatchesKeypoint::iterator pos;
		
	for (pos = KeypointMatched.begin(); pos != KeypointMatched.end(); ++pos) 
	{
		if(pos->second==maxMatches)
		{
				cout <<"Total processing time on Matching process is " << totalMatchingTime << " seconds. With maxmatches" << maxMatches 
					 <<"With OBJECT_ID" << pos->first << endl;
				cout << "Nama Object :" << mDatabase->getDataFromDB(pos->first) << endl;
				              
				strcpy(sendBuff,url.c_str());
				//strcpy(sendBuff,mDatabase->getDataFromDB(pos->first).c_str());
		        ret = send(sock, sendBuff, strlen(sendBuff), 0);
		}
	}
		
	cout << endl;
   
    return 0;
}


int main(int argc, char** argv)
{
	
	WSADATA       wsd;
    SOCKET        sListen,
                  sClient;
    int           iAddrSize;
    HANDLE        hThread;
    DWORD         dwThreadId;
    struct sockaddr_in local,
                       client;

	string tmp,tmpToken;

	mSift = new cSift();
	mDatabase = new cDatabase();
	StringTokenizer st;  
	string::size_type idx;
	
    int nCount = 0;
   
    if (WSAStartup(MAKEWORD(2,2), &wsd) != 0)
    {
        printf("Failed to load Winsock!\n");
        return 1;
    }

    
    sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (sListen == SOCKET_ERROR)
    {
        printf("socket() failed: %d\n", WSAGetLastError());
        return 1;
    }
    
    strcpy(szAddress,SERVER);
    local.sin_addr.s_addr = inet_addr(szAddress);    
    local.sin_family = AF_INET;
    local.sin_port = htons(DEFAULT_PORT);

    if (bind(sListen, (struct sockaddr *)&local, 
            sizeof(local)) == SOCKET_ERROR)
    {
        printf("bind() failed: %d\n", WSAGetLastError());
        return 1;
    }
    listen(sListen, 8);
    
    while (1)
    {
        iAddrSize = sizeof(client);
        sClient = accept(sListen, (struct sockaddr *)&client,
                        &iAddrSize);        
        if (sClient == INVALID_SOCKET)
        {        
            printf("accept() failed: %d\n", WSAGetLastError());
            break;
        }
        printf("Accepted client: %s:%d\n", 
            inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        ret = recv(sClient, szBuff, DEFAULT_BUFFER, 0);
        
        if (ret == SOCKET_ERROR)
        {
            printf("recv() failed: %d\n", WSAGetLastError());
            break;
        }
        szBuff[ret] = '\0';
        printf("RECV: '%s'\n", szBuff);
        
		tmp = szBuff;
        st.process(tmp,"|");

        while (st.hasMoreTokens( )) 
		{
			st.nextToken(tmpToken);
			//cout << "token = " << nCount <<"="<< tmpToken << '\n';
	  
			if(nCount==0)
			{
				tmpSift=tmpToken;
				cout <<"Execute Sift= " << tmpToken <<endl;
			}
			else if(nCount==1)
			{
				imsi=tmpToken;
				cout <<"imsi= " << tmpToken <<endl;
			}
            else if(nCount==2)
			{
				imei=tmpToken;
				cout <<"imei= " << tmpToken <<endl;
			}
			else if(nCount==3)
			{
				latitude=tmpToken;
				cout <<"latitude= " << tmpToken <<endl;
			}
            else if(nCount==4)
			{
				longitude=tmpToken;
				cout <<"longitude= " << tmpToken <<endl;
			}
			else if(nCount==5)
			{
				OperatorName=tmpToken;
				cout <<"OperatorName= " << tmpToken <<endl;
			}
			else if(nCount==6)
			{
				date=tmpToken;
				cout <<"date= " << tmpToken <<endl;
			}

			nCount++;
		}
 

		idx = tmpSift.find('>');
		
        KeypointFile = tmpSift.substr(idx+2,strlen(tmp.c_str())+1); 
		cout << KeypointFile << endl;
		ConvertImageFileToPGM = "convert "+ KeypointFile.substr(0,KeypointFile.find('.'))+".jpg " + KeypointFile.substr(0,KeypointFile.find('.'))+".pgm\n";
		//ExecuteSift = "convert "+ KeypointFile.substr(0,KeypointFile.find('.')-1)+".jpg" + KeypointFile.substr(0,KeypointFile.find('.')-1)+".pgm \n";
		ExecuteSift = ConvertImageFileToPGM ;
        cout <<ConvertImageFileToPGM <<endl;

        hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sClient, 0, &dwThreadId);
        if (hThread == NULL)
        {
            printf("CreateThread() failed: %d\n", GetLastError());
            break;
        }
        
    }
    
	CloseHandle(hThread);
    closesocket(sListen);
    
    WSACleanup();

	return 0;
}

