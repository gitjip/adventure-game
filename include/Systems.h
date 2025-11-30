#pragma once

#include <string>

// Command abstraction for player inputs and scripted actions
class Command {
public:
  virtual ~Command() = default;
  virtual void execute() = 0;
};

// Example command types (minimal placeholders)
class MoveCommand : public Command {
public:
  MoveCommand(const std::string &dir);
  void execute() override;
private:
  std::string dir_;
};

class AttackCommand : public Command {
public:
  AttackCommand();
  void execute() override;
};

// Survival system: manages health, hunger, thirst, temperature
class SurvivalSystem {
public:
  SurvivalSystem();
  void tick();
  int health() const;
  int hunger() const;
  void modifyHealth(int delta);
private:
  int health_;
  int hunger_;
  int thirst_;
  int temperature_; // basic integer placeholder
};

// Combat system: minimal interface for attacking
class CombatSystem {
public:
  CombatSystem();
  void attack();
};
