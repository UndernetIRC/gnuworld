
#include <string>
#include "ccGate.h"

namespace gnuworld
{

namespace uworld
{

unsigned int ccGate::numAllocated;

ccGate::ccGate(string _Host, int _Port)
 :  Host(_Host),
    Port(_Port),
    Status(statReady),
    Found(false),
    threadId(0)
{
++numAllocated;
};

ccGate::~ccGate()
{
--numAllocated;
};

}
}
    
