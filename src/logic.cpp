#include "bullettracker.h"
#include "itemmanager.h"
#include "decisionhelper.h"
#include <QDebug>

// BulletTracker实现
BulletTracker::BulletTracker(QObject *parent)
    : QObject(parent)
    , m_totalLive(0)
    , m_totalBlank(0)
    , m_remainingLive(0)
    , m_remainingBlank(0)
    , m_currentPosition(0)
    , m_liveProbability(0.0)
{
}

void BulletTracker::startNewRound(int liveBullets, int blankBullets)
{
    m_totalLive = liveBullets;
    m_totalBlank = blankBullets;
    m_remainingLive = liveBullets;
    m_remainingBlank = blankBullets;
    m_currentPosition = 1;
    
    m_bulletHistory.clear();
    m_knownBullets.clear();
    
    calculateProbability();
    emit roundStarted(liveBullets, blankBullets);
}

void BulletTracker::fireBullet(bool isLive)
{
    if (m_remainingLive + m_remainingBlank <= 0) {
        return; // 没有剩余子弹
    }
    
    BulletInfo bullet;
    bullet.position = m_currentPosition;
    bullet.isLive = isLive;
    bullet.isKnown = false;
    bullet.isFired = true;
    
    m_bulletHistory.append(bullet);
    
    // 更新剩余数量
    if (isLive) {
        m_remainingLive = qMax(0, m_remainingLive - 1);
    } else {
        m_remainingBlank = qMax(0, m_remainingBlank - 1);
    }
    
    // 更新已知信息中对应位置的状态
    for (auto &known : m_knownBullets) {
        if (known.position == m_currentPosition) {
            known.isFired = true;
            break;
        }
    }
    
    m_currentPosition++;
    calculateProbability();
    
    emit bulletFired(bullet.position, isLive);
}

void BulletTracker::addKnownBullet(int position, bool isLive)
{
    // 检查是否已经存在该位置的信息
    for (auto &known : m_knownBullets) {
        if (known.position == position) {
            known.isLive = isLive;
            calculateProbability(); // 修复：重复修改时也要重新计算概率
            return;
        }
    }
    
    // 检查该位置是否已经发射
    bool alreadyFired = false;
    for (const auto &fired : m_bulletHistory) {
        if (fired.position == position) {
            alreadyFired = true;
            break;
        }
    }
    
    if (!alreadyFired) {
        BulletInfo bullet;
        bullet.position = position;
        bullet.isLive = isLive;
        bullet.isKnown = true;
        bullet.isFired = false;
        
        m_knownBullets.append(bullet);
        calculateProbability();
    }
}

void BulletTracker::removeKnownBullet(int position)
{
    for (int i = 0; i < m_knownBullets.size(); ++i) {
        if (m_knownBullets[i].position == position) {
            m_knownBullets.removeAt(i);
            calculateProbability();
            break;
        }
    }
}

void BulletTracker::reset()
{
    m_totalLive = 0;
    m_totalBlank = 0;
    m_remainingLive = 0;
    m_remainingBlank = 0;
    m_currentPosition = 0;
    m_liveProbability = 0.0;
    
    m_bulletHistory.clear();
    m_knownBullets.clear();
}

int BulletTracker::getRemainingLive() const
{
    return m_remainingLive;
}

int BulletTracker::getRemainingBlank() const
{
    return m_remainingBlank;
}

int BulletTracker::getCurrentPosition() const
{
    return m_currentPosition;
}

double BulletTracker::getLiveProbability() const
{
    return m_liveProbability;
}

const QList<BulletTracker::BulletInfo>& BulletTracker::getBulletHistory() const
{
    return m_bulletHistory;
}

const QList<BulletTracker::BulletInfo>& BulletTracker::getKnownBullets() const
{
    return m_knownBullets;
}

