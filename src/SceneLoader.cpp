#include "../include/SceneLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>

SceneLoader::SceneLoader() = default;

void SceneLoader::skip_whitespace(const std::string &s, size_t &pos) {
  while (pos < s.size() && isspace((unsigned char)s[pos])) ++pos;
}

std::string SceneLoader::extract_string(const std::string &s, size_t &pos) {
  skip_whitespace(s, pos);
  if (pos >= s.size() || s[pos] != '"') return {};
  ++pos; // skip opening
  std::string out;
  while (pos < s.size()) {
    char c = s[pos++];
    if (c == '"') break;
    if (c == '\\' && pos < s.size()) {
      char esc = s[pos++];
      if (esc == 'n') out.push_back('\n');
      else out.push_back(esc);
    } else {
      out.push_back(c);
    }
  }
  return out;
}

std::unique_ptr<Scene> SceneLoader::load(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs) return nullptr;
  std::stringstream ss; ss << ifs.rdbuf();
  std::string content = ss.str();

  size_t pos = 0;
  // find "name"
  size_t name_pos = content.find("\"name\"");
  std::string scene_name = "loaded_scene";
  if (name_pos != std::string::npos) {
    pos = name_pos + 6;
    auto n = extract_string(content, pos);
    if (!n.empty()) scene_name = n;
  }

  auto scene = std::make_unique<Scene>(scene_name);

  // find events array
  size_t events_pos = content.find("\"events\"");
  if (events_pos == std::string::npos) return scene;
  pos = events_pos;
  // simple loop: find occurrences of "type" within events
  while (true) {
    size_t type_pos = content.find("\"type\"", pos);
    if (type_pos == std::string::npos) break;
    pos = type_pos + 6;
    std::string etype = extract_string(content, pos);

    // look for params.message or params.amount
    size_t message_pos = content.find("\"message\"", pos);
    std::string message;
    if (message_pos != std::string::npos && message_pos < content.find(']', events_pos)) {
      size_t p = message_pos + 9;
      message = extract_string(content, p);
    }

    size_t amount_pos = content.find("\"amount\"", pos);
    int amount = 0;
    if (amount_pos != std::string::npos && amount_pos < content.find(']', events_pos)) {
      size_t p = amount_pos + 8;
      // parse simple integer after colon
      while (p < content.size() && !isdigit((unsigned char)content[p]) && content[p] != '-') ++p;
      if (p < content.size()) {
        amount = std::stoi(content.substr(p));
      }
    }

    // Create event actions that capture this loader instance
    if (etype == "greet") {
      scene->addEvent(std::make_unique<Event>(etype, [this, message]() {
        messages_.push_back(message);
      }));
    } else if (etype == "inc") {
      scene->addEvent(std::make_unique<Event>(etype, [this, amount]() {
        counter_ += amount;
      }));
    } else {
      // default: push type to messages
      scene->addEvent(std::make_unique<Event>(etype, [this, etype]() {
        messages_.push_back(etype);
      }));
    }

    pos = pos + 1;
  }

  return scene;
}

const std::vector<std::string> &SceneLoader::messages() const { return messages_; }
int SceneLoader::counter() const { return counter_; }
