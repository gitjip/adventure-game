#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Event.h"

class Scene {
public:
  explicit Scene(std::string name);
  ~Scene();
  void addEvent(std::unique_ptr<Event> e);
  void update();
  const std::string &name() const;
private:
  std::string name_;
  std::vector<std::unique_ptr<Event>> events_;
};
