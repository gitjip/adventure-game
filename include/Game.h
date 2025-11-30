#pragma once
#include <string>

class Game {
public:
  Game();
  void run();

  // Public helpers for persistence and level loading (used by tests)
  void save_game(const std::string &path = "saves/lastsave.txt");
  bool load_game(const std::string &path = "saves/lastsave.txt");
  bool load_level(const std::string &path);

private:
  void show_intro();
  void game_loop();
  bool handle_command(const std::string &cmd);
  int score_;
};