void BulletTracker::calculateProbability()
{
    int totalRemaining = m_remainingLive + m_remainingBlank;
    
    if (totalRemaining <= 0) {
        m_liveProbability = 0.0;
        emit probabilityChanged(m_liveProbability);
        return;
    }
    
    // 检查当前位置是否已知
    for (const auto &known : m_knownBullets) {
        if (known.position == m_currentPosition && !known.isFired) {
            m_liveProbability = known.isLive ? 1.0 : 0.0;
            emit probabilityChanged(m_liveProbability);
            return;
        }
    }
    
    // 基础概率计算
    int adjustedLive = m_remainingLive;
    int adjustedBlank = m_remainingBlank;
    
    // 减去已知但未发射的子弹
    for (const auto &known : m_knownBullets) {
        if (!known.isFired && known.position > m_currentPosition) {
            if (known.isLive) {
                adjustedLive = qMax(0, adjustedLive - 1);
            } else {
                adjustedBlank = qMax(0, adjustedBlank - 1);
            }
        }
    }
    
    int adjustedTotal = adjustedLive + adjustedBlank;
    
    if (adjustedTotal <= 0) {
        m_liveProbability = m_remainingLive > 0 ? 1.0 : 0.0;
    } else {
        m_liveProbability = static_cast<double>(adjustedLive) / adjustedTotal;
    }
    
    emit probabilityChanged(m_liveProbability);
}

// ItemManager实现
ItemManager::ItemManager(QObject *parent)
    : QObject(parent)
{
}

void ItemManager::addPlayerItem(ItemType type)
{
    ItemInfo item;
    item.type = type;
    item.name = getItemName(type);
    item.description = getItemDescription(type);
    item.isUsed = false;
    
    m_playerItems.append(item);
    emit itemAdded(true, type);
}

void ItemManager::addDealerItem(ItemType type)
{
    ItemInfo item;
    item.type = type;
    item.name = getItemName(type);
    item.description = getItemDescription(type);
    item.isUsed = false;
    
    m_dealerItems.append(item);
    emit itemAdded(false, type);
}

void ItemManager::usePlayerItem(ItemType type)
{
    for (auto &item : m_playerItems) {
        if (item.type == type && !item.isUsed) {
            item.isUsed = true;
            emit itemUsed(true, type);
            break;
        }
    }
}

void ItemManager::useDealerItem(ItemType type)
{
    for (auto &item : m_dealerItems) {
        if (item.type == type && !item.isUsed) {
            item.isUsed = true;
            emit itemUsed(false, type);
            break;
        }
    }
}

void ItemManager::clearAllItems()
{
    m_playerItems.clear();
    m_dealerItems.clear();
}

void ItemManager::removePlayerItem(int index)
{
    if (index >= 0 && index < m_playerItems.size()) {
        m_playerItems.removeAt(index);
    }
}

void ItemManager::removeDealerItem(int index)
{
    if (index >= 0 && index < m_dealerItems.size()) {
        m_dealerItems.removeAt(index);
    }
}

const QList<ItemManager::ItemInfo>& ItemManager::getPlayerItems() const
{
    return m_playerItems;
}

const QList<ItemManager::ItemInfo>& ItemManager::getDealerItems() const
{
    return m_dealerItems;
}

QString ItemManager::getItemName(ItemType type)
{
    switch (type) {
    case ItemType::MagnifyingGlass: return "放大镜";
    case ItemType::Cigarettes: return "香烟";
    case ItemType::Beer: return "啤酒";
    case ItemType::Handsaw: return "手锯";
    case ItemType::Handcuffs: return "手铐";
    case ItemType::BurnerPhone: return "一次性电话";
    case ItemType::Inverter: return "逆变器";
    case ItemType::Adrenaline: return "肾上腺素";
    case ItemType::ExpiredMedicine: return "过期药物";
    case ItemType::Jammer: return "干扰器";
    case ItemType::Remote: return "遥控器";
    default: return "未知道具";
    }
}

QString ItemManager::getItemDescription(ItemType type)
{
    switch (type) {
    case ItemType::MagnifyingGlass: return "查看当前子弹类型";
    case ItemType::Cigarettes: return "恢复1点生命值";
    case ItemType::Beer: return "弹出并跳过当前子弹";
    case ItemType::Handsaw: return "下一发实弹造成双倍伤害";
    case ItemType::Handcuffs: return "强制对手跳过下一回合";
    case ItemType::BurnerPhone: return "查看随机位置的子弹类型";
    case ItemType::Inverter: return "改变当前子弹的类型";
    case ItemType::Adrenaline: return "偷取并立即使用对手的一个道具";
    case ItemType::ExpiredMedicine: return "50%概率恢复2生命，否则失去1生命";
    case ItemType::Jammer: return "强制选定对手跳过下一回合";
    case ItemType::Remote: return "改变回合顺序";
    default: return "未知效果";
    }
}

