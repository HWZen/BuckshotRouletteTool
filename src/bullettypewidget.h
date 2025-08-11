#pragma once

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>

class BulletTypeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BulletTypeWidget(QWidget *parent = nullptr);
    
    // 设置当前选择的类型 (-1: 未知, 0: 空包弹, 1: 实弹)
    void setCurrentType(int type);
    
    // 获取当前选择的类型
    int getCurrentType() const;
    
    // 重置为未知状态
    void reset();

signals:
    // 当选择改变时发出信号
    void typeChanged(int newType);

private slots:
    void onButtonToggled();

private:
    void setupUI();
    
    QHBoxLayout *m_layout;
    QLabel *m_label;
    QRadioButton *m_liveButton;
    QRadioButton *m_blankButton;
    QButtonGroup *m_buttonGroup;
    
    int m_currentType; // -1: 未知, 0: 空包弹, 1: 实弹
};
