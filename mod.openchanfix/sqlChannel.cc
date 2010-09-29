/**
 * sqlChannel.cc
 *
 * Author: Matthias Crauwels <ultimate_@wol.be>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 * USA.
 *
 * $Id: sqlChannel.cc,v 1.6 2008/01/16 02:03:39 buzlip01 Exp $
 */

#include	<sstream>
#include	<string>

#include	"dbHandle.h"

#include	"ELog.h"
#include	"misc.h"

#include	"sqlChannel.h"
#include	"chanfix.h"

namespace gnuworld
{

namespace cf
{

const sqlChannel::flagType sqlChannel::F_BLOCKED	= 0x00000001 ;
const sqlChannel::flagType sqlChannel::F_ALERT		= 0x00000002 ;

const int sqlChannel::EV_MISC		= 1  ; /* Uncategorized event */
const int sqlChannel::EV_NOTE		= 2  ; /* Miscellaneous notes */
const int sqlChannel::EV_CHANFIX	= 3  ; /* Manual chanfixes */
const int sqlChannel::EV_BLOCK		= 4  ; /* Channel block */
const int sqlChannel::EV_UNBLOCK	= 5  ; /* Channel unblock */
const int sqlChannel::EV_ALERT		= 6  ; /* Channel alert */
const int sqlChannel::EV_UNALERT	= 7  ; /* Channel unalert */
const int sqlChannel::EV_REQUESTOP	= 8  ; /* Requestops */
const int sqlChannel::EV_TEMPBLOCK	= 9  ; /* Temp channel block */
const int sqlChannel::EV_UNTEMPBLOCK	= 10 ; /* Temp channel unblock */
const int sqlChannel::EV_SIMULATE	= 11 ; /* Fix simulation */

unsigned long int sqlChannel::maxUserId = 0;

sqlChannel::sqlChannel(sqlManager* _myManager) :
  id(0),
  channel(),
  user_name(),
  last(0),
  start(0),
  maxScore(0),
  modesRemoved(false),
  flags(0),
  inSQL(false)
{
  myManager = _myManager;
}

void sqlChannel::setAllMembers(dbHandle* theDB, int row)
{
  id = atoi(theDB->GetValue(row, 0));
  channel = theDB->GetValue(row, 1);
  flags = atoi(theDB->GetValue(row, 2));
  inSQL = true;

  if (id > maxUserId) maxUserId = id;
}

/**
 * This function inserts a brand new channel into the DB.
 */
bool sqlChannel::Insert(dbHandle* cacheCon)
{
/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Grab the next available user id */
id = ++maxUserId;

/* Create the INSERT statement */
std::stringstream insertString;
insertString    << "INSERT INTO channels "
                << "(id, channel, flags) "
                << "VALUES "
                << "("
                << id << ", "
		<< "'" << escapeSQLChars(channel) << "', "
		<< flags
		<< ")"
		;

if (!cacheCon->Exec(insertString.str())) {
  elog	<< "sqlChannel::Insert> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  inSQL = false;
  maxUserId--;
} else {
  inSQL = true;
}

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return inSQL;
} // sqlChannel::Insert()

bool sqlChannel::Delete(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the DELETE statement */
std::stringstream deleteString;
deleteString    << "DELETE FROM channels "
		<< "WHERE id = '" << id << "'"
		;

if (!cacheCon->Exec(deleteString.str())) {
  elog	<< "sqlChannel::Delete> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

bool sqlChannel::commit(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the UPDATE statement */
std::stringstream chanCommit;
chanCommit	<< "UPDATE channels SET "
		<< "flags = " << flags
		<< " WHERE "
		<< "id = " << id
		;

if (!cacheCon->Exec(chanCommit.str())) {
  elog	<< "sqlChannel::commit> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

/**
 * This method writes a 'notes' record, recording an event that has
 * occured in this channel.
 */

void sqlChannel::addNote(dbHandle* cacheCon, unsigned short eventType, iClient* theUser,
	const std::string& theMessage)
{
unsigned int num_notes = countNotes(cacheCon, 0);
while (num_notes >= MAXNOTECOUNT) {
  if (!deleteOldestNote(cacheCon))
    return;
  num_notes--;
}

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the INSERT statement */
std::stringstream theLog;
theLog	<< "INSERT INTO notes (ts, channelID, user_name, event, message) "
	<< "VALUES ("
	<< "now()::abstime::int4"
	<< ", "
	<< id
	<< ", '"
	<< theUser->getAccount()
	<< "', "
	<< eventType
	<< ", "
	<< "'"
	<< escapeSQLChars(theMessage)
	<< "')"
	;

if (!cacheCon->Exec(theLog.str())) {
  elog	<< "sqlChannel::addNote> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
}

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return;
}

const std::string sqlChannel::getLastNote(dbHandle* cacheCon, unsigned short eventType, time_t& eventTime)
{
std::string retval;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Retrieve the last note */
std::stringstream queryString;
queryString	<< "SELECT message,ts"
		<< " FROM notes WHERE channelID = "
		<< id
		<< " AND event = "
		<< eventType
		<< " ORDER BY ts DESC LIMIT 1"
		;

if (cacheCon->Exec(queryString.str(),true)) {
  if (cacheCon->Tuples() > 0) {
    std::string note = cacheCon->GetValue(0, 0);
    eventTime = atoi(cacheCon->GetValue(0, 1));
    retval = note;
  }
}

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

bool sqlChannel::deleteNote(dbHandle* cacheCon, unsigned int messageId)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the DELETE statement */
std::stringstream deleteString;
deleteString	<< "DELETE FROM notes WHERE channelID = "
		<< id
		<< " AND id = "
		<< messageId
		;

if (!cacheCon->Exec(deleteString.str())) {
  elog	<< "sqlChannel::deleteNote> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

bool sqlChannel::deleteOldestNote(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Retrieve the id of the oldest note */
std::stringstream selectString;
selectString	<< "SELECT id FROM notes WHERE channelID = "
		<< id
		<< " ORDER BY ts ASC LIMIT 1"
		;

if (cacheCon->Exec(selectString.str(),true)) {
  if (cacheCon->Tuples() > 0) {
    unsigned int note_id = atoi(cacheCon->GetValue(0, 0));

    std::stringstream deleteString;
    deleteString	<< "DELETE FROM notes WHERE id = "
			<< note_id
			;

    if (cacheCon->Exec(deleteString.str(),true))
      retval = true;
  }
}

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

bool sqlChannel::deleteAllNotes(dbHandle* cacheCon)
{
bool retval = false;

/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Create the DELETE statement */
std::stringstream deleteString;
deleteString	<< "DELETE FROM notes WHERE channelID = "
		<< id
		;

if (!cacheCon->Exec(deleteString.str())) {
  elog	<< "sqlChannel::deleteAllNotes> Something went wrong: "
	<< cacheCon->ErrorMessage()
	<< std::endl;
  retval = false;
} else
  retval = true;

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return retval;
}

size_t sqlChannel::countNotes(dbHandle* cacheCon, unsigned short eventType)
{
/* Get a connection instance to our backend */
//dbHandle* cacheCon = myManager->getConnection();

/* Count the notes */
std::stringstream queryString;
queryString	<< "SELECT count(id) FROM notes WHERE channelID = "
		<< id
		;
if (eventType) {
queryString	<< " AND event = "
		<< eventType
		;
}

size_t num_notes = 0;

if (cacheCon->Exec(queryString.str(),true))
  num_notes = atoi(cacheCon->GetValue(0, 0));

/* Dispose of our connection instance */
//myManager->removeConnection(cacheCon);

return num_notes;
}

sqlChannel::~sqlChannel()
{
// No heap space allocated
}

} // namespace cf

} // namespace gnuworld
