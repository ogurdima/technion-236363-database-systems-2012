#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>
#include "wet.h"

#define WET__DBG

#ifdef WET__DBG
#define DEBUG(...) printf(__VA_ARGS__);
#endif
#ifndef WET__DBG
#define DEBUG(...) ;
#endif

extern PGconn* conn;

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

///aaaa


///aaaa
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

void newInvitedMember(const int personId, const int companyId,
		const int invitedBy) {
	PGresult* res = NULL;
	int row = 0;
	int col = 0;
	char queryBuff[1024] = "";
	DEBUG("Called: newInvitedMember\n");
	
	//==========================================================================
	sprintf(queryBuff, "select * from person where pid = %d", invitedBy);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ILL_PARAMS);
	PQclear(res);
	res = NULL;
	DEBUG("newInvitedMember: invite guy exists\n");
	//==========================================================================
	
	sprintf(queryBuff, "select * from memberships where pid = %d and cid = %d", personId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_EXISTS(res, EXISTING_RECORD);
	PQclear(res); res = NULL;
	DEBUG("newInvitedMember: record does not exist\n");
	//==========================================================================
	
	newMember(personId, companyId);
	sprintf(queryBuff, "select * from memberships where pid = %d and cid = %d", personId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, ""); //Does not print anything
	PQclear(res); res = NULL;
	DEBUG("newInvitedMember: new record exists\n");
	//==========================================================================
	sprintf(queryBuff, "delete from memberships where pid = %d and cid = %d", personId, companyId);
	DEBUG("Q: %s\n", queryBuff);
	SAFE_DDL_DML(res, queryBuff);
	PQclear(res); res = NULL;
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

	//Right way: select * from memberships m1 where 2 = (select count (distinct m2.points) from memberships m2 where m2.points > m1.points);
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
	val = PQgetvalue(res, 2, 0);
	PQclear(res);
	res = NULL;
	//==========================================================================
	sprintf(queryBuff, "select pid from memberships where cid = %d and points = %s order by pid", companyId, val);
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
select pid, lastname, firstname, \
count( (select distinct m.pid from memberships m where m.invitedby = person.pid) ) + \
count( (select distinct m.invitedby from memberships m where m.pid = person.pid ) ) \
as num  \
from  \
	person  \
group by \
	pid, lastname, firstname \
order by \
	num desc , pid \
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
	select  \
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
	", companyId, companyId);
	SAFE_SELECT(res, queryBuff);
	QUERY_CHECK_NOT_EXISTS(res, EMPTY);
	printf(ADDRESSES_HEADER);
	for (row = 0; row < PQntuples(res); row++) 
	{
		printf(ADDRESSES_LINE, PQgetvalue(res, row,                                                                                                                                                                                                                                     