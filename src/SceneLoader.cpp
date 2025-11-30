#include "../include/SceneLoader.h"
#include <fstream>
#include <sstream>
// Hand-rolled lightweight JSON-like parser sufficient for our sample files.
// This avoids external dependencies and keeps parsing simple and forgiving.
#include <cctype>

SceneLoader::SceneLoader() = default;

static void skip_ws(const std::string &s, size_t &p) {
  while (p < s.size() && std::isspace((unsigned char)s[p])) ++p;
}

// Extract a quoted string starting at the first '"' at or after p.
static std::string extract_quoted(const std::string &s, size_t &p) {
  size_t q = s.find('"', p);
  if (q == std::string::npos) return {};
  ++q; // position after opening quote
  std::string out;
  while (q < s.size()) {
    char c = s[q++];
    if (c == '"') break;
    if (c == '\\' && q < s.size()) {
      char esc = s[q++];
      if (esc == 'n') out.push_back('\n');
      else out.push_back(esc);
    } else {
      out.push_back(c);
    }
  }
  p = q;
  return out;
}

// Find the ':' after a key (which may start at key_pos) and return position after it.
static size_t find_colon_after_key(const std::string &s, size_t key_pos) {
  size_t colon = s.find(':', key_pos);
  if (colon == std::string::npos) return std::string::npos;
  ++colon;
  return colon;
}

std::unique_ptr<Scene> SceneLoader::load(const std::string &path) {
  std::ifstream ifs(path);
  if (!ifs) return nullptr;
  std::stringstream ss; ss << ifs.rdbuf();
  std::string content = ss.str();

  // Default scene name is the filename
  std::string scene_name = path;

  // Try to read top-level "name"
  size_t name_key = content.find("\"name\"");
  if (name_key != std::string::npos) {
    size_t after = find_colon_after_key(content, name_key);
    if (after != std::string::npos) {
      scene_name = extract_quoted(content, after);
      if (scene_name.empty()) scene_name = path;
    }
  }

  auto scene = std::make_unique<Scene>(scene_name);

  // Find events array
  size_t events_key = content.find("\"events\"");
  if (events_key == std::string::npos) return scene;
  size_t arr_open = content.find('[', events_key);
  if (arr_open == std::string::npos) return scene;

  size_t pos = arr_open + 1;
  while (true) {
    // Find the next object start
    size_t obj_start = content.find('{', pos);
    if (obj_start == std::string::npos) break;
    size_t obj_end = content.find('}', obj_start);
    if (obj_end == std::string::npos) break;

    // Parse fields inside this object
    std::string etype;
    std::string message;
    int amount = 0;

    // find "type"
    size_t tkey = content.find("\"type\"", obj_start);
    if (tkey != std::string::npos && tkey < obj_end) {
      size_t after = find_colon_after_key(content, tkey);
      if (after != std::string::npos && after < obj_end) {
        etype = extract_quoted(content, after);
      }
    }

    // find params.message
    size_t mkey = content.find("\"message\"", obj_start);
    if (mkey != std::string::npos && mkey < obj_end) {
      size_t after = find_colon_after_key(content, mkey);
      if (after != std::string::npos && after < obj_end) {
        message = extract_quoted(content, after);
      }
    }

    // find params.amount (simple integer parsing)
    size_t akey = content.find("\"amount\"", obj_start);
    if (akey != std::string::npos && akey < obj_end) {
      size_t after = find_colon_after_key(content, akey);
      if (after != std::string::npos && after < obj_end) {
        // skip to first digit or '-'
        size_t p = after;
        while (p < obj_end && !std::isdigit((unsigned char)content[p]) && content[p] != '-') ++p;
        if (p < obj_end) {
          try {
            amount = std::stoi(content.substr(p, obj_end - p));
          } catch (...) { amount = 0; }
        }
      }
    }

    // Create event action
    if (etype == "greet") {
      scene->addEvent(std::make_unique<Event>(etype, [this, message]() {
        messages_.push_back(message);
      }));
    } else if (etype == "inc") {
      scene->addEvent(std::make_unique<Event>(etype, [this, amount]() {
        counter_ += amount;
      }));
    } else if (!etype.empty()) {
      scene->addEvent(std::make_unique<Event>(etype, [this, etype]() {
        messages_.push_back(etype);
      }));
    }

    pos = obj_end + 1;
  }

  return scene;
}

const std::vector<std::string> &SceneLoader::messages() const { return messages_; }
int SceneLoader::counter() const { return counter_; }
