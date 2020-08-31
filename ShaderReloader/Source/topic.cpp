
#include "topic.h"

void Publisher::notify(void* msg, Subscriber* sub)
{
	if (notifyEnabled) 
	{
		std::list<Subscriber*>::iterator it;
		for (it = _subscribers.begin(); it != _subscribers.end(); it++) 
		{
			if (*it != sub) 
			{
				(*it)->update(this, msg);
			}
		}
	}
	notifyEnabled = true;
}
