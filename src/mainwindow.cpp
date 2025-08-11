#include "main.h"
#include "version.h"
#include "bullettypewidget.h"
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QTimer>
#include <QDebug>

// MainWindow实现
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_tabWidget(nullptr)
    , m_bulletTracker(nullptr)
    , m_itemManager(nullptr)
    , m_decisionHelper(nullptr)
{
    setWindowTitle(QString("BuckshotRouletteTool %1").arg(PROJECT_VERSION));
    setMinimumSize(1000, 700);
    
    // 初始化核心组件
    m_bulletTracker = new BulletTracker(this);
    m_itemManager = new ItemManager(this);
    m_decisionHelper = new DecisionHelper(this);
    
    // 连接信号
    connect(m_bulletTracker, &BulletTracker::probabilityChanged, 
            this, &MainWindow::updateProbability);
    
    setupUI();
    updateDisplay();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_centralWidget);
    
    // 创建标签页
    m_tabWidget = new QTabWidget;
    mainLayout->addWidget(m_tabWidget);
    
    setupBulletTracker();
    setupItemManager();
    setupDecisionHelper();
    
    // 添加重置按钮
    QPushButton *resetButton = new QPushButton("重置游戏");
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::onResetGame);
    mainLayout->addWidget(resetButton);
}

void MainWindow::setupBulletTracker()
{
    m_bulletTab = new QWidget;
    m_tabWidget->addTab(m_bulletTab, "子弹追踪");
    
    QVBoxLayout *mainLayout = new QVBoxLayout(m_bulletTab);
    
    // 顶部：回合设置和统计
    QHBoxLayout *topLayout = new QHBoxLayout;
    
    // 新回合设置
    QGroupBox *roundGroup = new QGroupBox("新回合设置");
    QGridLayout *roundLayout = new QGridLayout(roundGroup);
    
    roundLayout->addWidget(new QLabel("实弹数量:"), 0, 0);
    m_liveBulletsSpinBox = new QSpinBox;
    m_liveBulletsSpinBox->setRange(1, 8);
    m_liveBulletsSpinBox->setValue(2);
    roundLayout->addWidget(m_liveBulletsSpinBox, 0, 1);
    
    roundLayout->addWidget(new QLabel("空包弹数量:"), 1, 0);
    m_blankBulletsSpinBox = new QSpinBox;
    m_blankBulletsSpinBox->setRange(1, 8);
    m_blankBulletsSpinBox->setValue(2);
    roundLayout->addWidget(m_blankBulletsSpinBox, 1, 1);
    
    m_newRoundButton = new QPushButton("开始新回合");
    connect(m_newRoundButton, &QPushButton::clicked, this, &MainWindow::onNewRound);
    roundLayout->addWidget(m_newRoundButton, 2, 0, 1, 2);
    
    topLayout->addWidget(roundGroup);
    
    // 当前状态
    QGroupBox *statusGroup = new QGroupBox("当前状态");
    QGridLayout *statusLayout = new QGridLayout(statusGroup);
    
    statusLayout->addWidget(new QLabel("剩余实弹:"), 0, 0);
    m_remainingLiveLabel = new QLabel("0");
    m_remainingLiveLabel->setStyleSheet("font-weight: bold; color: red;");
    statusLayout->addWidget(m_remainingLiveLabel, 0, 1);
    
    statusLayout->addWidget(new QLabel("剩余空包弹:"), 1, 0);
    m_remainingBlankLabel = new QLabel("0");
    m_remainingBlankLabel->setStyleSheet("font-weight: bold; color: blue;");
    statusLayout->addWidget(m_remainingBlankLabel, 1, 1);
    
    statusLayout->addWidget(new QLabel("实弹概率:"), 2, 0);
    m_probabilityLabel = new QLabel("0%");
    m_probabilityLabel->setStyleSheet("font-weight: bold; color: green;");
    statusLayout->addWidget(m_probabilityLabel, 2, 1);
    
    m_probabilityBar = new QProgressBar;
    m_probabilityBar->setRange(0, 100);
    m_probabilityBar->setValue(0);
    statusLayout->addWidget(m_probabilityBar, 3, 0, 1, 2);
    
    topLayout->addWidget(statusGroup);
    
    mainLayout->addLayout(topLayout);
    
    // 子弹追踪表格
    QGroupBox *bulletTableGroup = new QGroupBox("子弹追踪表格");
    QVBoxLayout *bulletTableLayout = new QVBoxLayout(bulletTableGroup);
    
    m_bulletTable = new QTableWidget(0, 4);
    m_bulletTable->setHorizontalHeaderLabels({"位置", "实际类型", "道具已知", "状态"});
    m_bulletTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_bulletTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_bulletTable->setAlternatingRowColors(true);
    
    // 设置表格样式
    m_bulletTable->setStyleSheet(
        "QTableWidget {"
        "    gridline-color: #d0d0d0;"
        "    background-color: white;"
        "}"
        "QTableWidget::item {"
        "    border-bottom: 1px solid #e0e0e0;"
        "}"
        "QTableWidget::item:selected {"
        "    background-color: #e3f2fd;"
        "}"
    );
    
    bulletTableLayout->addWidget(m_bulletTable);
    mainLayout->addWidget(bulletTableGroup);
}

