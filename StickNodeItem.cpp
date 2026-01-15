#include "StickNodeItem.h"

StickNodeItem::StickNodeItem()
{
    setFlags(ItemIsSelectable); // NOT movable
}

QVariant StickNodeItem::itemChange(GraphicsItemChange change,
    const QVariant& value)
{
    // Prevent movement
    if (change == ItemPositionChange)
        return pos();

    return NodeItem::itemChange(change, value);
}
