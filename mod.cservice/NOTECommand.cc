/**
 * NOTESCommand.cc
 * Leave a note for another user account, or read your own Notes.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: NOTECommand.cc,v 1.8 2007/08/28 16:10:11 dan_karrels Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"Network.h"
#include	"cservice.h"
#include	"cservice_config.h"

namespace gnuworld
{

using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

bool NOTECommand::Exec( iClient* theClient, const string& Message )
{

#ifndef USE_NOTES
	return true;
#endif

/* Is the user authorised? */
sqlUser* theUser = bot->isAuthed(theClient, false);
if(!theUser)
	{
	return false;
	}

/* Only let admins play for now */
int level = bot->getAdminAccessLevel(theUser);
if(!level) return false;

StringTokenizer st( Message ) ;
if( st.size() < 3 )
	{
	Usage(theClient);
	return true;
	}

/*
 * Sending a Note?
 */
if (string_lower(st[1]) == "send")
{
	/*
	 * First, check this person exists.
	 */
	sqlUser* targetUser = bot->getUserRecord(st[2]);
	string message = st.assemble(3);

	if (!targetUser)
		{
		bot->Notice(theClient, "I don't know who %s is.",
			st[2].c_str());
		return false;
		}

	/*
	 * They do? Great! lets see if we're currently allowed to send
	 * a note.
	 *
	 * Firstly, does the target wish to have a nice quiet inbox?
	 */
	if (targetUser->getFlag(sqlUser::F_NONOTES))
		{
		bot->Notice(theClient, "%s doesn't accept Notes.",
			targetUser->getUserName().c_str());
			return false;
		}

	/*
	 * Have we exceeded the maximum number of Notes we can
	 * send right now?
	 */

	if( (unsigned int)(bot->currentTime() - theUser->getLastNote()) >= bot->getConfnoteDuration() )
		{
		theUser->setLastNote(bot->currentTime());
		theUser->setNotesSent(0);
		}

	if( ((unsigned int)(bot->currentTime() - theUser->getLastNote()) <= bot->getConfnoteDuration()) && (theUser->getNotesSent() >= bot->getConfnoteLimit()) )
		{
		bot->Notice(theClient, "You have exceeded the maximum number of notes you can send at this time, please try later.");
		return false;
		}

	/*
	 * Dump the note into the database.
	 */

	/* .. SQL here .. */

	static const char* queryHeader = "INSERT INTO notes (user_id,from_user_id,message,last_updated) VALUES (";

	stringstream queryString;
	queryString	<< queryHeader
				<< targetUser->getID() << ", "
				<< theUser->getID() << ", '"
				<< escapeSQLChars(message) << "', "
				<< "date_part('epoch', CURRENT_TIMESTAMP)::int);"
				<< ends;

	if( !bot->SQLDb->Exec(queryString ) )
		{
		LOG( ERROR, "NOTECommand SQL Error:") ;
		LOGSQL_ERROR( bot->SQLDb ) ;

		bot->Notice(theClient, "An unknown error occured delivering the note.");
		return false ;
		}

	bot->Notice(theClient, "Successfully delivered message to %s!",
		targetUser->getUserName().c_str());

	bot->noticeAllAuthedClients(targetUser, "%s has just sent you a note. Type /msg %s notes read all to read it.",
		theUser->getUserName().c_str(), bot->getNickName().c_str());

	theUser->setNotesSent(theUser->getNotesSent() + 1);
	return true;
}

/*
 * Reading a Note?
 */
if (string_lower(st[1]) == "read")
{
	if(string_lower(st[2]) == "all")
	{
	/*
	 * Perform a query to list all notes belonging to this user.
	 */
	stringstream allNotesQuery;
	allNotesQuery	<< "SELECT users.user_name, notes.message, notes.last_updated, message_id "
					<< "FROM notes,users "
					<< "WHERE notes.from_user_id = users.id "
					<< "AND notes.user_id = "
					<< theUser->getID()
					<< " ORDER BY notes.last_updated ASC"
					<< ends;

	if( !bot->SQLDb->Exec( allNotesQuery, true ) )
//	if( PGRES_TUPLES_OK != status )
		{
		LOGSQL_ERROR( bot->SQLDb ) ;

		bot->Notice(theClient, "An unknown error occured while reading your notes.");
		return false ;
		}

	if (bot->SQLDb->Tuples() <= 0)
		{
		bot->Notice(theClient, "You have no notes.");
		return false;
		}

	unsigned int noteCount = bot->SQLDb->Tuples();

	for (unsigned int i = 0 ; i < noteCount; i++)
		{
		string from = bot->SQLDb->GetValue(i,0);
		string theMessage = bot->SQLDb->GetValue(i,1);
		unsigned int when = atoi(bot->SQLDb->GetValue(i,2));
		unsigned int message_id = atoi(bot->SQLDb->GetValue(i,3));

		bot->Notice(theClient, "\002NOTE\002 (Message-Id: %i): Recieved from %s, %s ago : %s",
			message_id, from.c_str(), prettyDuration(when).c_str(), theMessage.c_str());
		}

	}

	bot->Notice(theClient, "To erase an individual note, type /msg %s notes erase <message-id>. To erase all your notes, type /msg %s notes erase all",
		bot->getNickName().c_str(), bot->getNickName().c_str());

	return true;
}

/*
 * Erasing a Note?
 */
if (string_lower(st[1]) == "erase")
{
	if(string_lower(st[2]) == "all")
		{
		stringstream queryString;
		queryString	<< "DELETE FROM notes where user_id = "
					<< theUser->getID()
					<< ends;

		if( !bot->SQLDb->Exec(queryString ) )
			{
			LOG( ERROR, "NOTECommand SQL Error:") ;
			LOGSQL_ERROR( bot->SQLDb ) ;
			bot->Notice(theClient, "An unknown error occured while deleting your notes.");
			return false;
			}

		bot->Notice(theClient, "Successfully erased all your notes.");
		return true;
		}

	/*
	 * TOFINISH: Delete by message-id.
	 */

	unsigned int messageId = atoi(st[2].c_str());
	if(!messageId)
		{
		bot->Notice(theClient, "Invalid message-id.");
		return false;
		}

		stringstream queryString;
		queryString	<< "DELETE FROM notes where user_id = "
					<< theUser->getID()
					<< " AND message_id = "
					<< messageId
					<< ends;

		if( !bot->SQLDb->Exec(queryString, true ) )
			{
			LOG( ERROR, "NOTECommand SQL Error:") ;
			LOGSQL_ERROR( bot->SQLDb ) ;
			bot->Notice(theClient, "An error occured while deleting note-id %i.", messageId);
			return false;
			}

		bot->Notice(theClient, "Successfully erased note with message-id %i.", messageId);
		return true;

	return true;
}


return true ;
}

} // namespace gnuworld.
