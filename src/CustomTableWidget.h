#pragma once

#include "PCH.h"

#include "Configurations.h"

class CustomTableWidget : public QTableWidget {
public:
    CustomTableWidget(int rows, int cols, QWidget *parent = nullptr) 
        : QTableWidget(rows, cols, parent) {
        
        // CRITICAL: Enable mouse tracking on the viewport
        this->viewport()->setMouseTracking(true);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        // Get the model index under the current mouse position
        QModelIndex index = this->indexAt(event->pos());

        // alter cursor shape
        if (index.isValid() && index.column() == SCANNER_SAMPLES_COLUMN6) {
            this->viewport()->setCursor(Qt::PointingHandCursor);
        } else {
            this->viewport()->setCursor(Qt::ArrowCursor);
        }

        // Fallback to default class behaviors
        QTableWidget::mouseMoveEvent(event);
    }
};