void MainWindow::setupItemManager()
{
    m_itemTab = new QWidget;
    m_tabWidget->addTab(m_itemTab, "道具管理");
    
    QHBoxLayout *itemLayout = new QHBoxLayout(m_itemTab);
    
    // 道具按钮数据结构
    struct ItemButtonInfo {
        QString name;
        ItemManager::ItemType type;
        QString color;
    };
    
    // 玩家道具
    m_playerItemsGroup = new QGroupBox("玩家道具");
    QVBoxLayout *playerLayout = new QVBoxLayout(m_playerItemsGroup);
    
    // 玩家道具按钮区域
    QLabel *playerAddLabel = new QLabel("添加道具:");
    playerLayout->addWidget(playerAddLabel);
    
    // 创建网格布局来排列道具按钮
    QGridLayout *playerButtonsLayout = new QGridLayout;
    
    QList<ItemButtonInfo> items = {
        {"🔍 放大镜", ItemManager::ItemType::MagnifyingGlass, "#6C757D"},
        {"🚬 香烟", ItemManager::ItemType::Cigarettes, "#6C757D"},
        {"🍺 啤酒", ItemManager::ItemType::Beer, "#6C757D"},
        {"🪚 手锯", ItemManager::ItemType::Handsaw, "#6C757D"},
        {"⛓️ 手铐", ItemManager::ItemType::Handcuffs, "#6C757D"},
        {"📞 一次性电话", ItemManager::ItemType::BurnerPhone, "#6C757D"},
        {"🔄 逆变器", ItemManager::ItemType::Inverter, "#6C757D"},
        {"💉 肾上腺素", ItemManager::ItemType::Adrenaline, "#6C757D"},
        {"💊 过期药物", ItemManager::ItemType::ExpiredMedicine, "#6C757D"}
    };
    
    // 创建玩家道具按钮（3列布局）
    for (int i = 0; i < items.size(); ++i) {
        QPushButton *button = new QPushButton(items[i].name);
        button->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 8px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: %2;"
            "}"
            "QPushButton:pressed {"
            "    background-color: %3;"
            "}"
        ).arg(items[i].color)
         .arg(items[i].color + "CC")  // 悬停时稍微透明
         .arg(items[i].color + "AA")); // 按下时更透明
        
        // 连接信号
        connect(button, &QPushButton::clicked, [this, itemType = items[i].type]() {
            m_itemManager->addPlayerItem(itemType);
            updateDisplay();
        });
        
        int row = i / 3;
        int col = i % 3;
        playerButtonsLayout->addWidget(button, row, col);
    }
    
    playerLayout->addLayout(playerButtonsLayout);
    
    // 玩家道具列表
    QLabel *playerItemsLabel = new QLabel("已拥有道具:");
    playerLayout->addWidget(playerItemsLabel);
    
    m_playerItemsList = new QListWidget;
    m_playerItemsList->setMaximumHeight(150);
    m_playerItemsList->setAlternatingRowColors(true);
    playerLayout->addWidget(m_playerItemsList);
    
    playerLayout->addStretch();
    
    // 庄家道具
    m_dealerItemsGroup = new QGroupBox("庄家道具");
    QVBoxLayout *dealerLayout = new QVBoxLayout(m_dealerItemsGroup);
    
    // 庄家道具按钮区域
    QLabel *dealerAddLabel = new QLabel("添加道具:");
    dealerLayout->addWidget(dealerAddLabel);
    
    // 创建网格布局来排列庄家道具按钮
    QGridLayout *dealerButtonsLayout = new QGridLayout;
    
    // 创建庄家道具按钮（3列布局，使用统一颜色）
    QList<ItemButtonInfo> dealerItems = {
        {"🔍 放大镜", ItemManager::ItemType::MagnifyingGlass, "#6C757D"},
        {"🚬 香烟", ItemManager::ItemType::Cigarettes, "#6C757D"},
        {"🍺 啤酒", ItemManager::ItemType::Beer, "#6C757D"},
        {"🪚 手锯", ItemManager::ItemType::Handsaw, "#6C757D"},
        {"⛓️ 手铐", ItemManager::ItemType::Handcuffs, "#6C757D"},
        {"📞 一次性电话", ItemManager::ItemType::BurnerPhone, "#6C757D"},
        {"🔄 逆变器", ItemManager::ItemType::Inverter, "#6C757D"},
        {"💉 肾上腺素", ItemManager::ItemType::Adrenaline, "#6C757D"},
        {"💊 过期药物", ItemManager::ItemType::ExpiredMedicine, "#6C757D"}
    };
    
    for (int i = 0; i < dealerItems.size(); ++i) {
        QPushButton *button = new QPushButton(dealerItems[i].name);
        button->setStyleSheet(QString(
            "QPushButton {"
            "    background-color: %1;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 4px;"
            "    padding: 8px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "    background-color: %2;"
            "}"
            "QPushButton:pressed {"
            "    background-color: %3;"
            "}"
        ).arg(dealerItems[i].color)
         .arg(dealerItems[i].color + "CC")
         .arg(dealerItems[i].color + "AA"));
        
        // 连接信号
        connect(button, &QPushButton::clicked, [this, itemType = dealerItems[i].type]() {
            m_itemManager->addDealerItem(itemType);
            updateDisplay();
        });
        
        int row = i / 3;
        int col = i % 3;
        dealerButtonsLayout->addWidget(button, row, col);
    }
    
    dealerLayout->addLayout(dealerButtonsLayout);
    
    // 庄家道具列表
    QLabel *dealerItemsLabel = new QLabel("庄家道具:");
    dealerLayout->addWidget(dealerItemsLabel);
    
    m_dealerItemsList = new QListWidget;
    m_dealerItemsList->setMaximumHeight(150);
    m_dealerItemsList->setAlternatingRowColors(true);
    dealerLayout->addWidget(m_dealerItemsList);
    
    dealerLayout->addStretch();
    
    itemLayout->addWidget(m_playerItemsGroup);
    itemLayout->addWidget(m_dealerItemsGroup);
}

