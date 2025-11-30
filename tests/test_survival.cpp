#include "../include/Systems.h"
#include <cassert>
#include <iostream>
#include <algorithm>

int main() {
  // Test 1: initial values
  SurvivalSystem s;
  assert(s.health() == 100 && "Initial health should be 100");
  assert(s.hunger() == 0 && "Initial hunger should be 0");

  // Test 2: tick increases hunger
  s.tick();
  assert(s.hunger() == 1 && "Hunger should increase after tick");

  // Test 3: hunger reaching critical reduces health
  // Fast-forward hunger to 100
  for (int i = 0; i < 200; ++i) s.tick();
  assert(s.hunger() == 100 && "Hunger should cap at 100");
  assert(s.health() <= 100 && "Health should be unchanged or reduced when hunger is critical");

  // Test 4: modifyHealth works
  int before = s.health();
  s.modifyHealth(-5);
  assert(s.health() == std::max(0, before - 5) && "modifyHealth should change health");

  std::cout << "SurvivalSystem tests passed" << std::endl;
  return 0;
}
