#include "../include/SceneLoader.h"
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"

using nlohmann::json;

SceneLoader::SceneLoader() = default;

std::unique_ptr<Scene> SceneLoader::load(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs) return nullptr;

  json j;
  try {
    ifs >> j;
  } catch (const json::parse_error &) {
    return nullptr;
  }

  std::string scene_name = path;
  if (j.contains("name") && j["name"].is_string()) scene_name = j["name"].get<std::string>();

  auto scene = std::make_unique<Scene>(scene_name);

  if (!j.contains("events") || !j["events"].is_array()) return scene;

  for (const auto &ev : j["events"]) {
    std::string etype;
    if (ev.contains("type") && ev["type"].is_string()) etype = ev["type"].get<std::string>();

    std::string message;
    int amount = 0;
    if (ev.contains("params") && ev["params"].is_object()) {
      const auto &p = ev["params"];
      if (p.contains("message") && p["message"].is_string()) message = p["message"].get<std::string>();
      if (p.contains("amount") && p["amount"].is_number_integer()) amount = p["amount"].get<int>();
    }

    if (etype == "greet") {
      scene->addEvent(std::make_unique<Event>(etype, [this, message]() { messages_.push_back(message); }));
    } else if (etype == "inc") {
      scene->addEvent(std::make_unique<Event>(etype, [this, amount]() { counter_ += amount; }));
    } else if (!etype.empty()) {
      scene->addEvent(std::make_unique<Event>(etype, [this, etype]() { messages_.push_back(etype); }));
    }
  }

  return scene;
}

const std::vector<std::string> &SceneLoader::messages() const { return messages_; }
int SceneLoader::counter() const { return counter_; }
