#pragma once

#ifndef CREATENEWDIALOG_H
#define CREATENEWDIALOG_H

#include <QDialog>
#include <QSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>

class CreateNewDialog : public QDialog
{
    Q_OBJECT

private:
    QLineEdit* newName;
public:
    explicit CreateNewDialog(QWidget* parent = nullptr);
    QString GetNewName() const;
};

#endif // CREATENEWDIALOG_H
