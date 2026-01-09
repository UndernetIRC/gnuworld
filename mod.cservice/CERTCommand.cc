/**
 * CERTCommand.cc
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
 */

#include	<string>
#include	"cservice.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"responses.h"

namespace gnuworld
{

bool CERTCommand::Exec( iClient* [[maybe_unused]] theClient, const string& [[maybe_unused]] Message )
{
#ifdef NEW_IRCU_FEATURES

bot->incStat("COMMANDS.CERT");

sqlUser* theUser = bot->isAuthed( theClient, true ) ;
if( !theUser )
	{
	return false ;
 	}

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage( theClient ) ;
	return true ;
	}

string Command = string_upper( st[ 1 ] ) ;
if( ( Command != "ADD" ) && ( Command != "REM" ) && ( Command != "LIST" ) )
	{
	Usage( theClient ) ;
	return true ;
	}

if( Command == "LIST" )
	{
    bot->Notice( theClient, "+-------------------------------------------------------------------------------------------------+------------------------------------------+--------------+--------------------+" ) ;
    bot->Notice( theClient, "| %-95s | %-40s | %-12s | %-18s |", "TLS Fingerprint", "Added By", "Last Updated", "Note" ) ;
    bot->Notice( theClient, "+-------------------------------------------------------------------------------------------------+------------------------------------------+--------------+--------------------+" ) ;

	std::stringstream theQuery ;
	theQuery	<< "SELECT fingerprint, added_by, added_ts, note FROM users_fingerprints WHERE user_id = "
				<< theClient->getAccountID()
				<< " ORDER BY added_ts" ;

	if( !bot->SQLDb->Exec( theQuery, true ) )
		{
		LOGSQL_ERROR( bot->SQLDb ) ;
		return false ;
		}

	if( bot->SQLDb->Tuples() > 0 )
		{
		for( unsigned int i = 0 ; i < bot->SQLDb->Tuples() ; i++ )
			bot->Notice( theClient, "| %-95s | %-40s | %-12s | %-18s |",
				compactToCanonical( bot->SQLDb->GetValue( i, 0 ) ).c_str(),
				bot->SQLDb->GetValue( i, 1 ).c_str(),
				prettyTime( std::stoul( bot->SQLDb->GetValue( i, 2 ) ), false ).c_str(),
				bot->SQLDb->GetValue( i, 3 ).c_str() ) ;
    	bot->Notice( theClient, "+-------------------------------------------------------------------------------------------------+------------------------------------------+--------------+--------------------+" ) ;
		bot->Notice( theClient, "Done listing %d fingerprint%s.",
			bot->SQLDb->Tuples(), bot->SQLDb->Tuples() > 1 ? "s" : "" ) ;
		}
	else
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::no_fingerprints_found ) ) ;
		}

	if( theClient->hasTlsFingerprint() )
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::your_fingerprint_is ).c_str(),
			compactToCanonical( theClient->getTlsFingerprint() ).c_str() ) ;
		}

	return true ;
	}

