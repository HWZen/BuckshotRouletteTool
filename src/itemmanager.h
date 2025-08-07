#pragma once

#include <QObject>
#include <QList>
#include <QString>

class ItemManager : public QObject {
    Q_OBJECT

public:
    enum class ItemType {
        MagnifyingGlass,    // 放大镜 - 查看当前子弹
        Cigarettes,         // 香烟 - 恢复生命
        Beer,              // 啤酒 - 弹出当前子弹
        Handsaw,           // 手锯 - 双倍伤害
        Handcuffs,         // 手铐 - 跳过对手回合
        BurnerPhone,       // 一次性电话 - 查看随机子弹
        Inverter,          // 逆变器 - 改变当前子弹类型
        Adrenaline,        // 肾上腺素 - 偷取并使用道具
        ExpiredMedicine,   // 过期药物 - 50%概率恢复2生命或失去1生命
        Jammer,            // 干扰器 - 跳过选定对手回合
        Remote             // 遥控器 - 改变回合顺序
    };

    struct ItemInfo {
        ItemType type;
        QString name;
        QString description;
        bool isUsed;
    };

    explicit ItemManager(QObject *parent = nullptr);
    
    void addPlayerItem(ItemType type);
    void addDealerItem(ItemType type);
    void usePlayerItem(ItemType type);
    void useDealerItem(ItemType type);
    void clearAllItems();
    
    const QList<ItemInfo>& getPlayerItems() const;
    const QList<ItemInfo>& getDealerItems() const;
    
    static QString getItemName(ItemType type);
    static QString getItemDescription(ItemType type);

signals:
    void itemAdded(bool isPlayer, ItemType type);
    void itemUsed(bool isPlayer, ItemType type);

private:
    QList<ItemInfo> m_playerItems;
    QList<ItemInfo> m_dealerItems;
};
