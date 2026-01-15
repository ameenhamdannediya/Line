#pragma once
#include "ConnectionItem.h"

class StickConnectionItem : public ConnectionItem
{
public:
    StickConnectionItem(NodeItem* start, NodeItem* end);

    void updatePosition() override;

    void paint(QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget = nullptr) override;

};
