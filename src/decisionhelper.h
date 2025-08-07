#pragma once

#include <QObject>
#include <QString>
#include "bullettracker.h"
#include "itemmanager.h"

class DecisionHelper : public QObject {
    Q_OBJECT

public:
    struct GameState {
        int remainingLive;
        int remainingBlank;
        int currentPosition;
        QList<BulletTracker::BulletInfo> knownBullets;
        QList<ItemManager::ItemInfo> playerItems;
        QList<ItemManager::ItemInfo> dealerItems;
        int playerHealth;
        int dealerHealth;
        bool isPlayerTurn;
        bool handsawActive; // 手锯是否激活
    };

    explicit DecisionHelper(QObject *parent = nullptr);
    
    QString getAdvice(const GameState &state);
    double calculateExpectedValue(const GameState &state, bool shootDealer);

private:
    QString analyzeCurrentSituation(const GameState &state);
    QString recommendAction(const GameState &state);
    QString analyzeItems(const GameState &state);
};
