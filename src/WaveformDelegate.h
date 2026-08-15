#pragma once

#include "PCH.h"

// ---------------------------------------------------------
// Waveform delegate
//
// The waveform is LEFT aligned inside the graph cell.
// ---------------------------------------------------------
class WaveformDelegate : public QStyledItemDelegate
{
public:

    explicit WaveformDelegate(
        QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
    {
    }

    void paint(
        QPainter *painter,
        const QStyleOptionViewItem &option,
        const QModelIndex &index) const override
    {
        QVariant value = index.data(Qt::DecorationRole);

        if (!value.isValid()) {
            QStyledItemDelegate::paint(
                painter,
                option,
                index);
            return;
        }

        QPixmap pixmap = qvariant_cast<QPixmap>(value);

        if (pixmap.isNull()) {
            QStyledItemDelegate::paint(painter, option, index);
            return;
        }

        // Draw selection background.
        if (option.state & QStyle::State_Selected) {
            painter->fillRect(option.rect, option.palette.highlight());
        }

        QRect available = option.rect.adjusted(2, 2, -2, -2);

        // Fit image to available height/width.
        QPixmap scaled =
            pixmap.scaled(
                available.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

        // LEFT ALIGN.
        const int x = available.left();

        // Vertically center.
        const int y =
            available.top() +
            (available.height() -
             scaled.height()) / 2;

        painter->drawPixmap(x, y, scaled);
    }
};