// DecisionHelper实现
DecisionHelper::DecisionHelper(QObject *parent)
    : QObject(parent)
{
}

QString DecisionHelper::getAdvice(const GameState &state)
{
    QString advice;
    
    advice += "=== 当前局势分析 ===\n\n";
    advice += analyzeCurrentSituation(state);
    
    advice += "\n=== 推荐行动 ===\n\n";
    advice += recommendAction(state);
    
    advice += "\n=== 道具分析 ===\n\n";
    advice += analyzeItems(state);
    
    return advice;
}

double DecisionHelper::calculateExpectedValue(const GameState &state, bool shootDealer)
{
    int totalRemaining = state.remainingLive + state.remainingBlank;
    if (totalRemaining <= 0) return 0.0;
    
    double liveProbability = static_cast<double>(state.remainingLive) / totalRemaining;
    
    // 检查当前位置是否已知
    for (const auto &known : state.knownBullets) {
        if (known.position == state.currentPosition && !known.isFired) {
            liveProbability = known.isLive ? 1.0 : 0.0;
            break;
        }
    }
    
    if (shootDealer) {
        // 射击庄家的期望值
        double damageMultiplier = state.handsawActive ? 2.0 : 1.0;
        return liveProbability * damageMultiplier - (1.0 - liveProbability) * 0.1; // 空包弹有小惩罚
    } else {
        // 射击自己的期望值
        return (1.0 - liveProbability) * 1.0 - liveProbability * 2.0; // 实弹惩罚更大
    }
}

QString DecisionHelper::analyzeCurrentSituation(const GameState &state)
{
    QString analysis;
    
    int totalRemaining = state.remainingLive + state.remainingBlank;
    double liveProbability = totalRemaining > 0 ? 
        static_cast<double>(state.remainingLive) / totalRemaining : 0.0;
    
    // 检查当前位置是否已知
    bool currentKnown = false;
    bool currentIsLive = false;
    for (const auto &known : state.knownBullets) {
        if (known.position == state.currentPosition && !known.isFired) {
            currentKnown = true;
            currentIsLive = known.isLive;
            liveProbability = currentIsLive ? 1.0 : 0.0;
            break;
        }
    }
    
    analysis += QString("剩余子弹：%1发实弹，%2发空包弹\n")
        .arg(state.remainingLive).arg(state.remainingBlank);
    
    if (currentKnown) {
        analysis += QString("当前子弹：%1（已知）\n")
            .arg(currentIsLive ? "实弹" : "空包弹");
    } else {
        analysis += QString("当前子弹概率：%1% 实弹，%2% 空包弹\n")
            .arg(liveProbability * 100, 0, 'f', 1).arg((1.0 - liveProbability) * 100, 0, 'f', 1);
    }
    
    analysis += QString("生命值：玩家 %1，庄家 %2\n")
        .arg(state.playerHealth).arg(state.dealerHealth);
    
    if (state.handsawActive) {
        analysis += "手锯激活：下一发实弹造成双倍伤害\n";
    }
    
    return analysis;
}

QString DecisionHelper::recommendAction(const GameState &state)
{
    QString recommendation;
    
    int totalRemaining = state.remainingLive + state.remainingBlank;
    if (totalRemaining <= 0) {
        return "回合结束，等待新回合开始。";
    }
    
    double liveProbability = static_cast<double>(state.remainingLive) / totalRemaining;
    
    // 检查当前位置是否已知
    bool currentKnown = false;
    bool currentIsLive = false;
    for (const auto &known : state.knownBullets) {
        if (known.position == state.currentPosition && !known.isFired) {
            currentKnown = true;
            currentIsLive = known.isLive;
            liveProbability = currentIsLive ? 1.0 : 0.0;
            break;
        }
    }
    
    double shootDealerEV = calculateExpectedValue(state, true);
    double shootSelfEV = calculateExpectedValue(state, false);
    
    if (currentKnown) {
        if (currentIsLive) {
            if (state.handsawActive) {
                recommendation += "强烈推荐：射击庄家（已知实弹 + 手锯双倍伤害）\n";
            } else {
                recommendation += "推荐：射击庄家（已知实弹）\n";
            }
        } else {
            recommendation += "推荐：射击自己（已知空包弹，可以连续行动）\n";
        }
    } else {
        if (liveProbability >= 0.7) {
            recommendation += "推荐：射击庄家（高实弹概率）\n";
            if (state.handsawActive) {
                recommendation += "手锯激活，伤害翻倍，强烈推荐射击庄家！\n";
            }
        } else if (liveProbability <= 0.3) {
            recommendation += "推荐：射击自己（低实弹概率，有机会连续行动）\n";
        } else {
            recommendation += "中等概率，建议根据当前局势和道具情况决定：\n";
            recommendation += QString("- 射击庄家期望值：%1\n").arg(shootDealerEV);
            recommendation += QString("- 射击自己期望值：%1\n").arg(shootSelfEV);
            
            if (shootDealerEV > shootSelfEV) {
                recommendation += "轻微推荐：射击庄家\n";
            } else {
                recommendation += "轻微推荐：射击自己\n";
            }
        }
    }
    
    return recommendation;
}

