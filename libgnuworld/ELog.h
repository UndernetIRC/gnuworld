/*
 * ELog.h
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
 * $Id: ELog.h,v 1.2 2003/06/03 01:01:52 dan_karrels Exp $
 */

#ifndef __ELOG_H
#define __ELOG_H "$Id: ELog.h,v 1.2 2003/06/03 01:01:52 dan_karrels Exp $"

#include	<sstream>
#include	<iostream>
#include	<fstream>
#include	<string>

#include	"config.h"

namespace gnuworld
{

using std::stringstream ;
using std::ofstream ;
using std::ostream ;
using std::string ;
using std::ios ;

/**
 * This class handles logging for GNUWorld.  It maintains a pointer
 * to a C++ output stream.  If this pointer is non-NULL, all messages
 * are logged to this stream.  All messages are also logged to the
 * file whose name is specified in the constructor.
 */
class ELog
{

protected:

	typedef ostream& (*__E_omanip)( ostream& ) ;
	typedef ostream& (*__E_manip)( ios& ) ;

	/**
	 * A pointer to a C++ output stream for logging.  If this
	 * stream is non-NULL, then all messages will be logged
	 * to this stream.
	 */
	ostream		*outStream ;

#ifdef EDEBUG
	/**
	 * The file output stream to which to log all messages.
	 */
	ofstream	outFile ;
#endif

public:

	/**
	 * Instantiate an instance of this class.  No output file
	 * is opened, and no output stream is specified for
	 * logging.
	 */
	ELog() ;

#ifdef EDEBUG
	/**
	 * Instantiate an instance of this class, specifying the
	 * name of the file to which to log messages.  This log file
	 * will be created if it does not already exist.  If it
	 * exists, it will be truncated.
	 */
	ELog( const string& ) ;
#endif

	/**
	 * Destroy an instance of this class.  This method will
	 * close the output file if it is open.
	 */
	virtual ~ELog() ;

#ifdef EDEBUG
	/**
	 * Open an output file for logging messages.  If an output
	 * file is currently open, it will be flushed and closed.
	 * This method will create the file if it does not already
	 * exist, otherwise the existing file will be truncated.
	 * This method returns true on success, false otherwise.
	 * Keep in mind that even if this method return false,
	 * any existing output file will be closed.
	 */
	bool openFile( const string& fileName ) ;

	/**
	 * Close the output log file, if it is open.
	 */
	void closeFile() ;

	/**
	 * Return true if an output log file is open, false otherwise.
	 * For some reason, std::fstream::is_open() is not const.
	 */
	inline bool isOpen()
		{ return outFile.is_open() ; }
#endif

	/**
	 * Use this method to specify which stream to which
	 * to log messages.  Specifying NULL will stop all
	 * output logging to any stream (except for the output file,
	 * if it exists).
	 */
	inline void setStream( ostream* newStream )
		{ outStream = newStream ; }

	ELog& operator<<( __E_omanip func ) ;
	ELog& operator<<( __E_manip func ) ;
	ELog& operator<<( const char& ) ;
	ELog& operator<<( const string& ) ;
	ELog& operator<<( const int& ) ;
	ELog& operator<<( const unsigned int& ) ;
	ELog& operator<<( const long int& ) ;
	ELog& operator<<( const unsigned long int& ) ;
	ELog& operator<<( const short int& ) ;
	ELog& operator<<( const unsigned short int& ) ;
	ELog& operator<<( stringstream& s ) ;
	ELog& operator<<( const double& ) ;

} ;

/// The global logging instance.
extern ELog elog ;

} // namespace gnuworld

#endif // __ELOG_H
