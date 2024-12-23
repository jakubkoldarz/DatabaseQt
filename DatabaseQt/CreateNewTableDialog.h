#pragma once
#ifndef CREATENEWTABLEDIALOG_H
#define CREATENEWTABLEDIALOG_H

#include "InsertRowsDialog.h"

#include <iostream>

#include <QDialog>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <qheaderview.h>
#include <QLineEdit>
#include <QCheckBox>
#include <QMenu>
#include <QMessageBox>
#include <qbuttongroup.h>
#include <qcombobox.h>

class CreateNewTableDialog : public QDialog
{
    Q_OBJECT

private:
    QTableWidget* table;
    QButtonGroup* checkboxes;
    QLineEdit* newTableName;

    void removeRows(const QModelIndexList& selectedRows);
    void insertRows(int count);
    void updateRows();
public:
    explicit CreateNewTableDialog(QWidget* parent = nullptr);
    QString GetQueryString() const;
private slots:
    void onInsertRowRequest(const QPoint& pos);
};

#endif // CREATENEWTABLEDIALOG_H