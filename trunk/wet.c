#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "wet.h"

//#define WET__DBG

#ifdef WET__DBG
#define DEBUG(...) printf(__VA_ARGS__); 
#endif
#ifndef WET__DBG
#define DEBUG(...) ;
#endif

PGconn* conn;

#define SAFE_SELECT(res, query) 	res = PQexec((conn), (query)); 							\
									if (!res || (PQresultStatus(res) != PGRES_TUPLES_OK)) 	\
									{														\
										fprintf(stdout, "Error executing query: %s\n",		\
										PQresultErrorMessage(res)); PQclear(res);			\
										freeAllConnections();								\
										exit(1);											\
									}

#define SAFE_DDL_DML(res, query) 	res = PQexec((conn), (query)); 							\
									if (!res || (PQresultStatus(res) != PGRES_COMMAND_OK)) 	\
									{														\
										fprintf(stdout, "Error executing query: %s\n",		\
										PQresultErrorMessage(res)); PQclear(res);			\
										freeAllConnections();								\
										exit(1);											\
									}														

#define QUERY_CHECK_NOT_EXISTS(res, errtype) 	if (0 == PQntuples(res))		\
												{								\
													printf(errtype);			\
													PQclear(res); res = NULL;	\
													return;						\
												}

#define QUERY_CHECK_EXISTS(res, errtype) 	if (0 != PQntuples(res))		\
											{								\
												printf(errtype);			\
												PQclear(res); res = NULL;	\
												return;						\
											}

//indentCorrection

//#ifndef WET__DBG

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

int main(int argc, char* argv[])
{
	DEBUG("Parse Start\n");
	connectToDb();
	parseInput();
	freeAllConnections();
	DEBUG("Parse End\n");
	return 0;
}

//#endif

void getPoints(const int personId) {
	PGresult* res = NULL;
	int row = 0;
	int col = 0;
	char queryBuff[1024] = "";
	DEBUG("Called: getPoints\n");
	sprintf(queryBuff, "select * from person where pid = %d", personId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	//==========================================================================
	sprintf(queryBuff, "select cid, points from memberships where pid = %d order by cid", personId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, EMPTY);
	//==========================================================================
	printf(PERSON_POINTS_HEADER);
	for (row = 0; row < PQntuples(res); row++) {
		printf(PERSON_POINTS_LINE, PQgetvalue(res, row, 0), PQgetvalue(res,
				row, 1));
	}
	PQclear(res);
	res = NULL;
}

void newMember(const int personId, const int companyId) {
	PGresult* res = NULL;
	int row = 0;
	int col = 0;
	char queryBuff[1024] = "";
	DEBUG("Called: newMember\n");
	//==========================================================================
	sprintf(queryBuff, "select * from person where pid = %d", personId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newMember: person exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newMember: company exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from forbidden where pid = %d AND cid = %d",
			personId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newMember: not forbidden\n");
	//==========================================================================
	sprintf(queryBuff, "select * from memberships where pid = %d AND cid = %d",
			personId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_EXISTS(res, EXISTING_RECORD);
	PQclear(res);
	res = NULL;
	DEBUG("newMember: record does not exist\n");
	//==========================================================================
	sprintf(queryBuff,
			"insert into memberships (pid, cid, points) values (%d, %d, 0)",
			personId, companyId);
	DEBUG("Q: %s\n", queryBuff);
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res);
	res = NULL;
}

void newInvitedMember(const int personId, const int companyId, const int invitedBy) {
	PGresult* res = NULL;
	int row = 0;
	int col = 0;
	char queryBuff[1024] = "";
	
	//==========================================================================
	sprintf(queryBuff, "select * from person where pid = %d", personId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newInvitedMember: invited guy exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newInvitedMember: company exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from person where pid = %d", invitedBy);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newInvitedMember: invitee guy exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from forbidden where pid = %d and cid = %d",
			personId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newInvitedMember: not forbidden\n");
	//==========================================================================
	
	//==========================================================================
	sprintf(queryBuff, "select * from memberships where pid = %d and cid = %d", invitedBy, companyId);
	DEBUG("newInvited: %s\n", queryBuff);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_INVITATION);
	PQclear(res); res = NULL;
	DEBUG("newInvitedMember: man is a fool\n");
	if (personId == invitedBy)
	{
		printf(ILL_INVITATION);
		PQclear(res); res = NULL;
		return;
	}
	//==========================================================================
	sprintf(queryBuff, "select * from memberships where pid = %d and cid = %d", personId, companyId);
	DEBUG("newInvited: %s\n", queryBuff);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_EXISTS(res, EXISTING_RECORD);
	PQclear(res); res = NULL;
	DEBUG("newInvitedMember: record does not exist\n");
	//==========================================================================
	sprintf(queryBuff, "insert into memberships (pid, cid, points, invitedby) values (%d, %d, 0, %d)", personId, companyId, invitedBy);
	DEBUG("Q: %s\n", queryBuff);
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res); res = NULL;
}

void forbidMembership(const int personId, const int companyId) {
	PGresult* res = NULL;
	char queryBuff[1024] = "";
	DEBUG("Called: forbidMembership\n");
	//==========================================================================
	sprintf(queryBuff, "select * from person where pid = %d", personId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("forbidMembership: person exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("forbidMembership: company exists\n");
	//==========================================================================
	sprintf(queryBuff, "select * from forbidden where pid = %d AND cid = %d",
			personId, companyId);
	SAFE_SELECT(res, queryBuff);
	if (1 == PQntuples(res)) {
		PQclear(res);
		res = NULL;
		return;
	}
	PQclear(res);
	res = NULL;
	DEBUG("forbidMembership: membership is not forbidden\n");
	//==========================================================================
	sprintf(queryBuff, "delete from memberships where pid = %d AND cid = %d",
			personId, companyId);
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res);
	res = NULL;
	DEBUG("forbidMembership: membership is deleted\n");
	//==========================================================================
	sprintf(queryBuff, "insert into forbidden (pid, cid) values(%d, %d)",
			personId, companyId);
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res);
	res = NULL;
	DEBUG("forbidMembership: membership is forbidden\n");
}

void getTlcPerson(const int companyId) {

	//Right way: select pid from memberships m1 where 2 = (select count (distinct m2.points) from memberships m2 where m2.points > m1.points);
	PGresult* res = NULL;
	char queryBuff[1024] = "";
	char*  val = NULL;
	int row = 0;
	DEBUG("Called: getTlcPerson\n");
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("getTlcPerson: company exists\n");
	//==========================================================================
	
	sprintf(queryBuff, "select distinct points from (select * from memberships where cid = %d) as fmsps order by points desc", companyId);
	SAFE_SELECT(res, queryBuff);
	if (3 > PQntuples(res))		
	{								
		printf(EMPTY);			
		PQclear(res); res = NULL;	
		return;						
	}
	PQclear(res);
	res = NULL;
	//==========================================================================
	sprintf(queryBuff, 
	"\
select pid \
from memberships m1 \
where 2 = (\
	select count (distinct m2.points) \
	from memberships m2 \
	where 	m2.points > m1.points \
			and m1.cid = m2.cid \
			and m1.cid = %d \
	) \
order by pid", companyId);
	SAFE_SELECT(res, queryBuff);
	printf(TLC_PERSON_HEADER);
	for (row = 0; row < PQntuples(res); row++) 
	{
		printf(TLC_PERSON_LINE, PQgetvalue(res, row, 0));
	}
	PQclear(res);
	res = NULL;
}

void getFriendliestPeople(const unsigned peopleNum) {
	PGresult* res = NULL;
	char queryBuff[1024] = "";
	char*  val = NULL;
	int row = 0;
	DEBUG("Called: getFriendliestPeople\n");
	sprintf(queryBuff, " \
select p1.pid, p1.lastname, p1.firstname, count(distinct(q1.pid) ) as num \
from person p1 left outer join memberships q1 \
on \
( q1.invitedby = p1.pid) \
or \
(q1.pid in (select m.invitedby from memberships m where m.pid = p1.pid and m.invitedby is not null)) \
group by \
	p1.pid, p1.lastname, p1.firstname \
order by \
	num desc , p1.pid \
limit %d \
	", peopleNum);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, EMPTY);
	printf(FRIENDLIEST_PEOPLE_HEADER);
	for (row = 0; row < PQntuples(res); row++) 
	{
		printf(FRIENDLIEST_PEOPLE_LINE, 
			PQgetvalue(res, row, 0),
			PQgetvalue(res, row, 1), 
			PQgetvalue(res, row, 2), 
			PQgetvalue(res, row, 3)
		);
	}
	PQclear(res);
	res = NULL;
}

