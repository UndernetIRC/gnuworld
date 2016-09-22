/**
 * moduleLoader.h
 * Author: Daniel Karrels (dan@karrels.com)
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
 * $Id: moduleLoader.h,v 1.21 2004/05/18 16:50:57 dan_karrels Exp $
 */

#ifndef __MODULELOADER_H
#define __MODULELOADER_H "$Id: moduleLoader.h,v 1.21 2004/05/18 16:50:57 dan_karrels Exp $"

#include	<iostream>
#include	<string>

#include	<cstdlib> // exit()
#include	"ltdl.h"

#include	"ELog.h"

namespace gnuworld
{

/**
 * This is a templated module loader class.  The templated type is the
 * type of object to retrieve from the module.  This class is used with
 * libtool for maximum portability.
 */
template< typename modType, typename argType = std::string >
class moduleLoader
{ 

protected:

	/**
	 * This is the type of the bootstrap function.
	 */
	typedef modType (*GNUWModuleFunc)(argType arg) ;

	/**
  	 * Pointer to the loaded module. 
	 */
	lt_dlhandle		moduleHandle ;

	/**
	 * Pointer to the initialisation function.	
	 */
	GNUWModuleFunc		modFunc ;

	/**
	 * Pointer to the object being retrieved from the
	 * module.
	 */
	modType			modPtr ;

	/**
	 * The name of the module which this instance represents.
	 */
	std::string		moduleName ;

	/**
	 * This variable is true if there was an error in loading
	 * or resolving a symbol, false otherwise.
	 */
	bool			hasError ;

	/**
	 * Provide a protected mutator for this class to use to modify
	 * the error value.
	 */
	inline void		setError( bool newVal = true )
		{ hasError = newVal ; }

public:
	/**
	 * Constructor, takes a module filename as the only
	 * parameter.
	 */
	moduleLoader( const std::string& _moduleName )
	 : moduleName( _moduleName ),
	   hasError( false )
	{
	// Initialize the function pointer to 0
	modFunc = 0 ;

	// Must call lt_dlinit() to initialize libltdl.
	// This method may be called more than once
	if( lt_dlinit() != 0 )
		{
		elog	<< "moduleLoader> Failed to initialize "
			<< "module loading system: "
			<< lt_dlerror()
			<< std::endl ;
		setError() ;
		return ;
		}

	std::string fileName( moduleName ) ;
	if( fileName[ 0 ] != '/' )
		{
		// No absolute path specified
		// Attempt to find the module in the currect directory
		fileName = std::string( "./" ) + fileName ;
		}

	// Libtool libraries end with .la, and at this point, this class
	// only supports libtool libraries
	if( std::string::npos == fileName.find( ".la" ) )
		{
		// TODO: This should be more thorough
		fileName += ".la" ;
		}

//	elog	<< "moduleLoader> Attempting to load module "
//		<< fileName
//		<< std::endl;

	// lt_dlopenext() will do all that lt_dlopen() does,
	// but also check for .la, .so, .sl, etc extensions
	moduleHandle = lt_dlopen( fileName.c_str() ) ;

	if( 0 == moduleHandle )
		{
		elog	<< "moduleLoader> Error opening module ("
			<< moduleName
			<< "): "
			<< lt_dlerror()
			<< std::endl;
		setError() ;
		return ;
		}

//	elog	<< "moduleLoader> Module "
//		<< moduleName
//		<< " successfully loaded"
//		<< std::endl;
	}

	/**
	 * The destructor closes the module, but does not perform
	 * any other deallocation.
	 */
	virtual ~moduleLoader()
	{
	if( lt_dlclose( moduleHandle ) != 0 )
		{
		elog	<< "~moduleLoader> Error closing module: "
			<< lt_dlerror()
			<< std::endl ;
		}
	moduleHandle = 0 ;
	}

	/**
	 * Extracts an instance of modType
	 * derived object from this module, and returns it.
	 * This method will execute only once for each instance.
	 * Each additional call after the initial call to laodObject()
	 * will return the object previously created.
	 * The symbolSuffix is used to distinguish between similarly
	 * named symbols to be loaded from the same module.
	 */
	modType loadObject( argType arg,
		const std::string& symbolSuffix = std::string() )
	{
	if( getError() )
		{
		// Error already present
		return 0 ;
		}

	const std::string symbolName =
		std::string( "_gnuwinit" ) + symbolSuffix ;
	modFunc = (GNUWModuleFunc) lt_dlsym( moduleHandle,
			symbolName.c_str() ) ;
	if( 0 == modFunc )
		{
		elog	<< "moduleLoader::loadObject> Error: "
			<< lt_dlerror()
			<< std::endl ;
		setError() ;
		return 0 ;
		}

	modPtr = modFunc( arg );

	// Types usable by this class must support comparison against 0
	if( 0 == modPtr )
		{
		elog	<< "moduleLoader> Unable to instantiate modType."
			<< std::endl;
		setError() ;
		}

	return modPtr ;
	}

	/**
	 * Return a string containing the module's name, as passed
	 * to the constructor
	 */
	inline const std::string&	getModuleName() const
		{ return moduleName ; }

	/**
	 * Return the last error seen by the module system.
	 */
	inline const std::string&	getLastError() const
		{ return ::lt_dlerror() ; }

	/**
	 * Return by value the object being loaded from the module.
	 */
	inline modType		getObject() const
		{ return modPtr ; }

	/**
	 * Return true if an error occured, false otherwise.
	 */
	inline bool		getError() const
		{ return hasError ; }

} ;

} /* gnuworld */
 
#endif /* __MODULELOADER_H */
