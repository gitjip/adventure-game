#include "../include/SceneLoader.h"
#include <cassert>
#include <iostream>

int main() {
  SceneLoader loader;
  // data is copied to build/data by copy_data target; use that path
  auto scene = loader.load("data/scenes/sample_scene.json");
  assert(scene && "Scene should load");

  scene->update();
  // sample_scene.json uses a greet event with message "You enter the clearing."
  const auto &msgs = loader.messages();
  assert(!msgs.empty() && msgs[0] == "You enter the clearing." && "Greet message should be captured");

  std::cout << "SceneLoader tests passed" << std::endl;
  return 0;
}
