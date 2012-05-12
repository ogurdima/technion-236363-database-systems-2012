#include <stdio.h>
#include <string.h>
#include "wet.h"

#define MAX_LINE_SIZE 128

static char command_name[MAX_LINE_SIZE + 2],command_line[MAX_LINE_SIZE + 2];
static int intv[3], argc, missing_argc;

static void goto_next_line(FILE* file) {
	while (!feof(file) && fgetc(file) != '\n');
}

static int check_command(char* expected_name, int expected_argc) {
	return (!strcmp(command_name,expected_name) && (missing_argc = expected_argc - argc) <= 0);
}

void parseInput() {
	while (!feof(stdin)) {
		if (fgets(command_line,MAX_LINE_SIZE + 2,stdin) == NULL) break;
		if (strlen(command_line) > MAX_LINE_SIZE) {
			printf("Command too long, can not parse\n");
			goto_next_line(stdin);		
		}
		else {
			missing_argc = 0;
			sprintf(command_name,"");
			argc = sscanf(command_line,"%s %d,%d,%d",command_name,&(intv[0]),&(intv[1]),&(intv[2]));
			if (check_command(COMMAND_GET_POINTS,2)) getPoints(intv[0]);
			else if (check_command(COMMAND_NEW_MEMBER,3)) newMember(intv[0],intv[1]);
			else if (check_command(COMMAND_NEW_INVITED_MEMBER,4)) newInvitedMember(intv[0],intv[1],intv[2]);
			else if (check_command(COMMAND_FORBID_MEMBERSHIP,3)) forbidMembership(intv[0],intv[1]);
			else if (check_command(COMMAND_GET_TLC_PERSON,2)) getTlcPerson(intv[0]);
			else if (check_command(COMMAND_GET_FRIENDLIEST_PEOPLE,2)) getFriendliestPeople(intv[0]);
			else if (check_command(COMMAND_GET_MEMBERS_ADDRESSES,2)) getMembersAddresses(intv[0]);
			else if (check_command(COMMAND_PYRAMID_BONUS,2)) pyramidBonus(intv[0],intv[1]);
			else if (check_command(COMMAND_EXIT,1)) break;
			else if (missing_argc > 0) printf("Missing %d argument(s) in command %s!\n",missing_argc,command_name);
			else if (argc > 0) printf("Illegal command!\n");
		}
	}
}
