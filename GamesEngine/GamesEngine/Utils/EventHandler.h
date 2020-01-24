#pragma once
#include <functional>
#include "Event.h"

class EventHandler {
private:
	Event* m_handledEvent;

public:
	EventHandler(Event* handledEvent) : m_handledEvent(handledEvent) {}
	~EventHandler() {}

	/**
	 * Handle the function event that was passed to the handler
	 * bassed on the type of the handled event
	 * @param eventTypeHandled						The event type that has to be fullfilled
	 *												in order to be allowed to call the function
	 * @param funcHandler							Function that need to be fulfilled for the specific type
	 */
	void Handle( EventTypes eventTypeHandled, const std::function<void()>& funcHandler ) {
		if (this->m_handledEvent->GetEventType() == eventTypeHandled) {
			funcHandler();
			this->m_handledEvent->SetHandledTrue();
		}
	}
};