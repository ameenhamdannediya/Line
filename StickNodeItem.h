#pragma once
#include "NodeItem.h"

class StickNodeItem : public NodeItem
{
public:
    StickNodeItem();

protected:
    QVariant itemChange(GraphicsItemChange change,
        const QVariant& value) override;
};
