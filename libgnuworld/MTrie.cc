/**
 * MTrie.cc
 *
 * $Id: MTrie.cc,v 1.3 2003/07/21 16:05:21 dan_karrels Exp $
 */

#include	<map>
#include	<list>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"MTrie.h"
#include	"StringTokenizer.h"
#include	"match.h"

// TODO: Max number of levels? It could reduce memory by a large amount

using std::map ;
using std::clog ;
using std::list ;
using std::string ;
using gnuworld::StringTokenizer ;

template< typename _valueT >
MTrie< _valueT >::MTrie()
 : numElements( 0 )
{}

template< typename _valueT >
MTrie< _valueT >::~MTrie()
{
for( nodes_iterator itr = nodesMap.begin() ;
	itr != nodesMap.end() ; ++itr )
	{
	delete itr->second ;
	}
nodesMap.clear() ;
}

template< typename _valueT >
bool MTrie< _valueT >::insert( const string& key, const _valueT& value )
{
// Ensure there are no wildcard characters in the key.
if( key.find( '*' ) != string::npos )
	{
	clog	<< "MTrie::insert> Found \'*\' in key: "
		<< key
		<< endl ;
	return false ;
	}

if( key.find( '?' ) != string::npos )
	{
	clog	<< "MTrie::insert> Found \'?\' in key: "
		<< key
		<< endl ;
	return false ;
	}

// Tokenize the key
// The key is of the form "www.yahoo.com" or "ns1.switch.abc.net"
StringTokenizer st( key, '.' ) ;

MTrie< _valueT >* currentNode = this ;

// The purpose of this loop is to iterate to the level in which
// to place the new item.
// The proper level is found when the tokenItr is no longer valid,
// i.e., an empty token is found (works for top level also).
for( StringTokenizer::const_reverse_iterator tokenItr = st.rbegin() ;
	tokenItr != st.rend() ; ++tokenItr )
	{
	if( currentNode->nodesMap.find( *tokenItr ) ==
		currentNode->nodesMap.end() )
		{
		// This node does not exist
		MTrie< _valueT >* newNode =
			new (std::nothrow) MTrie< _valueT > ;
		assert( newNode != 0 ) ;

		currentNode->nodesMap[ *tokenItr ] = newNode ;

//		clog	<< "MTrie::insert> Added node for key: "
//			<< *tokenItr
//			<< endl ;
		}
	currentNode = currentNode->nodesMap[ *tokenItr ] ;
	}

// currentNode now points to the level at which to add this item
currentNode->valuesList.push_back( value ) ;

//clog	<< "MTrie::insert> Added key: "
//	<< key
//	<< endl ;

++numElements ;
return true ;
}

template< typename _valueT >
list< typename MTrie< _valueT >::value_type >
MTrie< _valueT >::find( const string& key ) const
{
clog	<< "MTrie::find> Searching for key: "
	<< key
	<< endl ;

// key may have wildcards, which is trickier than it may sound.
// A key may be "w*w.yahoo.com" which matches to both
// "www.yahoo.com" and "www.wwwww.yahoo.com"
if( (key.find( '*' ) != string::npos) || (key.find( '?' ) != string::npos) )
	{
	return wildCardFind( key ) ;
	}

// Normal key, no wildcards
list< value_type > returnMe ;

// Tokenize the key
// The key is of the form "www.yahoo.com" or "ns1.switch.abc.net"
StringTokenizer st( key, '.' ) ;

const MTrie< _valueT >* currentNode = this ;

for( StringTokenizer::const_reverse_iterator tokenItr = st.rbegin() ;
	tokenItr != st.rend() ; ++tokenItr )
	{
	const_nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist
		// Since we are not at the last token,
		// this key does not exist
		clog	<< "MTrie::find> Unable to find key: "
			<< key
			<< endl ;

		return returnMe ;
		}
	currentNode = nItr->second ;
	}

// currentNode now points to the node that corresponds to the last
// token in key.
// Iterate the node's value list for all matches
for( const_values_iterator vItr = currentNode->valuesList.begin() ;
	vItr != currentNode->valuesList.end() ; ++vItr )
	{
	if( 0 == match( key, *vItr ) )
		{
		clog	<< "MTrie::find> Found a match for key: "
			<< key
			<< ", match: "
			<< *vItr
			<< endl ;
		returnMe.push_back( value_type( key, *vItr ) ) ;
		}
	}
return returnMe ;
}

template< typename _valueT >
list< typename MTrie< _valueT >::value_type >
MTrie< _valueT >::wildCardFind( const string& key ) const
{
// precondition: key contains either a '?' and/or '*'
clog	<< "MTrie::wildCardFind> Searching for key: "
	<< key
	<< endl ;

// First, establish the base
list< string > base ;
list< string > remainingTokens ;
list< value_type > returnMe ;

const MTrie< data_type >* currentNode = this ;

StringTokenizer tokens( key, '.' ) ;
StringTokenizer::const_reverse_iterator tokenItr = tokens.rbegin() ;

for( ; tokenItr != tokens.rend() ; ++tokenItr )
	{
	if( (string::npos != (*tokenItr).find( '*' ))
		|| (string::npos != (*tokenItr).find( '?' )) )
		{
		// Wildcard
		break ;
		}

	// No wildcard, continue to next level
	const_nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist
		// Since we are not at the last token,
		// this key does not exist
		clog	<< "MTrie::wildCardFind> Unable to find key: "
			<< key
			<< endl ;

		return returnMe ;
		}
	currentNode = nItr->second ;

	// Add to base
	base.push_front( *tokenItr ) ;
	} // for( tokenItr )

