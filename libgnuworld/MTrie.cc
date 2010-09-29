/**
 * MTrie.cc
 * Copyright (C) 2002 Daniel Karrels <dan@karrels.com>
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
 * $Id: MTrie.cc,v 1.14 2004/05/18 16:50:57 dan_karrels Exp $
 */

#include	<map>
#include	<list>
#include	<string>
#include	<iostream>

#include	<cassert>

#include	"MTrie.h"
#include	"StringTokenizer.h"
#include	"match.h"

// TODO: allow comparison functor (case insensitive)

using std::map ;
using std::clog ;
using std::endl ;
using std::list ;
using std::string ;
using std::ostream ;
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
//	clog	<< "MTrie::insert> Found \'*\' in key: "
//		<< key
//		<< endl ;
	return false ;
	}

if( key.find( '?' ) != string::npos )
	{
//	clog	<< "MTrie::insert> Found \'?\' in key: "
//		<< key
//		<< endl ;
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
//clog	<< "MTrie::find> Searching for key: "
//	<< key
//	<< endl ;

// base tracks the keys used to iterate to the different levels.
// It is used when matching for '*', since '*' may cross
// levels.
base.clear() ;
returnMe.clear() ;
origKey = key ;

StringTokenizer tokens( key, delimiter ) ;
StringTokenizer::const_reverse_iterator tokenItr = tokens.rbegin() ;

// key may have wildcards, which is trickier than it may sound.
// A key may be "w*w.yahoo.com" which matches to both
// "www.yahoo.com" and "www.wwwww.yahoo.com"
// The recursive find will handle all cases of '?', '*', and
// normal non-wildcard keys.
find( this, tokens, tokenItr ) ;

return returnMe ;
}

template< typename _valueT >
void MTrie< _valueT >::find(
	const MTrie< _valueT >* currentNode,
	const StringTokenizer& tokens,
	StringTokenizer::const_reverse_iterator tokenItr ) const
{
// If the key is empty, that means that we have reached the
// final node.  All values at this node are matches.
// This happens when the left most token has a '?' in it:
// "n?ws.abs.net"
if( tokenItr == tokens.rend() )
	{
//	clog	<< "MTrie::find> Found end of tokens, numValues: "
//		<< currentNode->valuesList.size()
//		<< endl ;

	for( const_values_iterator vItr = currentNode->valuesList.begin(),
		vEndItr = currentNode->valuesList.end() ;
		vItr != vEndItr ; ++vItr )
		{
		returnMe.push_back( value_type( getBase(), *vItr ) ) ;
		}
	return ;
	}

//clog	<< "MTrie::find> tokens: "
//	<< tokens
//	<< ", tokenItr: "
//	<< *tokenItr
//	<< endl ;

bool foundQuestionMark = false ;

// Iterate as far as possible
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
//		clog	<< "MTrie::find> Found question mark in token: "
//			<< *tokenItr
//			<< endl ;
		foundQuestionMark = true ;
		break ;
		}

	// No wildcard, continue to next level
	const_nodes_iterator nItr = currentNode->nodesMap.find( *tokenItr ) ;
	if( currentNode->nodesMap.end() == nItr )
		{
		// This node does not exist

//		clog	<< "MTrie::find> Unable to find key: "
//			<< key
//			<< endl ;

		return ;
		}
	currentNode = nItr->second ;

	// Add to base
	base.push_front( *tokenItr ) ;
	} // for( tokenItr )

if( tokenItr == tokens.rend() )
	{
	// We are at a matching node
	for( const_values_iterator vItr = currentNode->valuesList.begin(),
		vEndItr = currentNode->valuesList.end() ;
		vItr != vEndItr ; ++vItr )
		{
		returnMe.push_back( value_type( getBase(), *vItr ) ) ;
		}
	return ;
	}

//clog	<< "MTrie::find> *tokenItr: "
//	<< *tokenItr
//	<< endl ;

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

//clog	<< "MTrie::find> localKey: "
//	<< localKey
//	<< endl ;

// Move to the next token for calls to find()
++tokenItr ;

// This variable is true if the localKey determined below
// is '*' alone.
bool loneStar = false ;