void MainWindow::setupDecisionHelper()
{
    m_adviceTab = new QWidget;
    m_tabWidget->addTab(m_adviceTab, "决策建议");
    QVBoxLayout *adviceLayout = new QVBoxLayout(m_adviceTab);
    m_getAdviceButton = new QPushButton("获取AI建议");
    m_getAdviceButton->setStyleSheet("QPushButton { background-color: #51cf66; color: white; font-weight: bold; padding: 10px; }");
    connect(m_getAdviceButton, &QPushButton::clicked, this, &MainWindow::onGetDecisionAdvice);
    adviceLayout->addWidget(m_getAdviceButton);
    m_adviceTextEdit = new QTextEdit;
    m_adviceTextEdit->setPlainText("点击上方按钮获取基于当前游戏状态的最优决策建议。\n\n建议将包括：\n- 当前局势分析\n- 概率计算\n- 推荐行动\n- 道具使用建议");
    adviceLayout->addWidget(m_adviceTextEdit);
}

void MainWindow::onNewRound()
{
    int live = m_liveBulletsSpinBox->value();
    int blank = m_blankBulletsSpinBox->value();
    
    m_bulletTracker->startNewRound(live, blank);
    m_itemManager->clearAllItems();
    
    updateDisplay();
    
    QMessageBox::information(this, "新回合", 
                           QString("新回合开始！\n实弹: %1发\n空包弹: %2发").arg(live).arg(blank));
}

