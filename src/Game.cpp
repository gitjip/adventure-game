#include "Game.h"
#include <iostream>
#include <fstream>
#include <boost/filesystem.hpp>
#include <cctype>

namespace fs = boost::filesystem;

Game::Game(): score_(0) {
  // Ensure a save directory exists (demonstrates Boost usage)
  try {
    fs::path save_dir("saves");
    if (!fs::exists(save_dir)) {
      fs::create_directory(save_dir);
    }
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Warning: could not create saves directory: " << e.what() << '\n';
  }
}

void Game::show_intro() {
  std::cout << "Welcome to Adventure Game!\n";
  std::cout << "Type `help` for a list of commands.\n\n";
}

void Game::game_loop() {
  std::string cmd;
  bool running = true;

  while (running) {
    std::cout << "> ";
    if (!std::getline(std::cin, cmd)) break;
    if (cmd.empty()) continue;
    running = handle_command(cmd);
  }
}

bool Game::handle_command(const std::string &cmd) {
  if (cmd == "quit" || cmd == "exit") {
    std::cout << "Goodbye!\n";
    return false;
  }
  if (cmd == "help") {
    std::cout << "Commands: help, look, score, quit\n";
    return true;
  }
  if (cmd == "look") {
    std::cout << "You look around. Nothing special here, but your imagination is rich.\n";
    score_ += 1;
    return true;
  }
  if (cmd == "score") {
    std::cout << "Score: " << score_ << "\n";
    return true;
  }
  if (cmd == "save") {
    save_game();
    return true;
  }
  if (cmd == "load") {
    if (load_game()) {
      std::cout << "Game loaded. Score: " << score_ << "\n";
    } else {
      std::cout << "No save found or failed to load.\n";
    }
    return true;
  }
  if (cmd.rfind("loadlevel ", 0) == 0) {
    std::string path = cmd.substr(std::string("loadlevel ").length());
    if (load_level(path)) {
      std::cout << "Level loaded from " << path << "\n";
    } else {
      std::cout << "Failed to load level: " << path << "\n";
    }
    return true;
  }

  std::cout << "Unknown command: '" << cmd << "' (type help)\n";
  return true;
}

void Game::run() {
  show_intro();
  game_loop();
}

void Game::save_game(const std::string &path) {
  try {
    fs::path p(path);
    fs::path parent = p.parent_path();
    if (!parent.empty() && !fs::exists(parent)) fs::create_directories(parent);
    std::ofstream ofs(p.string(), std::ios::trunc);
    if (!ofs) {
      std::cerr << "Failed to open save file for writing: " << p.string() << '\n';
      return;
    }
    ofs << score_ << '\n';
    ofs.close();
    std::cout << "Game saved to " << p.string() << "\n";
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Save failed: " << e.what() << '\n';
  }
}

bool Game::load_game(const std::string &path) {
  try {
    fs::path p(path);
    if (!fs::exists(p)) return false;
    std::ifstream ifs(p.string());
    if (!ifs) return false;
    int s = 0;
    ifs >> s;
    if (ifs.fail()) return false;
    score_ = s;
    return true;
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Load failed: " << e.what() << '\n';
    return false;
  }
}

// Very small, dependency-free level loader for simple JSON-like files.
bool Game::load_level(const std::string &path) {
  try {
    fs::path p(path);
    if (!fs::exists(p)) return false;
    std::ifstream ifs(p.string());
    if (!ifs) return false;
    std::string contents((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
    // naive parse: look for "start_score": <number>
    auto pos = contents.find("\"start_score\"");
    if (pos != std::string::npos) {
      auto colon = contents.find(':', pos);
      if (colon != std::string::npos) {
        std::string numstr;
        size_t i = colon + 1;
        while (i < contents.size() && (std::isspace((unsigned char)contents[i]) || contents[i] == '"')) ++i;
        while (i < contents.size() && (std::isdigit((unsigned char)contents[i]) || contents[i] == '-')) { numstr += contents[i++]; }
        if (!numstr.empty()) {
          try { score_ = std::stoi(numstr); } catch(...) { /* ignore */ }
        }
      }
    }
    return true;
  } catch (const fs::filesystem_error &e) {
    std::cerr << "Load level failed: " << e.what() << '\n';
    return false;
  }
}
