#include "Script.h"
#include "Debug.h"

namespace DE {

Script::Script() : Component()
{
};

Script::~Script() = default;

void Script::init(){

};

void Script::step(){
  ECHO("CUSTOM SCRIPT STEP")
};

void Script::update() {
}

void Script::terminate() {

}

} /* namespace DE */
