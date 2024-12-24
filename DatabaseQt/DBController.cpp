#include "DBController.h"

void DBController::log(const QString& msg, Type type) const
{
	if (msg.isEmpty())
		return;

	Color color = Color(type);

	TerminalColor::SetColor(Color::LightBlue);
	cout << "[" << this->name.toStdString() << "]: ";
	TerminalColor::SetColor(Color::White);

	TerminalColor::PrintInColor(msg.toStdString(), color);

	cout << "\n";
}

DBController::DBController(const QString& file, bool creatingNew)
{
	// Sprawdzenie rozszerzenia pliku
	QFileInfo filepath(file);
	if (filepath.suffix() != "db")
	{
		cout << "db: Nie udało się nawiązać połączenia z bazą danych: Niepoprawny typ pliku\n";
		return;
	}

	// Ustawienie nazwy bazy danych
	this->name = filepath.fileName();

	// Połączenie z bazą danych
	this->connectionID = "connection_";
	this->connectionID += this->name;

	this->conn = QSqlDatabase::addDatabase("QSQLITE", this->connectionID);
	this->conn.setDatabaseName(file);

	QString msg("Wystąpił błąd podczas próby połączenia z bazą danych: ");
	msg += this->conn.lastError().text();

	// Sprawdzenie połączenia
	if (!this->conn.open())
		this->log(msg, Type::Error);
	else
	{
		// Wypisanie powiadomienia o utworzeniu nowego pliku
		if(creatingNew)
			this->log(QString("Utworzono nowy plik z bazą danych w lokalizacji: %1").arg(file));
		
		this->log("Pomyślnie połączono się z bazą danych", Type::Connection);
	}
}

DBController::~DBController()
{
	if (this == nullptr)
		return;
	
	// Usuwanie modeli, aby zwolnić zasoby
	if (this->saveStatus)
	// Jeżeli użytkownik chce zapisać
	{
		this->log("Zmiany zostały zapisane", Type::Success);
		for (auto &model : this->models) 
		{
			int saveResult = model->submitAll();
			
			if (!saveResult) 
				this->log(QString("Wystąpił błąd podczas zapisu: %1").arg(model->lastError().text()), Type::Error);
			
			delete model; 
		}
	}
	else
	// Jeżeli użytkownik nie chce zapisywać
	{
		this->log("Zmiany zostały odrzucone", Type::Error);
		for (auto &model : this->models) 
		{
			model->revertAll();
			delete model;
		}
	}
	this->models.clear();

	// Zamknięcie połączenia z bazą danych
	if (this->conn.isOpen())
	{
		this->log("Zamykanie połączenia z bazą danych", Type::Disconnection);
		this->conn.close();
		this->conn = QSqlDatabase();
		QSqlDatabase::removeDatabase(this->connectionID);
	}
}

bool DBController::IsOpen() const
{
	return this->conn.isOpen();
}

void DBController::SetSaveStatus(bool newStatus)
{
	this->saveStatus = newStatus;
}

bool DBController::DropTable(const QString& table)
{
	// Utworzenie polecenia do bazy danych
	QSqlQuery query(this->conn);
	QString queryString = QString("DROP TABLE IF EXISTS %1").arg(table);

	// Wypisanie na konsole 
	this->log(queryString);

	// Wykonanie polecenia
	if (!query.exec(queryString)) 
	{
		QString errorMsg = QString("Nie udało się usunąć tabeli '%1': %2").arg(table, query.lastError().text());
		this->log(errorMsg, Type::Error);
		return false;
	}

	// Wypisanie na konsole jeżeli operacja się powiodła
	QString successMsg = QString("Tabela '%1' została pomyślnie usunięta.").arg(table);
	this->log(successMsg, Type::Success);

	// Usunięcie modelu
	delete this->models[table];
	this->models.remove(table);

	return true;
}

QSqlTableModel* DBController::GetTableModel(const QString& table)
{
	// Jeżeli istnieje już podany model dla tabeli
	if (this->models.contains(table))
		return this->models[table];

	// Wypisanie wiadomości na konsole o pobraniu danych z tabeli
	this->log(QString("Select * FROM '%1'").arg(table));

	// Utworzenie nowego modelu
	QSqlTableModel* model = new QSqlTableModel(nullptr, this->conn);
	model->setTable(table);
	
	// Wczytanie danych do modelu
	model->select();

	// Zablokowanie automatycznego zapisywania danych
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);

	// Dodanie do kolekcji wszystkich modelów
	this->models.insert(table, model);

	return this->models[table];
}

QStringList DBController::GetTables() const
{
	QStringList tables = this->conn.tables();
	return tables;
}

void DBController::RemoveRows(const QString& table, const QModelIndexList& selectedRows)
{
	for (const QModelIndex& index : selectedRows)
		this->models[table]->removeRow(index.row());

	this->log(QString("Usuwanie wierszy z tabeli '%1'").arg(table), Type::Query);
}

void DBController::InsertRows(const QString& table, int startIndex, int rowsCount)
{
	this->log(QString("Dodawanie %1 wierszy do tabeli '%2'").arg(rowsCount).arg(table));
	this->models[table]->insertRows(0, rowsCount);
}

bool DBController::Query(const QString& queryString)
{
	// Utworznie polecenia dla tej bazy danych
	QSqlQuery query(this->conn);

	this->log(queryString);

	// Wykonanie polecenia na bazie danych
	if (!query.exec(queryString)) 
	{
		QString errorMsg = QString("Nie udało się poprawnie dodać tabeli: %1").arg(query.lastError().text());
		this->log(errorMsg, Type::Error);
		return false;
	}

	this->log("Pomyślnie dodano nową tabele do bazy danych", Type::Success);
	return true;
}

void DBController::FilterTable(const QString& table, const QString& col, const QString& params)
{
	QString filter = QString("\"%1\" LIKE '%%2%'").arg(col).arg(params);
	this->log(QString("SELECT * FROM '%1' WHERE %2").arg(table).arg(filter));
	this->models[table]->setFilter(filter);
	this->models[table]->select();
}

void DBController::SelectAll(const QString& table)
{
	this->models[table]->select();
}

bool DBController::SaveTable(const QString& table)
{
	bool submitResult = this->models[table]->submitAll();
	if (submitResult)
		this->log(QString("Pomyślnie zapisano dane w tabeli '%1'").arg(table), Type::Success);
	else
	{
		QString err = this->models[table]->lastError().text();
		this->log(QString("Wystąpił błąd podczas próby zapisu danych do tabeli '%1': %2").arg(table).arg(err), Type::Error);
	}

	return submitResult;
}
