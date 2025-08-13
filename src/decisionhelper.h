#pragma once

#include <QObject>
#include <QString>
#include "bullettracker.h"
#include "itemmanager.h"
#include "aiclient.h"

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
        int playerMaxHealth;
        int dealerHealth;
        int dealerMaxHealth;
        bool isPlayerTurn;
        bool handsawActive; // 手锯是否激活
    };

    explicit DecisionHelper(QObject *parent = nullptr);
    
    // 传统本地决策（保留）
    QString getAdvice(const GameState &state);
    double calculateExpectedValue(const GameState &state, bool shootDealer);
    
    // AI决策
    void getAIAdvice(const GameState &state, const QString &apiUrl, const QString &apiKey, const QString &model = QString(), const QString &customPrompt = QString());
    void cancelAIRequest();

signals:
    void aiAdviceReceived(const QString &advice);
    void aiRequestStarted();
    void aiRequestFinished();
    void aiError(const QString &error);

private slots:
    void onAIResponse(const QString &response);
    void onAIError(const QString &error);
    void onAIRequestStarted();
    void onAIRequestFinished();

private:
    QString analyzeCurrentSituation(const GameState &state);
    QString recommendAction(const GameState &state);
    QString analyzeItems(const GameState &state);
    
    // AI相关方法
    QString buildGameInfoPrompt(const GameState &state);
    QString buildSystemPrompt();
    QString buildUserPrompt(const GameState &state, const QString &customPrompt);
    
    AIClient *m_aiClient;
};
