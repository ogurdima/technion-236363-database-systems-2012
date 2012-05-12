#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "wet.h"

#define MAIN__DBG

#ifdef MAIN__DBG
#define DEBUG(...) printf(__VA_ARGS__);
#endif
#ifndef MAIN__DBG
#define DEBUG(...) ;
#endif

#define FL 8

void testCallAllFunctions();
void connectToDb();
void freeAllConnections();
void firstQuery();
void printPgTable(PGresult* tbl);
int printPgTitle(PGresult* tbl);
int getPgMaxColStrSize(PGresult* tbl, int i);


PGconn *conn = NULL;

int main(int argc, char* argv[]) {
	DEBUG("Called: main\n");
	
	connectToDb();
	
	testCallAllFunctions();
	
	firstQuery();
	
	freeAllConnections();
	return 0;
}

void connectToDb()
{
	char buff[1024] = "";
	sprintf(buff, "host=pgsql.cs.technion.ac.il dbname=%s user=%s password=%s", USERNAME, USERNAME, PASSWORD);
	conn = PQconnectdb(buff);
	if (!conn || PQstatus(conn) == CONNECTION_BAD) {
		fprintf(stdout, "Connection to server failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}
}

void freeAllConnections()
{
	PQfinish(conn);
	conn = NULL;
}

void testCallAllFunctions()
{
	getPoints(0);
	newMember(0,0);
	newInvitedMember(0,0,0);
	forbidMembership(0,0);
	getTlcPerson(0);
	getFriendliestPeople(0);
	getMembersAddresses(0);
	pyramidBonus(0,0);
}

void firstQuery()
{
	PGresult* res = NULL;
	
	res = PQexec(conn, "select * from books");
	if (!res || PQresultStatus(res) != PGRES_TUPLES_OK) 
	{
		fprintf(stdout, "Error executing query: %s\n", PQresultErrorMessage(res)); PQclear(res);
		return;
	}
	DEBUG("Succeeded: firstQuery\n");
	printPgTable(res);
	
	PQclear(res);
	res = NULL;
}

void printPgTable(PGresult* tbl)
{
	DEBUG("Called: printPgTable\n");
	int row = 0;
	int col = 0;
	int i = 0;
	char tableString[1024*1024] = "";
	char horizontalSeparator[1024] = "";
	char* field = NULL;
	char rowString[1024] = "";
	char sizeFix[1024] = "";
	
	int titleStringSize = printPgTitle(tbl);
	
	for (i = 0; i < titleStringSize; i++)
	{
		horizontalSeparator[i] = '=';
	}
	horizontalSeparator[i] = '\0';
	
	for (row = 0; row < PQntuples(tbl); row++)
	{
		DEBUG("ROW: %d\n", row);
		for (col = 0; col < PQnfields(tbl); col++)
		{
			DEBUG("\tCOL: %d\n", col);
			
			int sizeOfEntry = getPgMaxColStrSize(tbl, col) + 2;
			
			field = PQgetvalue(tbl, row, col);
			for (i = 0; i < sizeOfEntry - strlen(field); i++)
			{
				sizeFix[i] = ' ';
			}
			sizeFix[i] = '\0';
			
			if (! strcmp(rowString, ""))
			{
				sprintf(rowString, "%s%s", field, sizeFix);
			}
			else
			{
				sprintf(rowString, "%s | %s%s", rowString, field, sizeFix);
			}
			
		}
		//DEBUG("ROW STR: %s\n", rowString);
	
		sprintf(tableString, "%s| %s |\n%s\n", tableString, rowString, horizontalSeparator);
		rowString[0] = '\0';
	}
	printf("%s\n", horizontalSeparator);
	printf("%s\n", tableString);
}

int printPgTitle(PGresult* tbl)
{
	DEBUG("Called: printPgTitle\n");
	int col = 0;
	int j = 0;
	char titleString[1024] = "";
	char fixedTitle[1024] = "";
	char* title = NULL;
	char sizeFix[1024] = "";
	DEBUG("BEFORE LOOP\n");
	for (col = 0; col < PQnfields(tbl); col++)
	{
		int sizeOfTitle = getPgMaxColStrSize(tbl, col) + 2;
		DEBUG("LOOP:");
		DEBUG("ITER: %d...", col);
		title = PQfname(tbl, col);
		DEBUG("PQfname:TITLE='%s'...", title);
		for (j = 0; j < sizeOfTitle - strlen(title); j++)
		{
			sizeFix[j] = ' ';
		}
		sizeFix[j] = '\0';
		if (! strcmp(titleString, ""))
		{
			DEBUG("if strcmp...");
			sprintf(titleString, "%s%s", title, sizeFix);
		}
		else
		{
			DEBUG("else strcmp...");
			sprintf(titleString, "%s | %s%s", titleString, title, sizeFix);
		}
		DEBUG("\n");
	}
	sprintf(fixedTitle, "| %s |", titleString);
	
	printf("%s\n", fixedTitle);
	return strlen(fixedTitle);
}

int getPgMaxColStrSize(PGresult* tbl, int col)
{
	int row = 0;
	int maxSize = 0;
	char* val = NULL;
	char* title = NULL;
	title = PQfname(tbl, col);
	maxSize = strlen(title);
	for (row = 0; row < PQntuples(tbl); row++)
	{
		//DEBUG("\tMAX: row=%d,col=%d\n", row, col);
		val = PQgetvalue(tbl, row, col);
		if (NULL == val)
			continue;
		maxSize = ( strlen(val) > maxSize ) ? strlen(val) : maxSize;
	}
	//DEBUG("MAX: Finished! returning %d\n", maxSize);
	return maxSize;
}
