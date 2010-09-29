/**
 * gThread.cc
 * Author: Daniel Karrels dan@karrels.com
 * Copyright (C) 2003 Daniel Karrels <dan@karrels.com>
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

#include	<unistd.h>

#include	<string>
#include	<iostream>

#include	"ELog.h"
#include	"gThread.h"

using namespace std ;
using namespace gnuworld ;

class subThread1 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread1" << endl ;
		sleep( 1 ) ;
		}
	}
} ;

class subThread2 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread2" << endl ;
		sleep( 2 ) ;
		}
	}
} ;

class subThread3 : public gThread
{
public:
	virtual void Exec()
	{ while( keepRunning )
		{
		cout	<< "subThread3" << endl ;
		sleep( 3 ) ;
		}
	}
} ;

int main()
{
subThread1 t1 ;
subThread2 t2 ;
subThread3 t3 ;

//cout	<< "Executing t1.Exec()" << endl ;
//t1.Exec() ;
//t1.Stop() ;

cout	<< "Starting threads..." << endl ;
t1.Start() ;
t2.Start() ;
t3.Start() ;

// Wait for user to hit CTRL-C
while( true )
	{
	sleep( 1 ) ;
	}

return 0 ;
}
