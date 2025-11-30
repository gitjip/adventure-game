#include "../include/Scene.h"
#include "../include/Event.h"
#include <cassert>
#include <iostream>

int main() {
  int counter = 0;
  Scene s("test_scene");
  // create an event that increments counter
  s.addEvent(std::make_unique<Event>("inc", [&counter]() { counter += 1; }));
  s.addEvent(std::make_unique<Event>("inc", [&counter]() { counter += 2; }));

  s.update();
  assert(counter == 3 && "Events should have incremented counter to 3");

  std::cout << "Scene/Event tests passed" << std::endl;
  return 0;
}