clog	<< "MTrie::wildCardFind> *tokenItr: "
	<< *tokenItr
	<< endl ;

// Because of the precondition, the only way to exit the above 
// loop is by finding a wildcard, tokenItr must be valid 
// tokenItr now points to a token of the form "w*w" or "*w*w*" 
// We can prevent some unnecessary searching by creating a 
// search token with as many real characters as possible. 
// In the above cases, it would be "w" and "*", respectively. 
// That is, if we are searching for "w*w", there is no reason 
// to recursively check the currentNode's nodesMap element whose 
// key is "moo". 
// Construct the most known token.
string localKey( *tokenItr ) ;

// searchPrefix will be everything before (and including) the
// rightmost '*'
// searchPrefix is only used if a '*' is found, and empty otherwise
// In the case of "abc*fgh", localKey will be "*fgh" and
// searchPrefix will be "abc*"
string searchPrefix ;

// starPos is the index of the '*'
string::size_type starPos = localKey.rfind( '*' ) ;

// tokenItr needs to be moved to the next element in both cases below
++tokenItr ;

// If the '?' occured on the last token, then currentNode->values
// contains all possible matches
//bool wildOnLastToken = (tokenItr == tokens.rend()) ;

if( starPos != string::npos )
	{
	// '*' trumps '?'
	// Here we must:
	// setup localKey to be the star and everything to its right
	// setup the rest of the string to be everything left of
	//  and including the star
	clog	<< "MTrie::wildCardFind> Found *"
		<< endl ;

	// localKey starts out as "n*ws"
	// Setup localKey to "*ws"
	localKey.erase( 0, starPos ) ;

	// Setup searchPrefix to "n*"
	searchPrefix = (*tokenItr).substr( 0, starPos ) + "*" ;

	for( ; tokenItr != tokens.rend() ; ++tokenItr )
		{
		searchPrefix = *tokenItr + searchPrefix ;
		}
	}
else
	{
	// '?' found
	// we must:
	// - use tokenItr as localKey
	// - setup a list/vector to store the rest of tokens for
	//   the recursive find
	clog	<< "MTrie::wildCardFind> Found ?"
		<< endl ;

	for( ; tokenItr != tokens.rend() ; ++tokenItr )
		{
		remainingTokens.push_front( *tokenItr ) ;
		}

/*
	// With the '?', it is possible to have matches here
	if( wildOnLastToken )
		{
		for( const_values_iterator vItr = 
			currentNode->valuesList.begin() ;
			vItr != currentNode->valuesList.end() ;
			++vItr )
			{
			clog	<< "MTrie::wildCardFind> vItr: "
				<< *vItr
				<< endl ;
			value_type addMe( getBase( base ),
				*vItr ) ;
			returnMe.push_back( addMe ) ;
			}
		}
*/
	}

clog	<< "MTrie::wildCardFind> starPos: "
	<< starPos
	<< ", localKey: "
	<< localKey
	<< ", searchPrefix: "
	<< searchPrefix
	<< ", remainingTokens: " ;

bool doneALoop = false ;
for( list< string >::const_iterator rtItr = remainingTokens.begin() ;
	rtItr != remainingTokens.end() ; ++rtItr )
	{
	if( doneALoop )
		{
		clog	<< "." ;
		}
	clog	<< *rtItr ;
	doneALoop = true ;
	}

clog	<< ", base: " ;

doneALoop = false ;
for( list< string >::const_iterator rtItr = base.begin() ;
	rtItr != base.end() ; ++rtItr )
	{
	if( doneALoop )
		{
		clog	<< "." ;
		}
	clog	<< *rtItr ;
	doneALoop = true ;
	}
clog	<< endl ;

// Everything is set, begin recursion
// Match localKey against all nodes
for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	if( !match( localKey, nItr->first ) )
		{
/*		recursiveFind( nItr->second, // MTrie*
			base,
			returnMe,
			searchPrefix,
			remainingTokens ) ;
*/		}
	}

return returnMe ;
}

/*
template< typename _valueT >
void MTrie< _valueT >::recursiveFind( MTrie< _valueT >* currentNode,
	list< string >& base,
	list< value_type >& returnMe,
	string& searchPrefix,
	list< string >& remainingTokens ) const
{
if( searchPrefix.empty() )
	{
	// Looking for matches in remainingTokens
	


}
*/
template< typename _valueT >
string MTrie< _valueT >::getBase( const list< string >& base ) const
{
string retMe ;
bool doneALoop = false ;

for( list< string >::const_iterator rtItr = base.begin() ;
	rtItr != base.end() ; ++rtItr )
	{
	if( doneALoop )
		{
		retMe += "." ;
		}
	retMe += *rtItr ;
	doneALoop = true ;
	}
return retMe ;
}
