#ifndef __MODLOADER_H
#define __MODLOADER_H "$Id: moduleLoader.h,v 1.5 2000/12/19 21:32:35 dan_karrels Exp $"

#include	<string>

#include	<dlfcn.h>
#include	<cstdlib> // exit()

#include	"ELog.h"

using std::string ;

namespace gnuworld
{

template< class modType >
class moduleLoader
{ 
public:
	/**
	 *  Constructor, takes a module filename as the only
	 *  parameter.
	 */
	moduleLoader( const string& moduleName )
	{
	elog << "[MOD]: Attempting to load module " << moduleName << endl;

	string fileName = "./" + moduleName;
	#ifdef RTLD_NOW
	  moduleHandle = ::dlopen( fileName.c_str(), RTLD_NOW );
	#else
	  moduleHandle = ::dlopen( fileName.c_str(), DL_LAZY ) ;
	#endif

	if( 0 == moduleHandle )
		{
		elog	<< "[MOD]: Error opening module: "
			<< ::dlerror() << endl;
		::exit( 0 ) ;
		}
	elog	<< "[MOD]: Module " << moduleName << " successfully loaded."
		<< endl;
	}

	/**
	 * Destructor, calls dlclose for the module handle.
	 */
	virtual ~moduleLoader()
	{
	::dlclose( moduleHandle );
	}

	/* 
	 *  loadObject, extracts an instance of modType
	 *  derived object from this module, and returns it.
	 */
	modType loadObject( const string& configFileName )
	{
	modFunc = (GNUWModuleFunc) ::dlsym(moduleHandle, "_gnuwinit");

	const char* error = ::dlerror(); 
 	if( error != NULL )
		{
		elog	<< "[MOD]: Error resolving _gnuwinit: "
			<< error << endl;
		::exit( 0 ) ;
		}
  
	modType ptr = modFunc(configFileName);

	// 0 is the global initializer
	if (0 == ptr)
		{
		elog	<< "[MOD]: Unable to instantiate modType."
			<< endl;
		::exit( 0 ) ;
		}

	return ptr;  
	}

protected:

  /**
   * This is the type of the bootstrap function.
   */
  typedef modType (*GNUWModuleFunc)(const string& args);

  /**
   * Pointer to the loaded module. 
   */
  void* moduleHandle;

  /**
   * Pointer to the initialisation function.
   */
  GNUWModuleFunc modFunc;

} ;

} /* gnuworld */
 
#endif /* __MODLOADER_H */