// Prepare localKey if a '*' was found
if( !foundQuestionMark )
	{
	// '*' was found in localKey

	// localKey starts out as "n*ws"
	// Setup localKey to "*ws"
	// In the case of a key like "*adsl*", there is no
	// sense in trimming down localKey just to perform
	// a match(), since all nodes match.

	// starPos is the index of the right-most '*'
	const string::size_type starPos = localKey.rfind( '*' ) ;

	if( starPos == (localKey.size() - 1) )
		{
		// The '*' is the right-most character, "*adsl*"
//		cout	<< "find> Found lonestar"
//			<< endl ;
		loneStar = true ;
		}
	else
		{
		// Not a generic '*' search, it's the case of "n*ws"
		// Trim localKey to look like "*ws"
		localKey.erase( 0, starPos ) ;
		}
	} // if( !foundQuestionMark )

// Everything is set, begin recursion
// Match localKey against all nodes
for( const_nodes_iterator nItr = currentNode->nodesMap.begin(),
	nEndItr = currentNode->nodesMap.end() ;
	nItr != nEndItr ; ++nItr )
	{
//	clog	<< "MTrie::find> match( "
//		<< localKey
//		<< ", "
//		<< nItr->first
//		<< " ): " ;

	if( foundQuestionMark )
		{
		if( !gnuworld::match( localKey, nItr->first ) )
			{
//			clog	<< "true" << endl ;
			base.push_front( nItr->first ) ;

			// Question mark only, no '*' in the token.
			find( nItr->second,
				tokens,
				tokenItr ) ;

			base.pop_front() ;
			}
//		else
//			{
//			clog	<< "false" << endl ;
//			}
		continue ;
		}

	// '*'
	if( loneStar )
		{
		/* NO-OP, fall through, all nodes match */
		}
	else if( gnuworld::match( localKey, nItr->first ) )
		{
		// Not a match, do not recursiveFind() on this node
		continue ;
		}

//	clog	<< "true" << endl ;

	base.push_front( nItr->first ) ;
	recursiveFind( nItr->second ) ;
	base.pop_front() ;
	} // for( nItr )
}

template< typename _valueT >
void MTrie< _valueT >::recursiveFind(
	const MTrie< _valueT >* currentNode,
	bool blindRecursion ) const
{
// blindRecursion:
// If we encounter a '*' as the first character of origKey (the original
// search string), and the current node matches that key, then by
// definition all nodes under the current node must also match.
// In this case, use a "blind recursion," simply adding all values
// under this node without performing a match() on any of them.

//clog	<< "MTrie::recursiveFind> key: "
//	<< origKey
//	<< ", base: "
//	<< getBase()
//	<< endl ;

// '*'
// No need to create a new string and perform a match
// if this node has no values
if( !currentNode->valuesList.empty() )
	{
	// We need the stringBase either way
	string stringBase = getBase() ;

	if( blindRecursion || !gnuworld::match( origKey, stringBase ) )
		{
		//	clog	<< "MTrie::recursiveFind> match"
		//		<< endl ;

		if( !blindRecursion && '*' == origKey[ 0 ] )
			{
			// Found a match starting at this node
			blindRecursion = true ;
			}

		// This node matches
		for( const_values_iterator vItr = 
			currentNode->valuesList.begin(),
			vEndItr = currentNode->valuesList.end() ;
			vItr != vEndItr ; ++vItr )
			{
	//		clog	<< "MTrie::recursiveFind> vItr: "
	//			<< *vItr
	//			<< endl ;
			value_type addMe( stringBase, *vItr ) ;
			returnMe.push_back( addMe ) ;
			} // for( vItr )
		} // if( !match() )
	} // if( !empty() )

for( const_nodes_iterator nItr = currentNode->nodesMap.begin(),
	nEndItr = currentNode->nodesMap.end() ;
	nItr != nEndItr ; ++nItr )
	{
	// match() is not important here, since we are doing
	// a blind '*' search, check every node from here down
	base.push_front( nItr->first ) ;
	recursiveFind( nItr->second, blindRecursion ) ;
	base.pop_front() ;
	} // for( nItr )
}

