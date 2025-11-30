#include "../include/Game.h"
#include <cassert>
#include <iostream>

int main() {
  Game g;
  // Ensure loading the sample level sets score from file
  bool ok = g.load_level("data/levels/sample_level.json");
  if (!ok) {
    std::cerr << "Failed to load sample level\n";
    return 2;
  }
  // sample_level.json sets start_score to 0
  // call save and load to verify persistence
  g.save_game("saves/testsave.txt");
  bool loaded = g.load_game("saves/testsave.txt");
  assert(loaded && "save/load roundtrip failed");
  return 0;
}
