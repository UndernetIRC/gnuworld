/* moduleLoader.h */

#ifndef __MODULELOADER_H
#define __MODULELOADER_H "$Id: moduleLoader.h,v 1.9 2001/06/14 22:14:12 dan_karrels Exp $"

#include	<iostream>
#include	<string>

#include	<ltdl.h>
#include	<cstdlib> // exit()

#include	"ELog.h"

namespace gnuworld
{

using std::string ;
using std::endl ;

template< class modType >
class moduleLoader
{ 

protected:

	/**
	 * This is the type of the bootstrap function.
	 */
	typedef modType (*GNUWModuleFunc)(const string& args);

	/**
  	 * Pointer to the loaded module. 
	 */
	lt_dlhandle		moduleHandle;

	/**
	 * Pointer to the initialisation function.	
	 */
	GNUWModuleFunc		modFunc;

	/**
	 * Pointer to the object being retrieved from the
	 * module.
	 */
	modType			modPtr ;

	/**
	 * The name of the module which this instance represents.
	 */
	string			moduleName ;

public:
	/**
	 * Constructor, takes a module filename as the only
	 * parameter.
	 */
	moduleLoader( const string& _moduleName )
	 : moduleName( _moduleName )
	{
	// Must call lt_dlinit() to initialize libltdl.
	// This method may be called more than once
	lt_dlinit() ;

	string fileName( string( "./" ) + moduleName ) ;
	if( string::npos == fileName.find( ".la" ) )
		{
		// TODO: This should be more thorough
		fileName += ".la" ;
		}

	elog	<< "moduleLoader> Attempting to load module "
		<< fileName
		<< endl;

	// lt_dlopenext() will do all that lt_dlopen() does,
	// but also check for .la, .so, .sl, etc extensions
	moduleHandle = lt_dlopenext( fileName.c_str() ) ;

	if( 0 == moduleHandle )
		{
		elog	<< "moduleLoader> Error opening module ("
			<< moduleName
			<< "): "
			<< lt_dlerror()
			<< endl;
		::exit( 0 ) ;
		}

	elog	<< "moduleLoader> Module "
		<< moduleName
		<< " successfully loaded"
		<< endl;
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
			<< endl ;
		}
	moduleHandle = 0 ;
	}

	/* 
	 * Extracts an instance of modType
	 * erived object from this module, and returns it.
	 */
	modType loadObject( const string& configFileName )
	{
	lt_ptr_t symPtr = lt_dlsym( moduleHandle, "_gnuwinit" ) ;
	if( 0 == symPtr )
		{
		elog	<< "moduleLoader::loadObject> Error: "
			<< lt_dlerror()
			<< endl ;
		exit( 0 ) ;
		}

	modFunc = (GNUWModuleFunc) symPtr ;

	modPtr = modFunc(configFileName);

	// 0 is the global initializer
	if( 0 == modPtr )
		{
		elog	<< "moduleLoader> Unable to instantiate modType."
			<< endl;
		}

	return modPtr ;
	}

	/**
	 * Return a string containing the module's name, as passed
	 * to the constructor
	 */
	inline const string&	getModuleName() const
		{ return moduleName ; }

	/**
	 * Return the last error seen by the module system.
	 */
	inline const string&	getLastError() const
		{ return ld_dlerror() ; }

	/**
	 * Return by value the object being loaded from the module.
	 */
	inline modType		getObject() const
		{ return modPtr ; }

} ;

} /* gnuworld */
 
#endif /* __MODULELOADER_H */