void MainWindow::onFireBullet()
{
    // 这个方法通过按钮回调处理
}

void MainWindow::onAddKnownBullet()
{
    // 这个方法通过按钮回调处理
}

void MainWindow::onResetGame()
{
    m_bulletTracker->reset();
    m_itemManager->clearAllItems();
    updateDisplay();
    
    QMessageBox::information(this, "重置", "游戏已重置！");
}

void MainWindow::onCalculateProbability()
{
    updateProbability();
}

void MainWindow::onGetDecisionAdvice()
{
    DecisionHelper::GameState state;
    state.remainingLive = m_bulletTracker->getRemainingLive();
    state.remainingBlank = m_bulletTracker->getRemainingBlank();
    state.currentPosition = m_bulletTracker->getCurrentPosition();
    state.knownBullets = m_bulletTracker->getKnownBullets();
    state.playerItems = m_itemManager->getPlayerItems();
    state.dealerItems = m_itemManager->getDealerItems();
    state.playerHealth = 3; // 默认值，后续可以添加UI控制
    state.dealerHealth = 3;
    state.isPlayerTurn = true;
    state.handsawActive = false;
    
    QString advice = m_decisionHelper->getAdvice(state);
    m_adviceTextEdit->setPlainText(advice);
}

void MainWindow::updateDisplay()
{
    // 更新子弹状态标签
    m_remainingLiveLabel->setText(QString::number(m_bulletTracker->getRemainingLive()));
    m_remainingBlankLabel->setText(QString::number(m_bulletTracker->getRemainingBlank()));
    
    // 更新子弹追踪表格
    int totalBullets = m_bulletTracker->getRemainingLive() + m_bulletTracker->getRemainingBlank() + 
                       m_bulletTracker->getBulletHistory().size();
    
    if (totalBullets > 0) {
        // 清空表格并重新设置
        m_bulletTable->clear();
        m_bulletTable->setRowCount(totalBullets);
        m_bulletTable->setHorizontalHeaderLabels({"位置", "实际类型", "道具已知", "状态"});
        
        const auto& history = m_bulletTracker->getBulletHistory();
        const auto& known = m_bulletTracker->getKnownBullets();
        int currentPos = m_bulletTracker->getCurrentPosition();
        
        qDebug() << "updateDisplay: totalBullets=" << totalBullets 
                 << "currentPos=" << currentPos 
                 << "historySize=" << history.size();
        
        for (int i = 0; i < totalBullets; ++i) {
            int position = i + 1;
            
            // 设置统一的行高
            m_bulletTable->setRowHeight(i, 35);
            
            // 位置列
            QTableWidgetItem* posItem = new QTableWidgetItem(QString::number(position));
            posItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            posItem->setTextAlignment(Qt::AlignCenter);
            m_bulletTable->setItem(i, 0, posItem);
            
            // 查找是否已发射
            bool isFired = false;
            bool actualType = false; // true=实弹, false=空包弹
            for (const auto& bullet : history) {
                if (bullet.position == position) {
                    isFired = true;
                    actualType = bullet.isLive;
                    break;
                }
            }
            
            // 实际类型列
            if (isFired) {
                // 已发射的子弹显示实际类型
                QTableWidgetItem* typeItem = new QTableWidgetItem(actualType ? "实弹" : "空包弹");
                typeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                typeItem->setTextAlignment(Qt::AlignCenter);
                typeItem->setForeground(actualType ? QColor(Qt::red) : QColor(Qt::blue));
                typeItem->setBackground(QColor(240, 240, 240)); // 灰色背景表示已发射
                m_bulletTable->setItem(i, 1, typeItem);
            } else if (position == currentPos) {
                // 当前位置可以设置类型
                BulletTypeWidget* typeWidget = new BulletTypeWidget;
                
                // 连接信号，当选择改变时记录开火
                connect(typeWidget, &BulletTypeWidget::typeChanged, 
                        [this, position](int newType) {
                    if (newType >= 0) { // 选择了实弹或空包弹
                        bool isLive = (newType == 1);
                        m_bulletTracker->fireBullet(isLive);
                        
                        // 使用QTimer::singleShot延迟更新，避免在信号处理中直接更新
                        QTimer::singleShot(0, this, &MainWindow::updateDisplay);
                    }
                });
                
                m_bulletTable->setCellWidget(i, 1, typeWidget);
            } else {
                // 其他位置显示为未知
                QTableWidgetItem* typeItem = new QTableWidgetItem("未知");
                typeItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                typeItem->setTextAlignment(Qt::AlignCenter);
                typeItem->setForeground(QColor(Qt::gray));
                m_bulletTable->setItem(i, 1, typeItem);
            }
            
            // 道具已知列
            bool isKnown = false;
            bool knownType = false;
            for (const auto& bullet : known) {
                if (bullet.position == position && !bullet.isFired) {
                    isKnown = true;
                    knownType = bullet.isLive;
                    break;
                }
            }
            
            if (isFired) {
                // 已发射的显示为空
                QTableWidgetItem* knownItem = new QTableWidgetItem("");
                knownItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
                knownItem->setBackground(QColor(240, 240, 240));
                m_bulletTable->setItem(i, 2, knownItem);
            } else {
                // 未发射的可以设置道具已知信息
                BulletTypeWidget* knownWidget = new BulletTypeWidget;
                
                if (isKnown) {
                    knownWidget->setCurrentType(knownType ? 1 : 0);
                } else {
                    knownWidget->setCurrentType(-1);
                }
                
                // 连接信号，当选择改变时更新已知信息
                connect(knownWidget, &BulletTypeWidget::typeChanged, 
                        [this, position](int newType) {
                    if (newType == -1) { // 选择未知，移除已知信息
                        m_bulletTracker->removeKnownBullet(position);
                    } else { // 选择实弹或空包弹
                        bool isLive = (newType == 1);
                        m_bulletTracker->addKnownBullet(position, isLive);
                    }
                    
                    // 使用QTimer::singleShot延迟更新，避免在信号处理中直接更新
                    QTimer::singleShot(0, this, &MainWindow::updateDisplay);
                });
                
                m_bulletTable->setCellWidget(i, 2, knownWidget);
            }
            
            // 状态列
            QString status;
            QColor statusColor = Qt::black;
            if (isFired) {
                status = "已发射";
                statusColor = Qt::gray;
            } else if (position == currentPos) {
                status = "当前子弹";
                statusColor = Qt::red;
            } else if (isKnown) {
                status = "道具已知";
                statusColor = Qt::blue;
            } else {
                status = "未知";
                statusColor = Qt::gray;
            }
            
            QTableWidgetItem* statusItem = new QTableWidgetItem(status);
            statusItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            statusItem->setTextAlignment(Qt::AlignCenter);
            statusItem->setForeground(statusColor);
            if (isFired) {
                statusItem->setBackground(QColor(240, 240, 240));
            }
            m_bulletTable->setItem(i, 3, statusItem);
        }
    } else {
        m_bulletTable->setRowCount(0);
    }
    
    // 启用/禁用按钮
    bool hasRound = (m_bulletTracker->getRemainingLive() + m_bulletTracker->getRemainingBlank()) > 0;
    m_getAdviceButton->setEnabled(hasRound);
    
    // 更新道具列表
    updateItemLists();
}

