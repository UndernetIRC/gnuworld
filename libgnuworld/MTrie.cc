/**
 * MTrie.cc
 *
 * $Id: MTrie.cc,v 1.4 2003/07/22 16:20:24 dan_karrels Exp $
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
// TODO: allow comparison functor (case insensitive)
// TODO: allow specifying delimiter ('.')

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

list< value_type > returnMe ;

// key may have wildcards, which is trickier than it may sound.
// A key may be "w*w.yahoo.com" which matches to both
// "www.yahoo.com" and "www.wwwww.yahoo.com"
if( key.find( '*' ) != string::npos )
	{
	wildCardFind( returnMe, key ) ;
	return returnMe ;
	}

// Tokenize the key
// The key is of the form "www.yahoo.com" or "ns1.switch.abc.net"
StringTokenizer st( key, '.' ) ;

if( key.find( '?' ) != string::npos )
	{
	list< string > base ;
	list< string > remainingTokens ;
	for( StringTokenizer::const_iterator sItr = st.begin() ;
		sItr != st.end() ; ++sItr )
		{
		remainingTokens.push_back( *sItr ) ;
		}
	recursiveFind( this,
		base,
		returnMe,
		remainingTokens ) ;
	return returnMe ;
	}

// Normal key, no wildcards
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
void MTrie< _valueT >::wildCardFind(
	list< typename MTrie< _valueT >::value_type >& returnMe,
	const string& key ) const
{
// precondition: key contains a '*'
clog	<< "MTrie::wildCardFind> Searching for key: "
	<< key
	<< endl ;

// First, establish the base
list< string > base ;

const MTrie< data_type >* currentNode = this ;

StringTokenizer tokens( key, '.' ) ;
StringTokenizer::const_reverse_iterator tokenItr = tokens.rbegin() ;

// Iterate as far as possible before beginning wild card match()
// searches.
for( ; tokenItr != tokens.rend() ; ++tokenItr )
	{
	if( string::npos != (*tokenItr).find( '*' ) )
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

		return ;
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

// starPos is the index of the '*'
string::size_type starPos = localKey.rfind( '*' ) ;

// Here we must:
// setup localKey to be the star and everything to its right
// setup the rest of the string to be everything left of
//  and including the star

// localKey starts out as "n*ws"
// Setup localKey to "*ws"
localKey.erase( 0, starPos ) ;

clog	<< "MTrie::wildCardFind> starPos: "
	<< starPos
	<< ", localKey: "
	<< localKey
	<< ", base: "
	<< getBase( base )
	<< endl ;

// Everything is set, begin recursion
// Match localKey against all nodes
for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	clog	<< "MTrie::wildCardFind> match( "
		<< localKey
		<< ", "
		<< nItr->first
		<< " ): " ;

	if( !match( localKey, nItr->first ) )
		{
		clog	<< "true" << endl ;
		base.push_front( nItr->first ) ;
		recursiveFind( nItr->second, // MTrie*
			base,
			returnMe,
			key ) ;
		base.pop_front() ;
		}
	else
		{
		clog	<< "false" << endl ;
		}
	}
}

template< typename _valueT >
void MTrie< _valueT >::recursiveFind(
	const MTrie< _valueT >* currentNode,
	list< string >& base,
	list< typename MTrie< _valueT >::value_type >& returnMe,
	const string& key ) const
{
clog	<< "MTrie::recursiveFind(*)> base: "
	<< getBase( base )
	<< ", key: "
	<< key
	<< endl ;

// '*'
string stringBase = getBase( base ) ;
if( !match( key, stringBase ) )
	{
	// This node matches
	for( const_values_iterator vItr = 
		currentNode->valuesList.begin() ;
		vItr != currentNode->valuesList.end() ;
		++vItr )
		{
		clog	<< "MTrie::recursiveFind> vItr: "
			<< *vItr
			<< endl ;
		value_type addMe( getBase( base ), *vItr ) ;
		returnMe.push_back( addMe ) ;
		} // for( vItr )
	} // if( !match() )

for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	// match() is not important here, since we are doing
	// a blind '*' search, check every node from here down
	base.push_front( nItr->first ) ;
	recursiveFind( nItr->second, // MTrie*
		base,
		returnMe,
		key ) ;
	base.pop_front() ;
	} // for( nItr )
}

template< typename _valueT >
void MTrie< _valueT >::recursiveFind( const MTrie< _valueT >* currentNode,
	list< string >& base,
	list< typename MTrie< _valueT >::value_type >& returnMe,
	list< string >& remainingTokens ) const
{
// '?'
if( remainingTokens.empty() )
	{
	// We have reached a final node, record all values
	for( const_values_iterator vItr = 
		currentNode->valuesList.begin() ;
		vItr != currentNode->valuesList.end() ;
		++vItr )
		{
		clog	<< "MTrie::recursiveFind(\?)> vItr: "
			<< *vItr
			<< endl ;
		value_type addMe( getBase( base ), *vItr ) ;
		returnMe.push_back( addMe ) ;
		} // for( vItr )

	// base case
	return ;
	} // if( base.empty() )

//list< string >::const_reverse_iterator tokenItr =
//	remainingTokens.rbegin() ;
list< string >::const_reverse_iterator tokenItr ;

// Iterate as far as possible before beginning question mark match()
// searches.
while( !remainingTokens.empty() )
	{
//for( ; tokenItr != remainingTokens.rend() ; ++tokenItr )
//	{
	tokenItr = remainingTokens.rbegin() ;
	if( string::npos != (*tokenItr).find( '?' ) )
		{
		// Question mark found
		break ;
		}

	// No wildcard, continue to next level
	const_nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist
		// Since we are not at the last token,
		// this key does not exist
		clog	<< "MTrie::recursiveFind(\?)> Unable to find key: "
			<< *tokenItr
			<< endl ;

		return ;
		}
	currentNode = nItr->second ;

	// Add to base
	base.push_front( *tokenItr ) ;

	remainingTokens.pop_back() ;
	} // for( tokenItr )

if( remainingTokens.empty() )
	{
	// If remainingTokens is empty it means that the final
	// token has been reached, and didn't have a '?'.
	// This is ok, it just means that we have reached the
	// end node.  Instead of building a for loop here, just
	// reinvoke this method with an empty remainingTokens
	// list, it will do the right thing (base case).
	recursiveFind( currentNode,
		base,
		returnMe,
		remainingTokens ) ;
	return ;
	}

// Found a '?'
string localKey( *tokenItr ) ;

// Everything is set, begin recursion
// Match localKey against all nodes
for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	clog	<< "MTrie::recursiveFind(\?)> match( "
		<< localKey
		<< ", "
		<< nItr->first
		<< " ): " ;

	if( !match( localKey, nItr->first ) )
		{
		clog	<< "true" << endl ;
		base.push_front( nItr->first ) ;
		recursiveFind( nItr->second, // MTrie*
			base,
			returnMe,
			remainingTokens ) ;
		base.pop_front() ;
		}
	else
		{
		clog	<< "false" << endl ;
		}
	}
}

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
