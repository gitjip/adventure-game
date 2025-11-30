# REQ-0002 Implementation Plan

ID: REQ-0002
Title: 完善游戏逻辑 — 实施计划（初始骨架）

目标
- 为后续实现生存、战斗、探索、合成和建造等系统准备可扩展的代码骨架。提供基础的系统接口、命令抽象和一个最小的驱动实现，便于单元测试与迭代开发。

工作分解（初始迭代）
1. 建立命令抽象：`Command` 基类与若干示例派生类（`MoveCommand`, `AttackCommand`）。
2. 生存系统骨架：`SurvivalSystem` 提供生命值、饥饿、口渴、温度的基本字段与 tick 更新接口。
3. 战斗系统骨架：`CombatSystem` 提供简单的攻击接口（用于后续扩展为回合制或实时）。
4. 将骨架加入 `adventure_core`，并保持现有 `Game` 类可直接使用这些系统。
5. 提交并切换分支（已在本分支进行）。

验收条件（本次迭代）
- 新增头文件 `include/Systems.h` 和实现文件 `src/Systems.cpp`，且能与现有工程编译通过。
- 在 `requirements/private/` 保存本实现计划（本文件）。

后续步骤（建议）
- 为每个子系统补充单元测试（例如 `tests/test_survival.cpp`）。
- 逐步实现战斗 AI、物品系统和关卡数据驱动机制。
