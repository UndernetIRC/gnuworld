/**
 * MTrie.cc
 *
 * $Id: MTrie.cc,v 1.6 2003/07/24 04:03:18 dan_karrels Exp $
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

using std::map ;
using std::clog ;
using std::list ;
using std::string ;
using gnuworld::StringTokenizer ;

template< typename _valueT >
MTrie< _valueT >::MTrie( const char delimiter )
 : numElements( 0 ),
   parent( 0 ),
   delimiter( delimiter )
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
StringTokenizer st( key, delimiter ) ;

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
		newNode->parent = currentNode ;

//		clog	<< "MTrie::insert> Added node for key: "
//			<< *tokenItr
//			<< endl ;

		currentNode = newNode ;
		}
	else
		{
		currentNode = currentNode->nodesMap[ *tokenItr ] ;
		}
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

list< string > base ;
list< value_type > returnMe ;

// key may have wildcards, which is trickier than it may sound.
// A key may be "w*w.yahoo.com" which matches to both
// "www.yahoo.com" and "www.wwwww.yahoo.com"
find( this, returnMe, base, key, key ) ;

return returnMe ;
}

template< typename _valueT >
void MTrie< _valueT >::find(
	const MTrie< _valueT >* currentNode,
	list< typename MTrie< _valueT >::value_type >& returnMe,
	list< string >& base,
	const string& origKey,
	const string& key ) const
{
if( key.empty() )
	{
	for( const_values_iterator vItr = currentNode->valuesList.begin() ; 
		vItr != currentNode->valuesList.end() ; ++vItr )
		{
		returnMe.push_back( value_type( getBase( base ), *vItr ) ) ;
		}
	return ;
	}

// precondition: key contains a '*'
clog	<< "MTrie::find> Searching for key: "
	<< key
	<< endl ;

StringTokenizer tokens( key, delimiter ) ;
StringTokenizer::const_reverse_iterator tokenItr = tokens.rbegin() ;

bool foundQuestionMark = false ;

// Iterate as far as possible before beginning wild card match()
// searches.
for( ; tokenItr != tokens.rend() ; ++tokenItr )
	{
	if( string::npos != (*tokenItr).find( '*' ) )
		{
		// Wildcard
		break ;
		}

	// Make sure this is after the search for '*'
	if( string::npos != (*tokenItr).find( '?') )
		{
		foundQuestionMark = true ;
		break ;
		}

	// No wildcard, continue to next level
	const_nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist
		// Since we are not at the last token,
		// this key does not exist
		clog	<< "MTrie::find> Unable to find key: "
			<< key
			<< endl ;

		return ;
		}
	currentNode = nItr->second ;

	// Add to base
	base.push_front( *tokenItr ) ;
	} // for( tokenItr )

if( tokenItr == tokens.rend() )
	{
	// We are at a matching node
	for( const_values_iterator vItr = currentNode->valuesList.begin() ; 
		vItr != currentNode->valuesList.end() ; ++vItr )
		{
		returnMe.push_back( value_type( getBase( base ), *vItr ) ) ;
		}
	return ;
	}

clog	<< "MTrie::find> *tokenItr: "
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

if( !foundQuestionMark )
	{
	// localKey starts out as "n*ws"
	// Setup localKey to "*ws"
	localKey.erase( 0, starPos ) ;
	}

clog	<< "MTrie::find> starPos: "
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
	clog	<< "MTrie::find> match( "
		<< localKey
		<< ", "
		<< nItr->first
		<< " ): " ;

	if( match( localKey, nItr->first ) )
		{
		clog	<< "false" << endl ;
		continue ;
		}

	clog	<< "true" << endl ;

	base.push_front( nItr->first ) ;
	if( foundQuestionMark )
		{
		// Question mark only, no '*' in the
		// token
		// Reassemble the key
		string newKey ;
		for( ++tokenItr; tokenItr != tokens.rend() ; 
			++tokenItr )
			{
			if( !newKey.empty() )
				{
				newKey.insert( newKey.begin(),
					delimiter ) ;
				}
			newKey.insert( 0, *tokenItr ) ;
			}

		clog	<< "MTrie::find> (?) newKey: "
			<< newKey
			<< endl ;

		find( nItr->second,
			returnMe,
			base,
			origKey,
			newKey ) ;
		}
	else
		{
		recursiveFind( nItr->second, // MTrie*
			base,
			returnMe,
			origKey ) ;
		} // if( questionMarkFound )
	base.pop_front() ;
	} // for( nItr )
}

template< typename _valueT >
void MTrie< _valueT >::recursiveFind(
	const MTrie< _valueT >* currentNode,
	list< string >& base,
	list< typename MTrie< _valueT >::value_type >& returnMe,
	const string& key ) const
{
clog	<< "MTrie::recursiveFind> key: "
	<< key
	<< ", base: "
	<< getBase( base )
	<< endl ;

// '*'
string stringBase = getBase( base ) ;
if( !match( key, stringBase ) )
	{
//	clog	<< "MTrie::recursiveFind> match"
//		<< endl ;

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
string MTrie< _valueT >::getBase( const list< string >& base ) const
{
string retMe ;
bool doneALoop = false ;

for( list< string >::const_iterator rtItr = base.begin() ;
	rtItr != base.end() ; ++rtItr )
	{
	if( doneALoop )
		{
		retMe += delimiter ;
		}
	retMe += *rtItr ;
	doneALoop = true ;
	}
return retMe ;
}

template< typename _valueT >
typename MTrie< _valueT >::size_type
MTrie< _valueT >::erase( const string& key )
{
clog	<< "MTrie::erase> Erasing key: "
	<< key
	<< endl ;

// key may have wildcards, which is trickier than it may sound.
// A key may be "w*w.yahoo.com" which matches to both
// "www.yahoo.com" and "www.wwwww.yahoo.com"

list< string > base ;
return erase( this, base, key, key ) ;
}

template< typename _valueT >
typename MTrie< _valueT >::size_type
MTrie< _valueT >::erase(
	MTrie< _valueT >* currentNode,
	list< string >& base,
	const string& origKey,
	const string& key )
{
size_type eraseCount = 0 ;
if( key.empty() )
	{
	// Everything on this level is a match
	eraseCount = currentNode->valuesList.size() ;
	currentNode->valuesList.clear() ;

	return eraseCount ;
	}

// precondition: key contains a '*'
clog	<< "MTrie::erase> Erasing key: "
	<< key
	<< endl ;

StringTokenizer tokens( key, delimiter ) ;
StringTokenizer::const_reverse_iterator tokenItr = tokens.rbegin() ;

bool foundQuestionMark = false ;

// Iterate as far as possible before beginning wild card match()
// searches.
for( ; tokenItr != tokens.rend() ; ++tokenItr )
	{
	if( string::npos != (*tokenItr).find( '*' ) )
		{
		// Wildcard
		break ;
		}

	// Make sure this is after the search for '*'
	if( string::npos != (*tokenItr).find( '?') )
		{
		foundQuestionMark = true ;
		break ;
		}

	// No wildcard, continue to next level
	nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist
		// Since we are not at the last token,
		// this key does not exist
		clog	<< "MTrie::erase> Unable to find key: "
			<< key
			<< endl ;

		return 0 ;
		}
	currentNode = nItr->second ;

	// Add to base
	base.push_front( *tokenItr ) ;
	} // for( tokenItr )

if( tokenItr == tokens.rend() )
	{
	// Everything on this level is a match
	eraseCount = currentNode->valuesList.size() ;
	currentNode->valuesList.clear() ;

	return eraseCount ;
	}

clog	<< "MTrie::erase> *tokenItr: "
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

if( !foundQuestionMark )
	{
	// localKey starts out as "n*ws"
	// Setup localKey to "*ws"
	localKey.erase( 0, starPos ) ;
	}

clog	<< "MTrie::erase> starPos: "
	<< starPos
	<< ", localKey: "
	<< localKey
	<< ", base: "
	<< getBase( base )
	<< endl ;

// Everything is set, begin recursion
// Match localKey against all nodes
for( nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	clog	<< "MTrie::erase> match( "
		<< localKey
		<< ", "
		<< nItr->first
		<< " ): " ;

	if( match( localKey, nItr->first ) )
		{
		clog	<< "false" << endl ;
		continue ;
		}

	clog	<< "true" << endl ;

	base.push_front( nItr->first ) ;
	if( foundQuestionMark )
		{
		// Question mark only, no '*' in the
		// token
		// Reassemble the key
		string newKey ;
		for( ++tokenItr; tokenItr != tokens.rend() ; 
			++tokenItr )
			{
			if( !newKey.empty() )
				{
				newKey.insert( newKey.begin(),
					delimiter ) ;
				}
			newKey.insert( 0, *tokenItr ) ;
			}

		clog	<< "MTrie::erase> (?) newKey: "
			<< newKey
			<< endl ;

		eraseCount += erase( nItr->second,
			base,
			origKey,
			newKey ) ;
		}
	else
		{
		eraseCount += recursiveErase( nItr->second, // MTrie*
			base,
			origKey ) ;
		} // if( questionMarkFound )
	base.pop_front() ;

	if( currentNode->valuesList.empty() &&
		currentNode->nodesMap.empty() )
		{
		clog	<< "MTrie::erase> Erasing empty node"
			<< endl ;

		// This does NOT invalidate the iterator
		delete nItr->second ;
		currentNode->nodesMap.erase( nItr ) ;
		}
	} // for( nItr )
return eraseCount ;
}

template< typename _valueT >
typename MTrie< _valueT >::size_type
MTrie< _valueT >::recursiveErase(
	MTrie< _valueT >* currentNode,
	list< string >& base,
	const string& key )
{
clog	<< "MTrie::recursiveErase(*)> base: "
	<< getBase( base )
	<< ", key: "
	<< key
	<< endl ;

size_type eraseCount = 0 ;

// '*'
string stringBase = getBase( base ) ;
if( !match( key, stringBase ) )
	{
	// This node matches
	eraseCount = currentNode->valuesList.size() ;
	currentNode->valuesList.clear() ;
	} // if( !match() )

for( nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	// match() is not important here, since we are doing
	// a blind '*' search, check every node from here down
	base.push_front( nItr->first ) ;
	eraseCount += recursiveErase( nItr->second, // MTrie*
		base,
		key ) ;
	base.pop_front() ;

	if( currentNode->valuesList.empty() &&
		currentNode->nodesMap.empty() )
		{
		clog	<< "MTrie::recursiveErase> Erasing empty node"
			<< endl ;

		// This does NOT invalidate the iterator
		delete nItr->second ;
		currentNode->nodesMap.erase( nItr ) ;
		}
	} // for( nItr )
return eraseCount ;
}

