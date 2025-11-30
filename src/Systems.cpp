#include "../include/Systems.h"
#include <iostream>

// MoveCommand
MoveCommand::MoveCommand(const std::string &dir): dir_(dir) {}
void MoveCommand::execute() { std::cout << "MoveCommand: " << dir_ << std::endl; }

// AttackCommand
AttackCommand::AttackCommand() {}
void AttackCommand::execute() { std::cout << "AttackCommand executed" << std::endl; }

// SurvivalSystem
SurvivalSystem::SurvivalSystem(): health_(100), hunger_(0), thirst_(0), temperature_(37) {}
void SurvivalSystem::tick() {
  // placeholder: simple hunger increase
  hunger_ += 1;
  if (hunger_ > 100) hunger_ = 100;
  // If hunger is critical, decrease health as a penalty
  if (hunger_ >= 100) {
    health_ -= 10;
    if (health_ < 0) health_ = 0;
  }
}
int SurvivalSystem::health() const { return health_; }
int SurvivalSystem::hunger() const { return hunger_; }
void SurvivalSystem::modifyHealth(int delta) {
  health_ += delta;
  if (health_ < 0) health_ = 0;
  if (health_ > 100) health_ = 100;
}

// CombatSystem
CombatSystem::CombatSystem() {}
void CombatSystem::attack() { std::cout << "CombatSystem::attack()" << std::endl; }
