#include "cDatabase.h"

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


using namespace std;
using namespace sql;


cDatabase::cDatabase(void)
{
}

cDatabase::~cDatabase(void)
{
	con->close();
    delete con;
}

XMLElement cDatabase::getNearestObjectXML(string ObjectID)
{
    XMLElement nearestObjectXML;

	return nearestObjectXML;
}

mapKeypointDB cDatabase::getKeypointFromDB()
{
	
	try 
	{
			stmt = con->createStatement();
			res = stmt->executeQuery ("SELECT object_id, keypoint FROM tbl_sift");
			string strKeypoint;
			string object_id;
			
			while (res->next()) 
		    {  
					object_id = res->getString("OBJECT_ID");
					strKeypoint = res->getString("KEYPOINT");
				    KeypointDataDB.insert(make_pair(object_id,strKeypoint));
					
			}
		     
			delete res;
		    delete stmt;
		    //con->close();
		    //delete con;
			
	} 

	catch (SQLException &e) 
	{
		cout << "ERROR: SQLException in " << __FILE__;
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) 
		{
			cout << "\nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		
	} 
	catch (std::runtime_error &e) 
	{

		cout << "ERROR: runtime_error in " << __FILE__;
		cout << "ERROR: " << e.what() << endl;

		
	}

	return KeypointDataDB;

}

int cDatabase::connectToHost(string server, string username, string password)
{
		 try {
			driver = get_driver_instance();
			con = driver->connect(server, username, password);
			//con->setSchema(database);
			} 
			catch (SQLException &e) 
			{
				cout << "ERROR: SQLException in " << __FILE__;
				cout << "ERROR: " << e.what();
				cout << " (MySQL error code: " << e.getErrorCode();
				cout << ", SQLState: " << e.getSQLState() << ")" << endl;

				if (e.getErrorCode() == 1047) 
				{
					cout << "\nYour server does not seem to support Prepared Statements at all. ";
					cout << "Perhaps MYSQL < 4.1?" << endl;
				}

				return EXIT_FAILURE;
			} 
			catch (std::runtime_error &e) 
			{

				cout << "ERROR: runtime_error in " << __FILE__;
				cout << "ERROR: " << e.what() << endl;

				return EXIT_FAILURE;
			}

		return EXIT_SUCCESS;
}

string cDatabase::getDataFromDB(string object_id)
{
	string nama_object;

	try 
	{
			
			//res = stmt->executeQuery ("SELECT object_id, keypoint FROM tbl_sift");
			prep_stmt = con -> prepareStatement (" SELECT nama_object FROM tbl_object where object_id=?");
		    prep_stmt -> setString (1, object_id);
			//cout << "Before execute " << endl;
			res = prep_stmt ->executeQuery();
			//cout << "Before execute " << endl;
			
			while (res->next()) 
		    {  
					nama_object = res->getString("nama_object");
					//cout << "nama object :" << nama_object << endl;
					
			}
		     
			delete res;
		    delete prep_stmt;		
			
	} 

	catch (SQLException &e) 
	{
		cout << "ERROR: SQLException in " << __FILE__;
		cout << "ERROR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << ")" << endl;

		if (e.getErrorCode() == 1047) 
		{
			cout << "\nYour server does not seem to support Prepared Statements at all. ";
			cout << "Perhaps MYSQL < 4.1?" << endl;
		}

		
	} 
	catch (std::runtime_error &e) 
	{

		cout << "ERROR: runtime_error in " << __FILE__;
		cout << "ERROR: " << e.what() << endl;

		
	}

    return nama_object;
	
}

void cDatabase::setMySQLDatabase(const string databasename)
{
     con->setSchema(databasename);
}

const string cDatabase::getMySQLDatabase()
{
     return database;
}
