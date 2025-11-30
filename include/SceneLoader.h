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
  // (Parsing implementation is provided in the .cpp and may use a JSON
  // library. No helper declarations are required here.)
};
