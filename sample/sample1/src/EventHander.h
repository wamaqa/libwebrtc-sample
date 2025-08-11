#pragma once
namespace libWebrtc {
	enum Event_Name
	{
		SetRemoteDescriptionObserverInterface_OnSetRemoteDescriptionComplete,
	};

	struct EventArgs
	{

	};

	class AbstractEventListener
	{

	};

	class EventListener
	{
	public:

		EventListener(Event_Name name, std::function<void(EventArgs)> listener)
			:listener_(std::move(listener)), m_eventName(name) {
		}

		//EventListener(Event_Name name, std::function<void(EventArgs)> listener, uint32_t id)
		//	:listener_(std::move(listener)), m_id(id), m_eventName(name) {
		//}

		const uint32_t getId() { return m_id; };
		Event_Name getEventName() { return m_eventName; };
	private:
		uint32_t m_id = -1;
		Event_Name m_eventName;
		std::function<void(EventArgs)> listener_;
		friend class EventHander;
	};


	class EventHander
	{
	public:
		EventHander() {};
		~EventHander() {};

		uint32_t AddEventListener(EventListener& eventListener) {
			if (!m_listeners.contains(eventListener.getEventName()))
				m_listeners[eventListener.getEventName()] = std::vector<EventListener>();
			eventListener.m_id = m_idIndex++;
			m_listeners[eventListener.getEventName()].push_back(std::move(eventListener));
			return m_idIndex;
		}


		//uint32_t AddEventListener(const Event_Name name, std::function<void(EventArgs)> listener) {
		//	if (!m_listeners.contains(name))
		//		m_listeners[name] = std::vector<EventListener>();
		//	auto eventListener = EventListener(name, std::move(listener), m_idIndex++);
		//	m_listeners[name].push_back(std::move(eventListener));
		//	return m_idIndex;
		//}

		void DispatchEvent(const Event_Name name, EventArgs argv) {
			auto it = m_listeners.find(name);
			if (it != m_listeners.end()) {
				for (const EventListener& listener : it->second) {
					listener.listener_(argv);
				}
			}
		}

		void RemoveEventListener(const Event_Name name, uint32_t eventId) {
			auto it = m_listeners.find(name);
			if (it != m_listeners.end()) {
				auto& vec = it->second;
				vec.erase(std::remove_if(vec.begin(), vec.end(),
					[eventId](const EventListener& listener) {
						return listener.m_id == eventId;
					}), vec.end());
			}
		}

	private:

		uint32_t m_idIndex = 0;
		std::map<Event_Name, std::vector<EventListener>> m_listeners;

	};

}