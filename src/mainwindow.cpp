#include "main.h"
#include "version.h"
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
        "    padding: 8px;"
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
    
    // 玩家道具
    m_playerItemsGroup = new QGroupBox("玩家道具");
    QVBoxLayout *playerLayout = new QVBoxLayout(m_playerItemsGroup);
    
    // 添加道具按钮
    QComboBox *playerItemCombo = new QComboBox;
    playerItemCombo->addItem("放大镜 - 查看当前子弹", static_cast<int>(ItemManager::ItemType::MagnifyingGlass));
    playerItemCombo->addItem("香烟 - 恢复1生命", static_cast<int>(ItemManager::ItemType::Cigarettes));
    playerItemCombo->addItem("啤酒 - 弹出当前子弹", static_cast<int>(ItemManager::ItemType::Beer));
    playerItemCombo->addItem("手锯 - 双倍伤害", static_cast<int>(ItemManager::ItemType::Handsaw));
    playerItemCombo->addItem("手铐 - 跳过对手回合", static_cast<int>(ItemManager::ItemType::Handcuffs));
    playerItemCombo->addItem("一次性电话 - 查看随机子弹", static_cast<int>(ItemManager::ItemType::BurnerPhone));
    playerItemCombo->addItem("逆变器 - 改变当前子弹类型", static_cast<int>(ItemManager::ItemType::Inverter));
    playerItemCombo->addItem("肾上腺素 - 偷取道具", static_cast<int>(ItemManager::ItemType::Adrenaline));
    playerItemCombo->addItem("过期药物 - 50%概率效果", static_cast<int>(ItemManager::ItemType::ExpiredMedicine));
    
    QPushButton *addPlayerItemBtn = new QPushButton("添加玩家道具");
    connect(addPlayerItemBtn, &QPushButton::clicked, [this, playerItemCombo]() {
        auto itemType = static_cast<ItemManager::ItemType>(playerItemCombo->currentData().toInt());
        m_itemManager->addPlayerItem(itemType);
        updateDisplay();
    });
    
    playerLayout->addWidget(playerItemCombo);
    playerLayout->addWidget(addPlayerItemBtn);
    playerLayout->addStretch();
    
    // 庄家道具
    m_dealerItemsGroup = new QGroupBox("庄家道具");
    QVBoxLayout *dealerLayout = new QVBoxLayout(m_dealerItemsGroup);
    
    QComboBox *dealerItemCombo = new QComboBox;
    dealerItemCombo->addItem("放大镜 - 查看当前子弹", static_cast<int>(ItemManager::ItemType::MagnifyingGlass));
    dealerItemCombo->addItem("香烟 - 恢复1生命", static_cast<int>(ItemManager::ItemType::Cigarettes));
    dealerItemCombo->addItem("啤酒 - 弹出当前子弹", static_cast<int>(ItemManager::ItemType::Beer));
    dealerItemCombo->addItem("手锯 - 双倍伤害", static_cast<int>(ItemManager::ItemType::Handsaw));
    dealerItemCombo->addItem("手铐 - 跳过对手回合", static_cast<int>(ItemManager::ItemType::Handcuffs));
    dealerItemCombo->addItem("一次性电话 - 查看随机子弹", static_cast<int>(ItemManager::ItemType::BurnerPhone));
    dealerItemCombo->addItem("逆变器 - 改变当前子弹类型", static_cast<int>(ItemManager::ItemType::Inverter));
    dealerItemCombo->addItem("肾上腺素 - 偷取道具", static_cast<int>(ItemManager::ItemType::Adrenaline));
    dealerItemCombo->addItem("过期药物 - 50%概率效果", static_cast<int>(ItemManager::ItemType::ExpiredMedicine));
    
    QPushButton *addDealerItemBtn = new QPushButton("添加庄家道具");
    connect(addDealerItemBtn, &QPushButton::clicked, [this, dealerItemCombo]() {
        auto itemType = static_cast<ItemManager::ItemType>(dealerItemCombo->currentData().toInt());
        m_itemManager->addDealerItem(itemType);
        updateDisplay();
    });
    
    dealerLayout->addWidget(dealerItemCombo);
    dealerLayout->addWidget(addDealerItemBtn);
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
    
    // 添加已知信息设置
    m_knownTab = new QWidget;
    m_tabWidget->addTab(m_knownTab, "已知信息");
    
    QVBoxLayout *knownLayout = new QVBoxLayout(m_knownTab);
    
    // 已知子弹表格
    m_knownBulletsTable = new QTableWidget(0, 3);
    m_knownBulletsTable->setHorizontalHeaderLabels({"位置", "类型", "操作"});
    m_knownBulletsTable->horizontalHeader()->setStretchLastSection(true);
    knownLayout->addWidget(m_knownBulletsTable);
    
    // 添加已知信息按钮
    QHBoxLayout *knownButtonLayout = new QHBoxLayout;
    QSpinBox *positionSpinBox = new QSpinBox;
    positionSpinBox->setRange(1, 8);
    QComboBox *typeCombo = new QComboBox;
    typeCombo->addItem("实弹", true);
    typeCombo->addItem("空包弹", false);
    
    m_addKnownButton = new QPushButton("添加已知信息");
    connect(m_addKnownButton, &QPushButton::clicked, [this, positionSpinBox, typeCombo]() {
        int position = positionSpinBox->value();
        bool isLive = typeCombo->currentData().toBool();
        m_bulletTracker->addKnownBullet(position, isLive);
        updateDisplay();
    });
    
    knownButtonLayout->addWidget(new QLabel("位置:"));
    knownButtonLayout->addWidget(positionSpinBox);
    knownButtonLayout->addWidget(new QLabel("类型:"));
    knownButtonLayout->addWidget(typeCombo);
    knownButtonLayout->addWidget(m_addKnownButton);
    knownButtonLayout->addStretch();
    
    knownLayout->addLayout(knownButtonLayout);
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
                QComboBox* typeCombo = new QComboBox;
                typeCombo->addItem("未知", -1);
                typeCombo->addItem("实弹", 1);
                typeCombo->addItem("空包弹", 0);
                typeCombo->setCurrentIndex(0); // 默认选择"未知"
                
                // 设置QComboBox的样式和尺寸
                // typeCombo->setMinimumHeight(30);
                // typeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                // typeCombo->setStyleSheet(
                //     "QComboBox {"
                //     "    padding: 5px;"
                //     "    border: 1px solid #cccccc;"
                //     "    border-radius: 3px;"
                //     "    background-color: white;"
                //     "    min-height: 20px;"
                //     "}"
                //     "QComboBox::drop-down {"
                //     "    subcontrol-origin: padding;"
                //     "    subcontrol-position: top right;"
                //     "    width: 20px;"
                //     "    border-left: 1px solid #cccccc;"
                //     "}"
                //     "QComboBox::down-arrow {"
                //     "    image: none;"
                //     "    border: 2px solid #666666;"
                //     "    width: 6px;"
                //     "    height: 6px;"
                //     "    border-top-color: transparent;"
                //     "    border-left-color: transparent;"
                //     "    border-right-color: #666666;"
                //     "    border-bottom-color: #666666;"
                //     "}"
                // );
                
                // 连接信号，当选择改变时记录开火
                connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                        [this, position, typeCombo](int index) {
                    if (index > 0) { // 选择了实弹或空包弹
                        QVariant data = typeCombo->currentData();
                        if (!data.isValid()) {
                            return; // 安全检查：如果数据无效，直接返回
                        }
                        
                        bool isLive = data.toBool();
                        
                        // 断开信号连接，防止重复触发
                        typeCombo->blockSignals(true);
                        
                        m_bulletTracker->fireBullet(isLive);
                        
                        // 使用QTimer::singleShot延迟更新，避免在信号处理中直接更新
                        QTimer::singleShot(0, this, &MainWindow::updateDisplay);
                    }
                });
                
                m_bulletTable->setCellWidget(i, 1, typeCombo);
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
                QComboBox* knownCombo = new QComboBox;
                knownCombo->addItem("未知", -1);
                knownCombo->addItem("实弹", 1);
                knownCombo->addItem("空包弹", 0);
                
                // 设置QComboBox的样式和尺寸
                // knownCombo->setMinimumHeight(30);
                // knownCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
                // knownCombo->setStyleSheet(
                //     "QComboBox {"
                //     "    padding: 5px;"
                //     "    border: 1px solid #cccccc;"
                //     "    border-radius: 3px;"
                //     "    background-color: white;"
                //     "    min-height: 20px;"
                //     "}"
                //     "QComboBox::drop-down {"
                //     "    subcontrol-origin: padding;"
                //     "    subcontrol-position: top right;"
                //     "    width: 20px;"
                //     "    border-left: 1px solid #cccccc;"
                //     "}"
                //     "QComboBox::down-arrow {"
                //     "    image: none;"
                //     "    border: 2px solid #666666;"
                //     "    width: 6px;"
                //     "    height: 6px;"
                //     "    border-top-color: transparent;"
                //     "    border-left-color: transparent;"
                //     "    border-right-color: #666666;"
                //     "    border-bottom-color: #666666;"
                //     "}"
                // );
                
                if (isKnown) {
                    knownCombo->setCurrentIndex(knownType ? 1 : 2);
                } else {
                    knownCombo->setCurrentIndex(0);
                }
                
                // 连接信号，当选择改变时更新已知信息
                connect(knownCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                        [this, position, knownCombo](int index) {
                    // 断开信号连接，防止重复触发
                    knownCombo->blockSignals(true);
                    
                    if (index == 0) { // 选择未知，移除已知信息
                        m_bulletTracker->removeKnownBullet(position);
                    } else { // 选择实弹或空包弹
                        QVariant data = knownCombo->currentData();
                        if (!data.isValid()) {
                            return; // 安全检查：如果数据无效，直接返回
                        }
                        
                        bool isLive = data.toBool();
                        m_bulletTracker->addKnownBullet(position, isLive);
                    }
                    
                    // 使用QTimer::singleShot延迟更新，避免在信号处理中直接更新
                    QTimer::singleShot(0, this, &MainWindow::updateDisplay);
                });
                
                m_bulletTable->setCellWidget(i, 2, knownCombo);
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
    
    // 更新已知信息表格
    m_knownBulletsTable->setRowCount(0);
    const auto& known = m_bulletTracker->getKnownBullets();
    for (const auto& bullet : known) {
        if (!bullet.isFired) {
            int row = m_knownBulletsTable->rowCount();
            m_knownBulletsTable->insertRow(row);
            m_knownBulletsTable->setItem(row, 0, new QTableWidgetItem(QString::number(bullet.position)));
            m_knownBulletsTable->setItem(row, 1, new QTableWidgetItem(bullet.isLive ? "实弹" : "空包弹"));
            QPushButton* removeBtn = new QPushButton("删除");
            m_knownBulletsTable->setCellWidget(row, 2, removeBtn);
        }
    }
    
    // 启用/禁用按钮
    bool hasRound = (m_bulletTracker->getRemainingLive() + m_bulletTracker->getRemainingBlank()) > 0;
    m_addKnownButton->setEnabled(hasRound);
    m_getAdviceButton->setEnabled(hasRound);
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
