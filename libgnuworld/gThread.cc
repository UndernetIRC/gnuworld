/**
 * gThread.cc
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
 * $Id: gThread.cc,v 1.4 2003/06/17 15:13:53 dan_karrels Exp $
 */

#include	<pthread.h>

#include	<new>
#include	<string>
#include	<iostream>

#include	<cstring>
#include	<cerrno>
#include	<cassert>

#include	"gThread.h"
#include	"ELog.h"

const char rcsId[] = "$Id: gThread.cc,v 1.4 2003/06/17 15:13:53 dan_karrels Exp $" ;

namespace gnuworld
{

using std::string ;
using std::cout ;
using std::endl ;

/**
 * This function issues the call back to the gThread subclass, and
 * is executed in a separate thread of execution.
 */
void* stub( void* arg )
{
assert( arg != 0 ) ;

gThread* theThread = reinterpret_cast< gThread* >( arg ) ;
theThread->Exec() ;

return 0 ;
}

gThread::gThread()
{
keepRunning = true ;
isShutdownComplete = false ;
threadID = 0 ;

if( ::pthread_mutex_init( &waitMutex, 0 ) != 0 )
	{
	elog	<< "gThread> Unable to initialize waitMutex: "
		<< strerror( errno )
		<< endl ;
	::exit( -1 ) ;
	}
}

gThread::~gThread()
{
Join() ;
DestroyAllMutexes() ;
// destroy mutexes
if( !isShutdownComplete )
	{
	Join() ;
	}

::pthread_mutex_destroy( &waitMutex ) ;
}

bool gThread::CreateMutex( const string& mutexName )
{
if( mutexName.empty() )
	{
	// Invalid name
	return false ;
	}

if( mutexMap.find( mutexName ) != mutexMap.end() )
	{
	// Mutex already exists for that name
	return false ;
	}

pthread_mutex_t newMutex ;
::pthread_mutex_init( &newMutex, 0 ) ;

if( !mutexMap.insert(
	mutexMapType::value_type( mutexName, newMutex ) ).second )
	{
	// Insertion into map failed
	::pthread_mutex_destroy( &newMutex ) ;

	return false ;
	}

// All is well
return true ;
}

bool gThread::DestroyMutex( const string& mutexName )
{
mutexIterator mItr = mutexMap.find( mutexName ) ;
if( mItr == mutexMap.end() )
	{
	return false ;
	}

bool retMe = true ;
if( ::pthread_mutex_destroy( &(mItr->second) ) != 0 )
	{
	elog	<< "gThread::DestroyMutex> Unable to destroy mutex: "
		<< mutexName
		<< " because: "
		<< strerror( errno )
		<< endl ;
	retMe = false ;
	}

// Remove it from the map either way
mutexMap.erase( mItr ) ;

return retMe ;
}

void gThread::DestroyAllMutexes()
{
for( mutexIterator mItr = mutexMap.begin() ; mItr != mutexMap.end() ;
	++mItr )
	{
	if( ::pthread_mutex_destroy( &(mItr->second) ) != 0 )
		{
		elog	<< "gThread::DestroyAllMutexes> "
			<< "pthread_mutex_destroy() failed: "
			<< strerror( errno )
			<< endl ;
		}
	}
mutexMap.clear() ;
}

void gThread::LockMutex( const string& mutexName )
{
mutexIterator mItr = mutexMap.find( mutexName ) ;
if( mItr == mutexMap.end() )
	{
	elog	<< "gThread::LockMutex> Unable to locate mutex: "
		<< mutexName
		<< endl ;
	return ;
	}

::pthread_mutex_lock( &(mItr->second) ) ;
}

void gThread::UnLockMutex( const string& mutexName )
{
mutexIterator mItr = mutexMap.find( mutexName ) ;
if( mItr == mutexMap.end() )
	{
	elog	<< "gThread::UnLockMutex> Unable to locate mutex: "
		<< mutexName
		<< endl ;
	return ;
	}

::pthread_mutex_unlock( &(mItr->second) ) ;
}

bool gThread::Start()
{
if( ::pthread_create( &threadID, 0, stub,
	reinterpret_cast< void* >( this ) ) != 0 )
	{
	elog	<< "gThread::Start> pthread_create() failed: "
		<< strerror( errno )
		<< endl ;

	threadID = 0 ;
	return false ;
	}
return true ;
}

void gThread::Join()
{
if( isShutdownComplete || (0 == threadID) )
	{
	// Not running, or never started
	return ;
	}

if( ::pthread_join( threadID, 0 ) != 0 )
	{
	elog	<< "gThread::Join> join() failed: "
		<< strerror( errno )
		<< endl ;
	}
isShutdownComplete = true ;
}

} // namespace gnuworld
