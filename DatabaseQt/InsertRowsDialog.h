#pragma once

#ifndef ADDROWSDIALOG_H
#define ADDROWSDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class AddRowsDialog : public QDialog
{
    Q_OBJECT

private:
    QSpinBox* spinBox;
public:
    explicit AddRowsDialog(QWidget* parent = nullptr);
    int GetNumberOfRows() const;
};

#endif // ADDROWSDIALOG_H