void getMembersAddresses(const int companyId) {
	
	PGresult* res = NULL;
	char queryBuff[1024] = "";
	char*  val = NULL;
	int row = 0;
	DEBUG("Called: getMembersAddresses\n");
	
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("getMembersAddresses: company exists\n");
	//==========================================================================
	sprintf(queryBuff, " \
	select distinct \
	p.address  \
from  \
	person p  \
where ( \
	p.pid in  \
		( ( select  \
				m.pid  \
			from  \
				memberships m  \
			where  \
				m.cid = %d  \
		) )  \
	and p.address not in \
		( ( select  \
				p1.address  \
			from  \
				person p1, forbidden f  \
			where  \
				f.cid = %d and f.pid = p1.pid  \
		) ) \
) \
order by p.address \
	", companyId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, EMPTY);
	printf(ADDRESSES_HEADER);
	for (row = 0; row < PQntuples(res); row++)
	{
		printf(ADDRESSES_LINE, PQgetvalue(res, row, 0));
	}
}

void pyramidBonus(const int companyId, const unsigned height)
{
	PGresult* res = NULL;
	char queryBuff[1024*1024*4] = "";
	char singleQuery[1024] = "";
	char parenthesesFix[1024] = "";
	char*  val = NULL;
	int i = 0;
	int row = 0;
	DEBUG("Called: pyramidBonus, height=%d\n", height);
	
	//==========================================================================
	sprintf(queryBuff, "select * from company where cid = %d", companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("getMembersAddresses: company exists\n");
	//==========================================================================
	
	if (0 == height)
	{
		return;
	}
	//==========================================================================
	// Building the query
	sprintf(singleQuery, "(select s.pid from memberships s where s.cid = %d and (s.invitedby=memberships.pid", companyId);
	sprintf(queryBuff, "\
update memberships set points = ( select sum( coalesce (points, 0) ) \
from memberships s \
where s.cid = %d and ( s.pid=memberships.pid", companyId);
	for (i = height; i > 0; i--)
	{
		if (i == height)
		{
			// First call is on s.pid, not on invitedby
			sprintf(queryBuff, "%s or s.pid in %s", queryBuff, singleQuery);
		}
		else
		{
			sprintf(queryBuff, "%s or s.invitedby in %s", queryBuff, singleQuery);
		}
		//signleQuery opens two parentheses
		sprintf(parenthesesFix, "%s))", parenthesesFix);
	}
	sprintf(queryBuff, "%s))", queryBuff);
	sprintf(queryBuff, "%s%s where cid = %d", queryBuff, parenthesesFix, companyId);
	DEBUG("\n\n%s\n\n", queryBuff);
	//==========================================================================
	// Executing the query
	
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res);
	res = NULL;
	
}