QString DecisionHelper::analyzeItems(const GameState &state)
{
    QString itemAnalysis;
    
    // 分析玩家道具
    itemAnalysis += "玩家道具建议：\n";
    bool hasUsefulPlayerItems = false;
    
    for (const auto &item : state.playerItems) {
        if (item.isUsed) continue;
        
        hasUsefulPlayerItems = true;
        switch (item.type) {
        case ItemManager::ItemType::MagnifyingGlass:
            itemAnalysis += "- 放大镜：立即使用查看当前子弹\n";
            break;
        case ItemManager::ItemType::Beer:
            if (state.remainingLive > state.remainingBlank) {
                itemAnalysis += "- 啤酒：当前实弹较多，可以考虑使用跳过当前子弹\n";
            } else {
                itemAnalysis += "- 啤酒：当前空包弹较多，暂不推荐使用\n";
            }
            break;
        case ItemManager::ItemType::Handsaw:
            if (state.remainingLive > 0) {
                itemAnalysis += "- 手锯：在确认下一发是实弹时使用，造成双倍伤害\n";
            }
            break;
        case ItemManager::ItemType::Handcuffs:
            itemAnalysis += "- 手铐：可以跳过庄家回合，在关键时刻使用\n";
            break;
        case ItemManager::ItemType::BurnerPhone:
            if (state.remainingLive + state.remainingBlank >= 2) {
                itemAnalysis += "- 一次性电话：获取额外信息，建议使用\n";
            }
            break;
        case ItemManager::ItemType::Inverter:
            itemAnalysis += "- 逆变器：在已知当前子弹类型时，可以反转为有利情况\n";
            break;
        case ItemManager::ItemType::Cigarettes:
            if (state.playerHealth < 3) {
                itemAnalysis += "- 香烟：当前生命值较低，可以考虑使用恢复\n";
            }
            break;
        default:
            itemAnalysis += QString("- %1：根据具体情况使用\n").arg(item.name);
            break;
        }
    }
    
    if (!hasUsefulPlayerItems) {
        itemAnalysis += "- 无可用道具\n";
    }
    
    // 分析庄家道具威胁
    itemAnalysis += "\n庄家道具威胁：\n";
    bool hasDealerThreats = false;
    
    for (const auto &item : state.dealerItems) {
        if (item.isUsed) continue;
        
        hasDealerThreats = true;
        switch (item.type) {
        case ItemManager::ItemType::MagnifyingGlass:
            itemAnalysis += "- 庄家有放大镜：庄家可能知道当前子弹类型\n";
            break;
        case ItemManager::ItemType::Handsaw:
            itemAnalysis += "- 庄家有手锯：小心庄家使用双倍伤害\n";
            break;
        case ItemManager::ItemType::Handcuffs:
            itemAnalysis += "- 庄家有手铐：可能会跳过你的回合\n";
            break;
        case ItemManager::ItemType::Inverter:
            itemAnalysis += "- 庄家有逆变器：当前子弹类型可能被改变\n";
            break;
        default:
            itemAnalysis += QString("- 庄家有%1\n").arg(item.name);
            break;
        }
    }
    
    if (!hasDealerThreats) {
        itemAnalysis += "- 庄家无威胁道具\n";
    }
    
    return itemAnalysis;
}
