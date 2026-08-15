#pragma once

#include "PCH.h"

class KeysFilter : public QObject {
private:
    QTableWidget* m_table;
    //QWidget* m_window;

public:
    explicit KeysFilter(QTableWidget *table) 
        : QObject(table),
        m_table(table)//,
        //m_window(window)
        {}

protected:
    // This function automatically intercepts events
    bool eventFilter(QObject *obj, QEvent *event) override {
        // 1. Check if the event is a key press
        if (event->type() == QEvent::KeyPress) {
            // 2. Convert the generic event into a key event
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            
            // 3. Check if the pressed key is exactly F5
            if (keyEvent->key() == Qt::Key_F5) {
                m_table->clearContents();
                m_table->setRowCount(0);
                
                // @todo treat the command line parameters first
                processGUI(*m_table, SCANNER_SAMPLES_DIRECTORY, SCANNER_SAMPLES_LIMIT);

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