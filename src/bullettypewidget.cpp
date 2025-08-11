#include "bullettypewidget.h"

BulletTypeWidget::BulletTypeWidget(QWidget *parent)
    : QWidget(parent)
    , m_layout(nullptr)
    , m_label(nullptr)
    , m_liveButton(nullptr)
    , m_blankButton(nullptr)
    , m_buttonGroup(nullptr)
    , m_currentType(-1)
{
    setupUI();
}

void BulletTypeWidget::setupUI()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setContentsMargins(2, 2, 2, 2);
    m_layout->setSpacing(8);
    
    // 左侧标签
    m_label = new QLabel("未知");
    // m_label->setMinimumWidth(60);
    // m_label->setAlignment(Qt::AlignCenter);
    m_label->setStyleSheet(
        "QLabel {"
        "    border: 1px solid #cccccc;"
        "    border-radius: 3px;"
        "    padding: 4px;"
        "    background-color: #f5f5f5;"
        "    color: #666666;"
        "}"
    );
    m_layout->addWidget(m_label);
    
    // 右侧单选框
    m_liveButton = new QRadioButton("实弹");
    m_liveButton->setStyleSheet("QRadioButton { color: red; font-weight: bold; }");
    
    m_blankButton = new QRadioButton("空包弹");
    m_blankButton->setStyleSheet("QRadioButton { color: blue; font-weight: bold; }");
    
    // 按钮组
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->addButton(m_liveButton, 1);  // 实弹 = 1
    m_buttonGroup->addButton(m_blankButton, 0); // 空包弹 = 0
    m_buttonGroup->setExclusive(true);
    
    // 连接信号
    connect(m_liveButton, &QRadioButton::toggled, this, &BulletTypeWidget::onButtonToggled);
    connect(m_blankButton, &QRadioButton::toggled, this, &BulletTypeWidget::onButtonToggled);
    
    m_layout->addWidget(m_liveButton);
    m_layout->addWidget(m_blankButton);
    m_layout->addStretch();
}

void BulletTypeWidget::setCurrentType(int type)
{
    if (m_currentType == type) {
        return; // 没有变化，直接返回
    }
    
    m_currentType = type;
    
    // 阻止信号发送，避免循环
    m_liveButton->blockSignals(true);
    m_blankButton->blockSignals(true);
    
    switch (type) {
        case 1: // 实弹
            m_liveButton->setChecked(true);
            m_label->setText("实弹");
            m_label->setStyleSheet(
                "QLabel {"
                "    border: 1px solid #ff0000;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: #ffe6e6;"
                "    color: #cc0000;"
                "    font-weight: bold;"
                "}"
            );
            break;
        case 0: // 空包弹
            m_blankButton->setChecked(true);
            m_label->setText("空包弹");
            m_label->setStyleSheet(
                "QLabel {"
                "    border: 1px solid #0000ff;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: #e6e6ff;"
                "    color: #0000cc;"
                "    font-weight: bold;"
                "}"
            );
            break;
        default: // 未知
            m_liveButton->setChecked(false);
            m_blankButton->setChecked(false);
            m_label->setText("未知");
            m_label->setStyleSheet(
                "QLabel {"
                "    border: 1px solid #cccccc;"
                "    border-radius: 3px;"
                "    padding: 4px;"
                "    background-color: #f5f5f5;"
                "    color: #666666;"
                "}"
            );
            break;
    }
    
    // 恢复信号
    m_liveButton->blockSignals(false);
    m_blankButton->blockSignals(false);
}

int BulletTypeWidget::getCurrentType() const
{
    return m_currentType;
}

void BulletTypeWidget::reset()
{
    setCurrentType(-1);
}

void BulletTypeWidget::onButtonToggled()
{
    int newType = -1;
    
    if (m_liveButton->isChecked()) {
        newType = 1;
    } else if (m_blankButton->isChecked()) {
        newType = 0;
    }
    
    if (newType != m_currentType) {
        setCurrentType(newType);
        emit typeChanged(newType);
    }
}
