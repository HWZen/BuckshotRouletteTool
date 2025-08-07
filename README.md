# BuckshotRouletteTool

**[English Description / 英文说明](#english-description)**

## 中文说明 / Chinese Description

### 项目简介

BuckshotRouletteTool 是为游戏 **Buckshot Roulette** 设计的辅助工具。这是一个**非外挂**的信息记录和决策辅助工具，旨在帮助玩家更好地理解游戏机制并做出策略性决策。

> **重要说明**：本工具仅用于已知信息记录和分析，所有信息均需用户手动输入（未来考虑捕获游戏数据，自动记录）。我们**严格反对**将任何形式的外挂使用于联网对局，但不反对单机游戏中的辅助工具使用。

### 游戏简介

Buckshot Roulette 是一款由 Mike Klubnika 开发的独立恐怖策略游戏。玩家需要与神秘的庄家进行一场生死较量，使用霰弹枪进行俄式轮盘游戏。游戏包含实弹和空包弹，以及各种战略道具。

### 核心功能

#### 🎯 子弹追踪系统
- **回合管理**：记录每轮的实弹和空包弹数量
- **历史追踪**：记录已发射子弹的类型和顺序
- **剩余统计**：实时显示剩余的实弹和空包弹数量
- **概率计算**：智能计算下一发子弹是实弹的概率

#### 🔍 已知信息管理
- **道具信息**：记录通过放大镜、一次性电话等道具获得的子弹信息
- **信息整合**：将已知信息与概率计算相结合
- **可视化显示**：直观展示已知的子弹位置和类型

#### 🎒 道具管理系统
- **玩家道具**：追踪玩家拥有的所有道具
- **庄家道具**：记录庄家的道具状态
- **道具效果**：详细说明每个道具的功能和使用时机

支持的道具包括：
- 🔍 **放大镜**：查看当前子弹类型
- 🚬 **香烟**：恢复1点生命值
- 🍺 **啤酒**：弹出并跳过当前子弹
- 🪚 **手锯**：下一发实弹造成双倍伤害
- ⛓️ **手铐**：强制对手跳过下一回合
- 📞 **一次性电话**：查看随机位置的子弹类型
- 🔄 **逆变器**：改变当前子弹的类型（实弹↔空包弹）
- 💉 **肾上腺素**：偷取并立即使用对手的一个道具
- 💊 **过期药物**：50%概率恢复2生命，否则失去1生命

#### 🤖 AI决策建议
- **局势分析**：基于当前游戏状态进行深度分析
- **策略建议**：提供最优的行动方案
- **风险评估**：计算不同选择的期望值
- **道具建议**：推荐最佳的道具使用时机

### 使用方法
1. **开始新回合**：在"子弹追踪"标签页中设置实弹和空包弹数量
2. **记录开火**：每次开火后点击对应的按钮记录结果
3. **添加已知信息**：在"已知信息"标签页中记录通过道具获得的信息
4. **管理道具**：在"道具管理"标签页中记录双方的道具状态
5. **获取建议**：在"决策建议"标签页中获取AI分析和策略建议

### 界面预览

工具采用标签页设计，包含以下主要界面：

1. **子弹追踪**：核心功能界面，显示当前回合状态和历史记录
2. **已知信息**：管理通过道具获得的子弹信息
3. **道具管理**：追踪双方的道具状态
4. **决策建议**：获取AI分析和策略建议

### 技术规格

- **开发语言**：C++23
- **图形界面**：Qt 5/6
- **构建系统**：XMake
- **支持平台**：Windows (主要)，未来计划支持 Linux 和 macOS
- **编译器**：支持现代C++编译器

### 安装

#### 环境要求
- Windows 10/11
- Qt 5.12+ 或 Qt 6.x
- 支持 C++23 的编译器
- XMake 构建工具

#### 编译步骤
```bash
# 克隆仓库
git clone https://github.com/HWZen/BuckshotRouletteTool.git
cd BuckshotRouletteTool

# 配置项目（可选指定平台和架构）
xmake f -p windows -a x64 -m release

# 编译项目
xmake

# 运行程序
xmake run BuckshotRouletteTool
```

### 未来计划

- [ ] **自动数据捕获**：研究游戏数据捕获技术，实现自动信息记录

### 贡献

欢迎任何形式的贡献！包括但不限于代码改进、功能建议、问题反馈、文档完善等。

### 许可证

本项目采用 MIT 许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

### 免责声明

- 本工具仅供学习和娱乐使用
- 严格反对将任何形式的外挂使用于联网对局
- 使用本工具的风险由用户自行承担
- 开发者不对使用本工具造成的任何后果负责

---

## English Description

### Project Overview

BuckshotRouletteTool is an auxiliary tool designed for the game **Buckshot Roulette**. This is a **non-cheat** information recording and decision-making assistance tool, aimed at helping players better understand game mechanics and make strategic decisions.

> **Important Note**: This tool is only for information recording and analysis. All information must be manually entered by users (future consideration for capturing game data automatically). We **strictly oppose** the use of any form of cheats in online multiplayer games, but do not oppose the use of auxiliary tools in single-player games.

### About Buckshot Roulette

Buckshot Roulette is an indie horror strategy game developed by Mike Klubnika. Players engage in a life-or-death showdown with a mysterious dealer, playing Russian roulette with a shotgun. The game features live shells, blank shells, and various strategic items.

### Core Features

#### 🎯 Bullet Tracking System
- **Round Management**: Record live and blank shells for each round
- **History Tracking**: Track fired bullets' types and sequence
- **Remaining Statistics**: Real-time display of remaining live and blank shells
- **Probability Calculation**: Smart calculation of next bullet's live probability

#### 🔍 Known Information Management
- **Item Information**: Record bullet information obtained through magnifying glass, burner phone, etc.
- **Information Integration**: Combine known information with probability calculations
- **Visual Display**: Intuitive display of known bullet positions and types

#### 🎒 Item Management System
- **Player Items**: Track all items owned by the player
- **Dealer Items**: Record dealer's item status
- **Item Effects**: Detailed description of each item's function and usage timing

#### 🤖 AI Decision Assistance
- **Situation Analysis**: Deep analysis based on current game state
- **Strategic Advice**: Provide optimal action plans
- **Risk Assessment**: Calculate expected values of different choices
- **Item Recommendations**: Suggest optimal timing for item usage

### How to Use
1. **Start New Round**: Set the number of live and blank shells in the "Bullet Tracking" tab
2. **Record Shots**: Click corresponding buttons after each shot to record results
3. **Add Known Information**: Record information obtained through items in the "Known Information" tab
4. **Manage Items**: Record both parties' item status in the "Item Management" tab
5. **Get Advice**: Obtain AI analysis and strategic suggestions in the "Decision Advice" tab

### Interface Overview

The tool uses a tabbed design with the following main interfaces:

1. **Bullet Tracking**: Core functionality interface showing current round status and history
2. **Known Information**: Manage bullet information obtained through items
3. **Item Management**: Track both parties' item status
4. **Decision Advice**: Get AI analysis and strategic advice

### Technical Specifications

- **Language**: C++23
- **GUI Framework**: Qt 5/6
- **Build System**: XMake
- **Supported Platforms**: Windows (primary), Linux and macOS planned
- **Compiler**: Modern C++ compilers with C++23 support

### Installation

#### Requirements
- Windows 10/11
- Qt 5.12+ or Qt 6.x
- C++23 compatible compiler
- XMake build tool

#### Build Instructions
```bash
# Clone repository
git clone https://github.com/HWZen/BuckshotRouletteTool.git
cd BuckshotRouletteTool

# Configure project (optional platform and architecture)
xmake f -p windows -a x64 -m release

# Build project
xmake

# Run program
xmake run BuckshotRouletteTool
```

### Contributing

Contributions of any form are welcome! Including but not limited to code improvements, feature suggestions, bug reports, documentation enhancements, etc.

### License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

### Disclaimer

- This tool is for educational and entertainment purposes only
- Strictly oppose the use of any form of cheats in online multiplayer games
- Users assume all risks of using this tool
- Developers are not responsible for any consequences of using this tool

---

## 联系方式 / Contact

- **作者 / Author**: HWZen
- **项目主页 / Project Home**: https://github.com/HWZen/BuckshotRouletteTool
- **问题反馈 / Issues**: https://github.com/HWZen/BuckshotRouletteTool/issues

## 致谢 / Acknowledgments

- **Buckshot Roulette** 游戏开发者 Mike Klubnika
- Qt 开源社区
- XMake 构建工具
- 所有贡献者和用户
