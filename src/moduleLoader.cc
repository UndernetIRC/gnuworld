/*
 * moduleLoader.cc
 * 17/06/2000 - Gte
 */

#include    <dlfcn.h>
#include    "client.h"
#include    "ELog.h"
#include    "moduleLoader.h"

namespace gnuworld
{

template< class modType > 
moduleLoader::moduleLoader(const string& moduleName)
{ 
  elog << "[MOD]: Attempting to load module " << moduleName << endl;

  string fileName = "./" + moduleName;
  moduleHandle = dlopen(fileName.c_str(), RTLD_NOW | RTLD_GLOBAL );

  if(moduleHandle == NULL)
  {
    elog << "[MOD]: Error opening module: " << dlerror() << endl;
    exit(0);
  }
  elog << "[MOD]: Module " << moduleName << " successfully loaded." << endl;
}

template< class modType >
moduleLoader::~moduleLoader()
{
  dlclose(moduleHandle);
}

template< class modType >
modType moduleLoader::loadObject(const string& configFileName)
{ 
  modFunc = (GNUWModuleFunc) dlsym(moduleHandle, "_gnuwinit");

  const char* error = dlerror(); 
  if(error != NULL)
  {
    elog << "[MOD]: Error resolving _gnuwinit: " << error << endl;
    exit(0);
  }
  
  modType ptr = modFunc(configFileName);

  if (0 == ptr)
  {
    elog << "[MOD]: Unable to instantiate modType." << endl;
    exit(0);
  }

  return ptr;  
}

} /* gnuworld */
