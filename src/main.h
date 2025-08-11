#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QListWidget>
#include <QTextEdit>
#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTabWidget>
#include <QTableWidget>
#include <QSplitter>
#include <random>

#include "bullettracker.h"
#include "itemmanager.h"
#include "decisionhelper.h"
#include "bullettypewidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNewRound();
    void onFireBullet();
    void onAddKnownBullet();
    void onResetGame();
    void onCalculateProbability();
    void onGetDecisionAdvice();
    void onRandomChoice();

private:
    void setupUI();
    void setupBulletTracker();
    void setupItemManager();
    void setupDecisionHelper();
    void updateDisplay();
    void updateProbability();
    void updateItemLists();

    // UI组件
    QWidget *m_centralWidget;
    QTabWidget *m_tabWidget;
    
    // 子弹追踪标签页
    QWidget *m_bulletTab;
    QSpinBox *m_liveBulletsSpinBox;
    QSpinBox *m_blankBulletsSpinBox;
    QPushButton *m_newRoundButton;
    QTableWidget *m_bulletTable;
    QLabel *m_remainingLiveLabel;
    QLabel *m_remainingBlankLabel;
    QLabel *m_probabilityLabel;
    QProgressBar *m_probabilityBar;
    QPushButton *m_randomChoiceButton;
    
    // 已知信息标签页
    QWidget *m_knownTab;
    QTableWidget *m_knownBulletsTable;
    QPushButton *m_addKnownButton;
    
    // 道具管理标签页
    QWidget *m_itemTab;
    QGroupBox *m_playerItemsGroup;
    QGroupBox *m_dealerItemsGroup;
    QListWidget *m_playerItemsList;
    QListWidget *m_dealerItemsList;
    
    // 决策建议标签页
    QWidget *m_adviceTab;
    QTextEdit *m_adviceTextEdit;
    QPushButton *m_getAdviceButton;
    
    // 核心逻辑组件
    BulletTracker *m_bulletTracker;
    ItemManager *m_itemManager;
    DecisionHelper *m_decisionHelper;
    
    // 随机数生成器
    std::random_device m_randomDevice;
    std::mt19937 m_randomGenerator;
    std::uniform_real_distribution<double> m_distribution;
};
