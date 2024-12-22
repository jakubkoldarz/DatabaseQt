#include "DBController.h"

/// <summary>
/// Wyświetla wiadomość na konsoli 
/// </summary>
/// <param name="msg"></param>
void DBController::log(const QString& msg) const
{
	if (msg.isEmpty())
		return;

	cout << "db [" << this->name.toStdString() << "]: ";
	cout << msg.toStdString();
	cout << "\n";
}

DBController::DBController(const QString& file)
{
	// Sprawdzenie rozszerzenia pliku
	QFileInfo filepath(file);
	if (filepath.suffix() != "db")
	{
		cout << "Cannot create connection with database: Invalid file type\n";
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
		this->log(msg);
	else
		this->log("Pomyślnie połączono się z bazą danych");
}

DBController::~DBController()
{
	if (this == nullptr)
		return;
	
	// Usuwanie modeli, aby zwolnić zasoby
	for (auto model : this->models) {
		model->revertAll();
		delete model; 
	}
	this->models.clear();

	// Zamknięcie połączenia z bazą danych
	if (this->conn.isOpen())
	{
		this->log("Zamykanie połączenia z bazą danych\n\n");
		this->conn.close();
		this->conn = QSqlDatabase();
		QSqlDatabase::removeDatabase(this->connectionID);
	}
}

bool DBController::Query(const QString& query)
{
	return false;
}

/// <summary>
/// Zwraca wartość logiczną na podstawie statusu połączenia z bazą danych
/// </summary>
/// <returns></returns>
bool DBController::IsOpen()
{
	return this->conn.isOpen();
}

/// <summary>
/// Zwraca wskaźnik na obiekt QSqlTableModel reprezentujący tabele w bazie danych
/// </summary>
/// <param name="table">Nazwa tabeli</param>
/// <returns></returns>
QSqlTableModel* DBController::GetTableModel(const QString& table)
{
	// Jeżeli istnieje już podany model dla tabeli
	if (this->models.contains(table))
		return this->models[table];

	// Wypisanie wiadomości na konsole o pobraniu danych z tabeli
	QString msg("SELECT * FROM ");
	msg += table;
	this->log(msg);

	// Utworzenie nowego modelu
	QSqlTableModel* model = new QSqlTableModel(nullptr, this->conn);
	model->setTable(table);
	
	// Wczytanie danych do modelu
	model->select();

	// Dodanie do kolekcji wszystkich modelów
	this->models.insert(table, model);

	return this->models[table];
}

/// <summary>
/// Zwraca listę tabel z bazy danych
/// </summary>
/// <returns></returns>
QStringList DBController::GetTables() const
{
	QStringList tables = this->conn.tables();
	return tables;
}
