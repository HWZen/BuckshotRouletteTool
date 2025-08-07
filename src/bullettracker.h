#pragma once

#include <QObject>
#include <QList>

class BulletTracker : public QObject {
    Q_OBJECT

public:
    struct BulletInfo {
        int position;
        bool isLive;  // true = 实弹, false = 空包弹
        bool isKnown; // 是否已知
        bool isFired; // 是否已发射
    };

    explicit BulletTracker(QObject *parent = nullptr);
    
    void startNewRound(int liveBullets, int blankBullets);
    void fireBullet(bool isLive);
    void addKnownBullet(int position, bool isLive);
    void removeKnownBullet(int position);
    void reset();
    
    int getRemainingLive() const;
    int getRemainingBlank() const;
    int getCurrentPosition() const;
    double getLiveProbability() const;
    
    const QList<BulletInfo>& getBulletHistory() const;
    const QList<BulletInfo>& getKnownBullets() const;

signals:
    void roundStarted(int liveBullets, int blankBullets);
    void bulletFired(int position, bool isLive);
    void probabilityChanged(double probability);

private:
    void calculateProbability();
    
    int m_totalLive;
    int m_totalBlank;
    int m_remainingLive;
    int m_remainingBlank;
    int m_currentPosition;
    double m_liveProbability;
    
    QList<BulletInfo> m_bulletHistory;
    QList<BulletInfo> m_knownBullets;
};
