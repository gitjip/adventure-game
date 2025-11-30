#pragma once

#include <string>
#include <functional>

class Event {
public:
  Event(std::string type, std::function<void()> action);
  virtual ~Event();
  const std::string &type() const;
  void trigger();
private:
  std::string type_;
  std::function<void()> action_;
};
