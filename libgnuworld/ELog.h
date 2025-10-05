/**
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
 * $Id: ELog.h,v 1.8 2005/02/20 15:49:21 dan_karrels Exp $
 */

#ifndef __ELOG_H
#define __ELOG_H "$Id: ELog.h,v 1.8 2005/02/20 15:49:21 dan_karrels Exp $"

#include	<iostream>
#include	<fstream>
#include	<string>
#include	<time.h>

namespace gnuworld
{

/**
 * This class handles logging for GNUWorld.  It maintains a pointer
 * to a C++ output stream.  If this pointer is non-NULL, all messages
 * are logged to this stream.  All messages are also logged to the
 * file whose name is specified in the constructor.
 */
class ELog
{

protected:

	typedef std::ostream& (*__E_omanip)( std::ostream& ) ;
	typedef std::ostream& (*__E_manip)( std::ios& ) ;

	/**
	 * A pointer to a C++ output stream for logging.  If this
	 * stream is non-NULL, then all messages will be logged
	 * to this stream.
	 */
	std::ostream	*outStream ;

	/**
	 * The file output stream to which to log all messages.
	 */
	std::ofstream	outFile ;

	/**
	 * True if logging to an output file.
	 */
	bool		logFile ;

	bool		newline;

public:

	/**
	 * Instantiate an instance of this class.  No output file
	 * is opened, and no output stream is specified for
	 * logging.
	 */
	ELog() ;

	/**
	 * Instantiate an instance of this class, specifying the
	 * name of the file to which to log messages.  This log file
	 * will be created if it does not already exist.  If it
	 * exists, it will be truncated.
	 */
	ELog( const std::string& ) ;

	/**
	 * Destroy an instance of this class.  This method will
	 * close the output file if it is open.
	 */
	virtual ~ELog() ;

	/**
	 * Open an output file for logging messages.  If an output
	 * file is currently open, it will be flushed and closed.
	 * This method will create the file if it does not already
	 * exist, otherwise the existing file will be truncated.
	 * This method returns true on success, false otherwise.
	 * Keep in mind that even if this method return false,
	 * any existing output file will be closed.
	 */
	bool openFile( const std::string& fileName ) ;

	/**
	 * Close the output log file, if it is open.
	 */
	void closeFile() ;

	/**
	 * Return true if an output log file is open, false otherwise.
	 * For some reason, std::fstream::is_open() is not const.
	 */
	inline bool isOpen()
		{ return (logFile && outFile.is_open()) ; }

	/**
	 * Use this method to specify which stream to which
	 * to log messages.  Specifying NULL will stop all
	 * output logging to any stream (except for the output file,
	 * if it exists).
	 */
	inline void setStream( std::ostream* newStream )
		{ outStream = newStream ; }

	/**
	 * Get the stream to which to log messages.
	 */

	inline std::ostream* getStream()
		{ return outStream ; }

	/*
	 * Get local time in [hh:mm:ss] format
	 */
	std::string getLocalTime();

	/**
	 * Output the endl function.
	 */
	ELog& operator<<( __E_omanip func ) ;

	/**
	 * Output the endl function.
	 */
	ELog& operator<<( __E_manip func ) ;

	/**
	 * Output any other type supported by std::ostream.
	 */
	template< typename T >
	ELog& operator<<( const T& var )
		{
		if (logFile)
		{
			if (newline)
				outFile << getLocalTime() << var;
			else
				outFile << var;
		}
		if (outStream)
		{
			if (newline)
				*outStream << getLocalTime() << var;
			else
				*outStream << var;
		}
		newline = false;
		return *this ;
		}

} ;

/// The global logging instance.
extern ELog elog ;

} // namespace gnuworld

#endif // __ELOG_H
