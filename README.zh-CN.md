```markdown
# 冒险游戏（控制台）

这是一个用于 Windows（MinGW）、CMake 与 Boost 的最小控制台文本冒险示例项目。

构建与运行（PowerShell，MinGW-w64）：

1. 创建构建目录并运行 CMake（使用 “MinGW Makefiles” 生成器）：

```powershell
mkdir build; cd build
cmake -G "MinGW Makefiles" .. -DBOOST_ROOT="C:/path/to/boost"
mingw32-make
```

2. 运行可执行文件：

```powershell
./adventure_game.exe
```

说明：
- 如果 CMake 无法找到 Boost，可设置 `BOOST_ROOT` 环境变量，或在 `cmake` 命令行传入 `-DBOOST_ROOT=`。
- 例如：`-DBOOST_ROOT=C:/local/boost_1_80_0`。
- 本项目使用 `Boost.Filesystem` 来演示如何链接 Boost 组件；你可以在 `CMakeLists.txt` 中添加其它组件。

祝玩得开心 — 游戏启动后输入 `help` 查看命令说明。

保存与加载
- 在提示符输入 `save` 会将当前分数写入 `saves/lastsave.txt`。
- 输入 `load` 可从上一次保存恢复分数（若存在）。

快速构建脚本（PowerShell）
- 使用仓库内的 `build.ps1` 脚本可方便地在 MinGW/CMake 下配置并构建：

```powershell
# 在项目根目录运行；可选择性传入 Boost 路径
.\build.ps1 -BoostRoot 'C:/local/boost_1_80_0'
```

在 MinGW 下排查 Boost 问题
--------------------------------

如果 CMake 无法找到 Boost，或在 MinGW 构建时遇到缺少头文件 / 链接错误，请按照下列步骤尝试：

- 首选做法：在 `cmake` 命令中使用 `-DBOOST_ROOT=` 指向你的 MinGW Boost 安装，或设置 `BOOST_ROOT` 环境变量。
  示例（PowerShell）：

```powershell
cmake -G "MinGW Makefiles" .. -DBOOST_ROOT="D:/Users/damin/mingw64"
```

- 常见 MinGW 布局：Boost 头文件通常位于 `BOOST_ROOT/x86_64-w64-mingw32/include`，库文件位于 `BOOST_ROOT/lib`。
  有些 Boost 的 MinGW 分发不会提供完整的 CMake config 模块（例如 `boost_systemConfig.cmake`），这会导致 CMake 的自动检测失败。仓库中的 `CMakeLists.txt` 与 `build.ps1` 包含针对这种情况的兼容处理：

  - 检测并添加 triplet 前缀的 include 目录（例如 `x86_64-w64-mingw32/include`）。
  - 当 CMake 的 config 模式无法提供导入目标时，从 `BOOST_ROOT/lib` 手动选择合适的 filesystem 库文件。

- 架构不匹配：如果你的工具链是 x64，请确保链接的 Boost 库也是 x64（文件名中通常包含 `x64` 或 `x32`）。若出现对 Boost.Filesystem 符号的“undefined reference”，通常是因为选用了与工具链位宽不匹配的 Boost 库——请安装/指向匹配的 Boost 包或把 `BOOST_ROOT` 指向正确的分发版本。

- 如果你想完全依赖 Boost 自带的 CMake config 模式（并直接使用 `Boost::filesystem` 目标），请确保 Boost 安装包含各组件的 CMake 配置文件。否则本项目提供的兼容/手动检测路径会更可靠。

如果仍有问题，请把 CMake 的 configure 输出（`cmake` 控制台输出）与具体的编译/链接错误粘贴过来，我会继续协助诊断。

# adventure-game（仓库）
```
