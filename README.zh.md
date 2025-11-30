# 冒险游戏（控制台）

这是一个最小的控制台文字冒险示例，针对 Windows（MinGW）、CMake 和 Boost 初始化的演示项目。

构建与运行（PowerShell，MinGW-w64）：

1. 创建构建目录并运行 CMake（使用 "MinGW Makefiles" 生成器）：

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
- 如果 CMake 无法找到 Boost，请设置 `BOOST_ROOT` 环境变量或在 `cmake` 命令行传入 `-DBOOST_ROOT=`。
- 例如：`-DBOOST_ROOT=C:/local/boost_1_80_0`。
- 本项目使用 `Boost.Filesystem` 作为示例链接的 Boost 组件；你可以在 `CMakeLists.txt` 中添加其他组件。

开始游戏后输入 `help` 获取命令列表。

存档与读取
- 在提示符下输入 `save` 会将当前分数写入 `saves/lastsave.txt`。
- 输入 `load` 会从最近的保存文件恢复分数（如果存在）。

快速构建辅助（PowerShell）
- 使用项目附带的 `build.ps1` 脚本在 MinGW+CMake 下配置并构建：

```powershell
# 在项目根目录运行；可选地传入 Boost 路径
.\build.ps1 -BoostRoot 'C:/local/boost_1_80_0'
```

在 MinGW 上排查 Boost 问题
--------------------------------

如果 CMake 找不到 Boost，或在使用 MinGW 构建时遇到缺少头文件 / 未定义引用（undefined reference）错误，请按下列步骤排查：

- 首选：在 `cmake` 命令中使用 `-DBOOST_ROOT=` 指定 MinGW 的 Boost 安装路径，或设置 `BOOST_ROOT` 环境变量。
  示例（PowerShell）：

```powershell
cmake -G "MinGW Makefiles" .. -DBOOST_ROOT="D:/Users/damin/mingw64"
```

- 常见的 MinGW 布局：Boost 头文件通常位于 `BOOST_ROOT/x86_64-w64-mingw32/include`，库文件位于 `BOOST_ROOT/lib`。
  如果你安装的是 MinGW-w64 的 Boost，CMake 的自动检测有时会指向被截断的路径，或尝试使用某些发行版没有提供的 Boost "config" 包。项目中的 `CMakeLists.txt` 和 `build.ps1` 提供了兼容处理，具体包括：

  - 检测并添加带 triplet 前缀的 include 目录（例如 `x86_64-w64-mingw32/include`）。
  - 在 CMake 的 config 模式找不到导入目标时，从 `BOOST_ROOT/lib` 手动选择 filesystem 的库文件。

- 架构不匹配：如果你的工具链是 x64，请确保链接的是 x64 版本的 Boost 库（库名中通常包含 `x64` 或 `x32`）。如果出现对 Boost.Filesystem 符号的未定义引用错误，通常是因为选中了 32 位的 Boost 库而使用了 64 位的编译器——请安装/使用对应架构的 Boost，或将 `BOOST_ROOT` 指向正确的分发目录。

- 如果你希望只使用 Boost 的 CMake config 模式（即现代的 `Boost::filesystem` 导入目标），请确保你的 Boost 安装包含每个组件的 CMake 配置文件（例如 `boost_systemConfig.cmake`）。否则请使用本项目采用的兼容/手动检测方式。

如果你仍有问题，请贴出 CMake 的配置输出（`cmake` 的输出）和具体的编译/链接错误信息，我会继续帮你诊断。

---

（此文件为 `README.md` 的中文翻译）
