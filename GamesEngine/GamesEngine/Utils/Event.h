#pragma once

enum EventTypes {
	MOVE_FORWARD, MOVE_BACKWARD, MOVE_LEFT, MOVE_RIGHT, JUMP, PLAYER_FINISH,
	C_MOVE_FORWARD, C_MOVE_BACKWARD, C_ROTATE_RIGHT, C_ROTATE_LEFT, C_ROTATE_UP, C_ROTATE_DOWN,
	SLOWDOWN, SPEEDUP,
	TOGGLE_WINDOW_MANAGER,
	WINDOW_CLOSE,
	NETWORK_CLIENT_RECEIVE,
	NETWORK_SERVER_CONNECT, NETWORK_SERVER_DISCONNECT, NETWORK_SERVER_RECEIVE
};

class Event {
private:
	bool m_handled = false;

	EventTypes m_type;
	float m_eventTimeStamp;

public:
	Event(EventTypes eType, float eTimeStamp) : m_type(eType), m_eventTimeStamp(eTimeStamp) {}
	~Event() {}

	/**
	 * Getters and setters
	 */
public:
	inline const bool& IsHandled() const { return this->m_handled;  }
	inline const float& GetETimeStamp() const { return this->m_eventTimeStamp; }
	inline EventTypes GetEventType() const { return this->m_type; }

	inline void SetHandledTrue() { this->m_handled = true; }
};
