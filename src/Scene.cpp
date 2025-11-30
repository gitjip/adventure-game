#include "../include/Scene.h"

Scene::Scene(std::string name) : name_(std::move(name)) {}
Scene::~Scene() = default;

void Scene::addEvent(std::unique_ptr<Event> e) {
  events_.push_back(std::move(e));
}

void Scene::update() {
  // For now, trigger all events then clear them (simple model for tests)
  for (auto &e : events_) {
    if (e) e->trigger();
  }
  events_.clear();
}

const std::string &Scene::name() const { return name_; }
