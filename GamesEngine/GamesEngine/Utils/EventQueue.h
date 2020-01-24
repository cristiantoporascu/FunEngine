#pragma once
#include "EventHandler.h"
#include <vector>

class EventQueue {
private:
	std::vector<Event*> m_events;

public:
	/**
	 * Singletone for the eventQueue to be accesable from everywhere
	 */
	static EventQueue* s_eventQueue;

	EventQueue() {}
	~EventQueue() {}

	/**
	 * Clear the handled events from the queue, by deleting them individualy
	 * from the vector before the start of the next frame
	 */
	void ClearHandledEvents() {
		for (unsigned int i = 0; i < this->m_events.size(); i++) {
			if (this->m_events[i]->IsHandled()) {
				delete this->m_events[i];
				this->RemoveEventById(i);
			}
		}
	}

	/*
	 * Getters and setters
	 */
public:
	inline Event* GetEventById(const int& id) const { return this->m_events[id]; }
	inline const int& GetEventQueueSize() const { return this->m_events.size(); }
	
	inline void AddEventToQueue(Event* e) { this->m_events.push_back(e); }
	inline void RemoveEventById(const int& id) { this->m_events.erase(this->m_events.begin() + id); }
};