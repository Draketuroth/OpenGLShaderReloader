//============================================================================
// # ShaderReloader #
// Name: pubsub.h
// Author: Fredrik Linde
// Created on: May 19, 2019
// Description: C++ implementation of Publisher and Subscriber pattern
//============================================================================

#ifndef PUBSUB_H
#define PUBSUB_H

#include <list>

class Publisher;

class Subscriber 
{
public:
	virtual ~Subscriber() {}
	virtual void update(Publisher* pub, void* msg = 0) = 0;
};

class Publisher 
{
public:
	Publisher() : notifyEnabled(true) {}
	virtual ~Publisher() {}

	void subscribe(Subscriber* sub) { _subscribers.push_back(sub); }
	void unsubscribe(Subscriber* sub) { _subscribers.remove(sub); }

	void notify(void* msg = 0, Subscriber* sub = 0);
	void setNotifyEnabled(bool flag) { notifyEnabled = flag; }
	bool getNotifyEnabled() const { return notifyEnabled; }

private:
	std::list<Subscriber*> _subscribers;
	bool notifyEnabled;

};

#endif