template< typename _valueT >
string MTrie< _valueT >::getBase() const
{
string retMe ;
retMe.reserve( 128 ) ;
bool doneALoop = false ;

for( list< string >::const_iterator rtItr = base.begin(),
	rtEndItr = base.end() ; rtItr != rtEndItr ; ++rtItr )
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
//clog	<< "MTrie::erase> Erasing key: "
//	<< key
//	<< endl ;

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

//clog	<< "MTrie::erase> Erasing key: "
//	<< key
//	<< endl ;

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
//		clog	<< "MTrie::erase> Unable to find key: "
//			<< key
//			<< endl ;

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

//clog	<< "MTrie::erase> *tokenItr: "
//	<< *tokenItr
//	<< endl ;

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

//clog	<< "MTrie::erase> starPos: "
//	<< starPos
//	<< ", localKey: "
//	<< localKey
//	<< ", base: "
//	<< getBase()
//	<< endl ;

// Everything is set, begin recursion
// Match localKey against all nodes
for( nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
//	clog	<< "MTrie::erase> match( "
//		<< localKey
//		<< ", "
//		<< nItr->first
//		<< " ): " ;

	if( gnuworld::match( localKey, nItr->first ) )
		{
//		clog	<< "false" << endl ;
		continue ;
		}

//	clog	<< "true" << endl ;

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

//		clog	<< "MTrie::erase> (?) newKey: "
//			<< newKey
//			<< endl ;

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
//		clog	<< "MTrie::erase> Erasing empty node"
//			<< endl ;

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
//clog	<< "MTrie::recursiveErase(*)> base: "
//	<< getBase()
//	<< ", key: "
//	<< key
//	<< endl ;

size_type eraseCount = 0 ;

// '*'
string stringBase = getBase() ;
if( !gnuworld::match( key, stringBase ) )
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
//		clog	<< "MTrie::recursiveErase> Erasing empty node"
//			<< endl ;

		// This does NOT invalidate the iterator
		delete nItr->second ;
		currentNode->nodesMap.erase( nItr ) ;
		}
	} // for( nItr )
return eraseCount ;
}

template< typename _valueT >
void MTrie< _valueT >::dumpDebug( ostream& out ) const
{
recursiveDebug( out, 1, this ) ;
}

template< typename _valueT >
void MTrie< _valueT >::recursiveDebug( ostream& out,
	size_t levelNum,
	const MTrie< _valueT >* currentNode ) const
{
for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	recursiveDebug( out, levelNum + 1, nItr->second ) ;
	}
out	<< levelNum << ' '
	<< currentNode->nodesMap.size() << ' '
	<< currentNode->valuesList.size() << ' '
	<< endl ;
}

template< typename _valueT >
void MTrie< _valueT >::levelDebug( ostream& out, size_t searchLevel )
	const
{
out	<< "Searching for level "
	<< searchLevel
	<< endl ;
levelDebug( out, 0, searchLevel, this ) ;
}

template< typename _valueT >
void MTrie< _valueT >::levelDebug( ostream& out, size_t currentLevel,
	size_t searchLevel, const MTrie< _valueT >* currentNode ) const
{
if( (currentLevel + 1) == searchLevel )
	{
	// Next level is the winner
	for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
		nItr != currentNode->nodesMap.end() ; ++nItr )
		{
		out	<< "Key: "
			<< nItr->first
			<< ", number of values: "
			<< value_size( nItr->second )
			<< endl ;
		}
	}
else
	{
	for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
		nItr != currentNode->nodesMap.end() ; ++nItr )
		{
		levelDebug( out, currentLevel + 1, searchLevel,
			currentNode ) ;
		}
	}
}

template< typename _valueT >
size_t MTrie< _valueT >::value_size( const MTrie< _valueT >* currentNode )
	const
{
size_t returnMe = currentNode->valuesList.size() ;
for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
	nItr != currentNode->nodesMap.end() ; ++nItr )
	{
	returnMe += value_size( nItr->second ) ;
	}
return returnMe ;
}

template< typename _valueT >
list< string >
MTrie< _valueT >::findMinLength( size_t minLength ) const
{
base.clear() ;
list< string > retMe ;

findMinLength( minLength, retMe, this ) ;

return retMe ;
}

template< typename _valueT >
void
MTrie< _valueT >::findMinLength( size_t minLength,
	list< string >& retMe,
	const MTrie< _valueT >* currentNode ) const
{
if( (base.size() >= minLength) && !currentNode->valuesList.empty() )
	{
	// We are at or past the min length, and this node
	// has at least one value to add
	// Only one entry into returnMe is necessary
	retMe.push_back( getBase() ) ;
	}
else
	{
	// Continue to all subnodes
	for( const_nodes_iterator nItr = currentNode->nodesMap.begin() ;
		nItr != currentNode->nodesMap.end() ; ++nItr )
		{
		base.push_front( nItr->first ) ;
		findMinLength( minLength,
			retMe,
			nItr->second ) ;
		base.pop_front() ;
		}
	}
}
