#pragma once
#include <string>

class Game {
public:
  Game();
  void run();
private:
  void show_intro();
  void game_loop();
  bool handle_command(const std::string &cmd);
  void save_game(const std::string &path = "saves/lastsave.txt");
  bool load_game(const std::string &path = "saves/lastsave.txt");
  bool load_level(const std::string &path);
  int score_;
};
