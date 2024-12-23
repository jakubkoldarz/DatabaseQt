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

DBController::DBController(const QString& file)
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
		this->log("Pomyślnie połączono się z bazą danych", Type::Connection);
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
		for (auto &model : this->models) {
			model->submitAll();
			delete model; 
		}
	}
	else
	// Jeżeli użytkownik nie chce zapisywać
	{
		this->log("Zmiany zostały odrzucone", Type::Error);
		for (auto &model : this->models) {
			model->revertAll();
			delete model;
		}
	}
	this->models.clear();

	// Zamknięcie połączenia z bazą danych
	if (this->conn.isOpen())
	{
		this->log("Zamykanie połączenia z bazą danych\n", Type::Disconnection);
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
	if (!query.exec(queryString)) {
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

void DBController::RemoveRows(const QString& table, QSqlTableModel* model, const QModelIndexList selectedRows)
{
	for (const QModelIndex& index : selectedRows)
		model->removeRow(index.row());

	this->log(QString("Usuwanie wierszy z tabeli '%1'").arg(table), Type::Query);
}
