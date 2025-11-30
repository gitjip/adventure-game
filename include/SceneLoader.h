#pragma once

#include <memory>
#include <string>
#include <vector>
#include "Scene.h"

// Minimal SceneLoader for REQ-0003: parses a very small JSON subset used
// by our sample files. It does not aim to be a general JSON parser.
class SceneLoader {
public:
  SceneLoader();
  // load a scene from file path (relative or absolute). Returns nullptr on error.
  std::unique_ptr<Scene> load(const std::string &path);

  // Access captured messages produced by events (for test verification).
  const std::vector<std::string> &messages() const;
  int counter() const;

private:
  std::vector<std::string> messages_;
  int counter_ = 0;

  // Helpers used by the simple parser
  static std::string extract_string(const std::string &s, size_t &pos);
  static void skip_whitespace(const std::string &s, size_t &pos);
};
