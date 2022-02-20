#pragma once

#include <utility>

#include "../../../wde.hpp"
#include "Observer.hpp"

namespace wde::core {
	class Subject {
		public:
			/**
			 * Create a new observers subject
			 * @param label The name of the subject
			 */
			explicit Subject(std::string label) : _label(std::move(label)) {}


			// Observers handling
			/**
			 * Add an observer to the watching list on the back of the stack
			 * @param observer
			 * @param pushTop True if the observer should be put on top of the list
			 */
			void addObserver(std::shared_ptr<Observer> observer, bool pushTop = false) {
				if (pushTop)
					_observers.insert(_observers.begin(), std::move(observer));
				else
					_observers.push_back(std::move(observer));
			}



			/**
			 * Send a notification to the listening observers
			 * @param event Payload
			 */
			void notify(const Event& event) {
				for (auto& obs : _observers)
					obs->onNotify(event);
			}


			// Utils
			/** @return The identification label of the subject used for debug */
			std::string getLabel() {
				return _label;
			}


		private:
			std::string _label;
			std::vector<std::shared_ptr<Observer>> _observers {};
	};
}
