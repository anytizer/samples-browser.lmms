#pragma once

#include "PCH.h"

class KeysFilter : public QObject {
private:
    CustomTableWidget* m_table;

public:
    explicit KeysFilter(CustomTableWidget *table) 
        : QObject(table),
        m_table(table)
        {}

protected:
    // intercepts events
    bool eventFilter(QObject *obj, QEvent *event) override {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
  
            if (keyEvent->key() == Qt::Key_F5) {
                // @todo treat the command line parameter first, for directory
                //processGUI(&m_table, SCANNER_SAMPLES_DIRECTORY);

                qDebug() << "The F5 key was pressed.";
                return true; 
            }

            if (keyEvent->key() == Qt::Key_Escape) {
                // in main window, exit.
                // if called under LMMS as Plugin, hide
                qDebug() << "The ESC key was pressed.";
                return true;
            }
        }

        // Pass all other events (like mouse movement) back to the window
        return QObject::eventFilter(obj, event);
    }
};