if( Command == "ADD" )
	{
	/* Check whether this user already has reached the limit of fingerprints. */
	if( bot->hasFP( theUser) > bot->getConfmaxFingerprints() )
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::max_fingerprints ).c_str(), bot->getConfmaxFingerprints() ) ;
		return true ;
		}

	string fingerPrint, note ;

	/* No param provided. Use current fingerprint, if any. */
	if( st.size() < 3 )
		{
		if( theClient->hasTlsFingerprint() )
			fingerPrint = theClient->getTlsFingerprint() ;
		else
			{
			Usage( theClient ) ;
			return true ;
			}
		}
	/* We have one param. */
	else if( st.size() == 3 )
		{
		/* Is the param a fingerprint? */
		if( isValidSHA256Fingerprint( st[ 2 ] ) )
			fingerPrint = canonicalToCompact( st[ 2 ] ) ;
		/* If it is not a fingerprint, we treat it as a note if (and only if) the user has a fingerprint. */
		else if( theClient->hasTlsFingerprint() )
			note = st[ 2 ] ;
		/* If the param is not a fingerprint and the user does not have a fingerprint, fail. */
		else
			{
			bot->Notice( theClient, bot->getResponse( theUser, language::invalid_fingerprint ).c_str() ) ;
			return true ;
			}
		}
	/* We have either two params, or a note consisting  of several words. */
	else if( st.size() > 3 )
		{
		/* Is the first word a fingerprint? */
		if( isValidSHA256Fingerprint( st[ 2 ] ) )
			{
			fingerPrint = canonicalToCompact( st[ 2 ] ) ;
			note = st.assemble( 3 ) ;
			}
		/* If it is not a fingerprint, we treat it as a note if (and only if) the user has a fingerprint. */
		else if( theClient->hasTlsFingerprint() )
			note = st.assemble( 2 ) ;
		/* If the first word is not a fingerprint and the user does not have a fingerprint, fail. */
		else
			{
			bot->Notice( theClient, bot->getResponse( theUser, language::invalid_fingerprint ).c_str() ) ;
			return true ;
			}
		}

	/* Add to cache. This will return false if the fingerprint already exists. */
	auto result = bot->addFP( canonicalToCompact( fingerPrint ), theClient->getAccountID() ) ;
	if( !result.second )
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::fingerprint_already_exists ).c_str() ) ;
		return true ;
		}

	/* Add to SQL. */
	std::stringstream theQuery ;
	theQuery	<< "INSERT INTO users_fingerprints (user_id, fingerprint, added_ts, added_by, note) VALUES ("
				<< theClient->getAccountID() << ", '"
				<< fingerPrint
				<< "', date_part('epoch', CURRENT_TIMESTAMP)::int, '"
				<< theClient->getRealNickUserHost() << "', '"
				<< note << "')"
				<< std::endl ;

	if( !bot->SQLDb->Exec( theQuery, true ) )
		{
		LOGSQL_ERROR( bot->SQLDb ) ;
		return false ;
		}

	bot->Notice( theClient, bot->getResponse( theUser, language::fingerprint_added ).c_str(),
		compactToCanonical( fingerPrint ).c_str() ) ;

	return true ;
	}

if( Command == "REM" )
	{
	string fingerPrint ;

	/* No fingerprint been provided. Use current, if any. */
	if( st.size() < 3 )
		{
		if( theClient->hasTlsFingerprint() )
			fingerPrint = theClient->getTlsFingerprint() ;
		else
			{
			Usage( theClient ) ;
			return true ;
			}
		}
	else
		{
		if( !isValidSHA256Fingerprint( st[ 2 ] ) )
			{
			bot->Notice( theClient, bot->getResponse( theUser, language::invalid_fingerprint ).c_str() ) ;
			return true ;
			}

		fingerPrint = canonicalToCompact( st[ 2 ] ) ;
		}

	/* Don't remove last fingerprint if CERTONLY is ON. */
	if( bot->hasFP( theUser) == 1 && theUser->getFlag( sqlUser::F_CERTONLY ) )
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::fingerprint_norem_certonly ).c_str() ) ;
		return true ;
		}

	if( !bot->checkFP( fingerPrint, theClient->getAccountID() ) )
		{
		bot->Notice( theClient, bot->getResponse( theUser, language::fingerprint_not_found ).c_str() ) ;
		return true ;
		}

	/* Remove from SQL. */
	std::stringstream theQuery ;
	theQuery	<< "DELETE FROM users_fingerprints WHERE fingerprint ='"
				<< fingerPrint << "'"
				<< std::endl ;

	if( !bot->SQLDb->Exec( theQuery, true ) )
		{
		LOGSQL_ERROR( bot->SQLDb ) ;
		return false ;
		}
	/* Remove from cache. */
	bot->removeFP( fingerPrint ) ;

	bot->Notice( theClient, bot->getResponse( theUser, language::fingerprint_removed ).c_str(),
		compactToCanonical( fingerPrint ).c_str() ) ;

	return true ;
	}

#endif // NEW_IRCU_FEATURES
return true ;
}

} // namespace gnuworld.
