/* moduleLoader.h */

#ifndef __MODULELOADER_H
#define __MODULELOADER_H "$Id: moduleLoader.h,v 1.8 2001/06/02 22:50:54 dan_karrels Exp $"

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
  GNUWModuleFunc	modFunc;

public:
	/**
	 *  Constructor, takes a module filename as the only
	 *  parameter.
	 */
	moduleLoader( const string& moduleName )
	{
	// Must call lt_dlinit() to initialize libltdl.
	// This method may be called more than once
	lt_dlinit() ;

	string fileName( string( "./" ) + moduleName ) ;

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
		<< " successfully loaded."
		<< endl;
	}

	/**
	 * Destructor, calls dlclose for the module handle.
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
	 *  loadObject, extracts an instance of modType
	 *  derived object from this module, and returns it.
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

	modType ptr = modFunc(configFileName);

	// 0 is the global initializer
	if (0 == ptr)
		{
		elog	<< "moduleLoader> Unable to instantiate modType."
			<< endl;
		::exit( 0 ) ;
		}

	return ptr;  
	}

} ;

} /* gnuworld */
 
#endif /* __MODULELOADER_H */
