#include "UI/UIStyle.hpp"


UIStyleManager::~UIStyleManager() 
{
	MAP_DELETE_CONTENT(mStyles)
}

void UIStyleManager::init()
{
	
}
