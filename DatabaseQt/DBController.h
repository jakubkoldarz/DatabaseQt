#pragma once

#include <iostream>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>
#include <QFileInfo>
#include <QTableView>
#include <QSqlTableModel>

using std::cout;
using std::endl;

class DBController
{
private:
	QString name;
	QString connectionID;
	QSqlDatabase conn;
	QMap<QString, QSqlTableModel*> models;

	void log(const QString& msg) const;
public:
	DBController(const QString& file);
	~DBController();
	bool Query(const QString& query);
	bool IsOpen();
	QSqlTableModel* GetTableModel(const QString& table);
	QStringList GetTables() const;
};

