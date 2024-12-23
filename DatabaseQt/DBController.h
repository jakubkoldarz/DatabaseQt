#pragma once

#include "TerminalColor.h"

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

enum Type {
	Disconnection = 1, Connection = 2, 
	Query = 7,
	Error = 9, Success = 2
};

class DBController
{
private:
	QString name;
	QString connectionID;
	QSqlDatabase conn;
	QMap<QString, QSqlTableModel*> models;
	bool saveStatus{ true };

	/// <summary>
	/// Wyświetla wiadomość na konsoli 
	/// </summary>
	/// <param name="msg">Wiadomość, która zostanie wypisana na konsoli</param>
	void log(const QString& msg, Type type = Type::Query) const;
public:
	DBController(const QString& file);
	~DBController();
	/// <summary>
	/// Zwraca wartość logiczną na podstawie statusu połączenia z bazą danych
	/// </summary>
	/// <returns>Status połączenia</returns>
	bool IsOpen() const;
	/// <summary>
	/// Określa czy zmiany w bazie danych mają zostać odrzucone czy zapisane po wywołaniu destruktora
	/// </summary>
	/// <param name="newStatus">Nowy status zapisywania</param>
	void SetSaveStatus(bool newStatus);
	/// <summary>
	/// Usuwa tabelę z bazy danych
	/// </summary>
	/// <param name="table">Nazwa tabeli do usunięcia</param>
	/// <returns>True w przypadku powodzenia operacji usuwania, False w przeciwnym wypadku</returns>
	bool DropTable(const QString& table);
	/// <summary>
	/// Pobiera dane z bazy danych i tworzy model na ich podstawie
	/// </summary>
	/// <param name="table">Nazwa tabeli</param>
	/// <returns>Wskaźnik na obiekt QSqlTableModel reprezentujący tabele w bazie danych</returns>
	QSqlTableModel* GetTableModel(const QString& table);
	/// <summary>
	/// Pobiera listę tabel z bazy danych
	/// </summary>
	/// <returns>Lista tabel zawartych w bazie danych</returns>
	QStringList GetTables() const;
	void RemoveRows(const QString& table, const QModelIndexList selectedRows);
	void InsertRows(const QString& table, int startIndex, int rowsCount);
	void createNewDatabase(const QString& name);
};