void MainWindow::updateProbability()
{
    double prob = m_bulletTracker->getLiveProbability();
    int percentage = static_cast<int>(prob * 100);
    
    m_probabilityLabel->setText(QString("%1%").arg(percentage));
    m_probabilityBar->setValue(percentage);
    
    // 根据概率调整颜色
    QString color;
    if (prob >= 0.7) {
        color = "red";
    } else if (prob >= 0.4) {
        color = "orange";
    } else {
        color = "green";
    }
    
    m_probabilityBar->setStyleSheet(QString("QProgressBar::chunk { background-color: %1; }").arg(color));
}

void MainWindow::updateItemLists()
{
    // 更新玩家道具列表
    m_playerItemsList->clear();
    const auto& playerItems = m_itemManager->getPlayerItems();
    for (int i = 0; i < playerItems.size(); ++i) {
        const auto& item = playerItems[i];
        
        // 创建自定义widget包含道具名称和删除按钮
        QWidget* itemWidget = new QWidget;
        QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 2, 5, 2);
        
        // 道具名称标签
        QLabel* nameLabel = new QLabel;
        QString itemText = item.name;
        if (item.isUsed) {
            itemText += " (已使用)";
            nameLabel->setStyleSheet("color: gray; text-decoration: line-through;");
        } else {
            nameLabel->setStyleSheet("color: darkgreen; font-weight: bold;");
        }
        nameLabel->setText(itemText);
        nameLabel->setToolTip(item.description);
        
        // 删除按钮
        QPushButton* deleteBtn = new QPushButton("❌");
        deleteBtn->setMaximumSize(25, 25);
        deleteBtn->setStyleSheet(
            "QPushButton {"
            // "    background-color: #DC3545;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 3px;"
            "    font-size: 12px;"
            "}"
            "QPushButton:hover {"
            // "    background-color: #C82333;"
            "}"
        );
        deleteBtn->setToolTip("删除此道具");
        
        // 连接删除信号
        connect(deleteBtn, &QPushButton::clicked, [this, i]() {
            m_itemManager->removePlayerItem(i);
            updateDisplay();
        });
        
        itemLayout->addWidget(nameLabel);
        itemLayout->addStretch();
        itemLayout->addWidget(deleteBtn);
        
        // 添加到列表
        QListWidgetItem* listItem = new QListWidgetItem;
        listItem->setSizeHint(itemWidget->sizeHint());
        m_playerItemsList->addItem(listItem);
        m_playerItemsList->setItemWidget(listItem, itemWidget);
    }
    
    // 更新庄家道具列表
    m_dealerItemsList->clear();
    const auto& dealerItems = m_itemManager->getDealerItems();
    for (int i = 0; i < dealerItems.size(); ++i) {
        const auto& item = dealerItems[i];
        
        // 创建自定义widget包含道具名称和删除按钮
        QWidget* itemWidget = new QWidget;
        QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);
        itemLayout->setContentsMargins(5, 2, 5, 2);
        
        // 道具名称标签
        QLabel* nameLabel = new QLabel;
        QString itemText = item.name;
        if (item.isUsed) {
            itemText += " (已使用)";
            nameLabel->setStyleSheet("color: gray; text-decoration: line-through;");
        } else {
            nameLabel->setStyleSheet("color: darkred; font-weight: bold;");
        }
        nameLabel->setText(itemText);
        nameLabel->setToolTip(item.description);
        
        // 删除按钮
        QPushButton* deleteBtn = new QPushButton("❌");
        deleteBtn->setMaximumSize(25, 25);
        deleteBtn->setStyleSheet(
            "QPushButton {"
            // "    background-color: #DC3545;"
            "    color: white;"
            "    border: none;"
            "    border-radius: 3px;"
            "    font-size: 12px;"
            "}"
            "QPushButton:hover {"
            // "    background-color: #C82333;"
            "}"
        );
        deleteBtn->setToolTip("删除此道具");
        
        // 连接删除信号
        connect(deleteBtn, &QPushButton::clicked, [this, i]() {
            m_itemManager->removeDealerItem(i);
            updateDisplay();
        });
        
        itemLayout->addWidget(nameLabel);
        itemLayout->addStretch();
        itemLayout->addWidget(deleteBtn);
        
        // 添加到列表
        QListWidgetItem* listItem = new QListWidgetItem;
        listItem->setSizeHint(itemWidget->sizeHint());
        m_dealerItemsList->addItem(listItem);
        m_dealerItemsList->setItemWidget(listItem, itemWidget);
    }
}
