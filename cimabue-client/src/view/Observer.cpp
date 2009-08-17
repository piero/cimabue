/*
* Observer.cpp
*
*  Created on: Aug 16, 2009
*      Author: piero
*/

#include "Observer.h"

Observer::Observer(StateManager *caller)
: model(caller)
{
	if (model != NULL)
		model->addView(this);
}

Observer::~Observer()
{
	if (model != NULL)
		model->remView(this);
}
