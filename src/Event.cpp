#include "../include/Event.h"

Event::Event(std::string type, std::function<void()> action)
  : type_(std::move(type)), action_(std::move(action)) {}

Event::~Event() = default;

const std::string &Event::type() const { return type_; }

void Event::trigger() {
  if (action_) action_();
}
