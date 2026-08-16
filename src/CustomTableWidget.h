#pragma once

#include "PCH.h"

#include "Configurations.h"

class CustomTableWidget : public QTableWidget {
public:
    CustomTableWidget(int rows, int cols, QWidget *parent = nullptr) 
        : QTableWidget(rows, cols, parent) {
        
        // 1. CRITICAL: Enable mouse tracking on the viewport
        this->viewport()->setMouseTracking(true);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        // 2. Get the model index under the current mouse position
        QModelIndex index = this->indexAt(event->pos());

        // 3. Check conditions and alter cursor shape
        if (index.isValid() && index.column() == SCANNER_SAMPLES_COLUMN6) {
            this->viewport()->setCursor(Qt::PointingHandCursor);
        } else {
            this->viewport()->setCursor(Qt::ArrowCursor);
        }

        // 4. Fallback to default class behaviors
        QTableWidget::mouseMoveEvent(event);
    }
};