#include "Assert.h"
#include "Debug.h"
#include <string>       // std::string
#include <cstdlib>      // abort

namespace DE {

void assert(bool condition, std::string message){
  if(!condition){
    ERROR("ASSERT > "+  message);
    abort();
  }
}

}
