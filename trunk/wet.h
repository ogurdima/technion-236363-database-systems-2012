#ifndef __WET_H_
#define __WET_H_

/*==========================================================*/
/*  -------- DBMS course, 236363, Spring 2012 -----------*/
/*==========================================================*/
/* This is a sample header file for the "wet" assignment.   */
/* This file should not be submitted, it'll be replaced     */
/* during the automatic tests. Therefore, DO NOT change     */
/* anything in this file except your USERNAME and PASSWORD. */

/* Update these two macros to your account's details */
#define USERNAME "soberdim"
#define PASSWORD "326814910"

/* DO NOT change the names of the macros -      */
/* your submitted program will fail compilation */
#define ILL_PARAMS             "Illegal parameters\n"
#define ILL_INVITATION         "Illegal invitation\n"
#define EXISTING_RECORD		   "Already exists\n"
#define EMPTY                  "Empty\n"

/* Function names */
#define COMMAND_GET_POINTS		"getPoints"
#define COMMAND_NEW_MEMBER		"newMember"
#define COMMAND_NEW_INVITED_MEMBER	"newInvitedMember"
#define COMMAND_FORBID_MEMBERSHIP	"forbidMembership"
#define COMMAND_GET_TLC_PERSON		"getTlcPerson"
#define COMMAND_GET_FRIENDLIEST_PEOPLE	"getFriendliestPeople"
#define COMMAND_GET_MEMBERS_ADDRESSES	"getMembersAddresses"
#define COMMAND_PYRAMID_BONUS		"pyramidBonus"
#define COMMAND_EXIT			"exit"

#define PERSON_POINTS_HEADER		".------------------.--------------.\n" \
                                        "|    company id    |    credit    |\n" \
                                        ".------------------.--------------.\n"
#define PERSON_POINTS_LINE		"| %-16s | %-12s |\n"

#define TLC_PERSON_HEADER	    	".------------------.\n" \
                                        "|  TLC person id   |\n" \
                                        ".------------------.\n"
#define TLC_PERSON_LINE			"| %-16s |\n"

#define FRIENDLIEST_PEOPLE_HEADER	".------------------.------------------.------------------.----------.\n" \
                                        "|     person id    |    last name     |    first name    | #friends |\n" \
                                        ".------------------.------------------.------------------.----------.\n"
#define FRIENDLIEST_PEOPLE_LINE		"| %-16s | %-16s | %-16s | %-8s |\n"

#define ADDRESSES_HEADER		".--------------------------.\n" \
                                        "|      member's address    |\n" \
                                        ".--------------------------.\n"
#define ADDRESSES_LINE			"| %-24s |\n"

void getPoints(const int personId);
void newMember(const int personId, const int companyId);
void newInvitedMember(const int personId, const int companyId, const int invitedBy);
void forbidMembership(const int personId, const int companyId);
void getTlcPerson(const int companyId);
void getFriendliestPeople(const unsigned peopleNum);
void getMembersAddresses(const int companyId);
void pyramidBonus(const int companyId, const unsigned height);

void parseInput();

#endif  /* __WET_H_ */
