#include "../include/Game.h"
#include <cassert>
#include <fstream>
#include <iostream>

int main() {
  // Create a small temporary level file under data/ so load_level can read it.
  const char *level_path = "data/levels/req_0001_level.json";
  // Ensure directory exists is handled by the Game ctor / copy_data in CMake when running tests from build.

  // Write a small JSON-like level with a non-zero start_score to exercise persistence
  std::ofstream ofs(level_path);
  if (!ofs) {
    std::cerr << "Failed to create temporary level file: " << level_path << "\n";
    return 2;
  }
  ofs << "{\n  \"title\": \"REQ-0001 Level\",\n  \"start_score\": 123\n}";
  ofs.close();

  Game g;
  bool ok = g.load_level(level_path);
  if (!ok) {
    std::cerr << "Failed to load temporary level: " << level_path << "\n";
    return 2;
  }

  // Save to a test save file and then load it back using a fresh Game instance
  const char *save_path = "saves/testsave_req_0001.txt";
  g.save_game(save_path);
  Game g2;
  bool loaded = g2.load_game(save_path);
  assert(loaded && "REQ-0001: save/load roundtrip failed");

  return 0;
}
