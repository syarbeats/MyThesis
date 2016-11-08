#pragma once


#include <string>

/* MySQL Connector/C++ specific headers */
#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <metadata.h>
#include <resultset_metadata.h>
#include <exception.h>
#include <warning.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <list>
#include "cSift.h"
#include "XMLParser.h"

typedef map<string, string> mapKeypointDB;

using namespace std;
using namespace sql;

class cDatabase
{
	public:
		cDatabase(void);
		~cDatabase(void);
		int connectToHost(string server, string username, string password);
		void setMySQLDatabase(const string databasename);
		const string getMySQLDatabase();
		mapKeypointDB getKeypointFromDB();
		string getDataFromDB(string object_id);
		XMLElement getNearestObjectXML(string ObjectID);


	private:
		Driver *driver;
		Connection *con;
		Statement *stmt;
		ResultSet *res;
		PreparedStatement *prep_stmt;
		int num, len;

		string server;
		const string user;
		const string password;
		const string database;
		
		mapKeypointDB KeypointDataDB;
		list<string> listKeypoint;
		XMLElement dialogueElement;
		


};
