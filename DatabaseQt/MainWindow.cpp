#include "MainWindow.h"

/// <summary>
///  Otwiera okienko do wyboru pliku, a następnie łączy się z bazą danych
/// </summary>
void MainWindow::onLoadButtonClicked()
{
	// Pobranie ścieżki do pulpitu użytkownika
	// Głównie w celu wygody wybierania baz danych
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

	// Przypisanie opisu okienka oraz filtrów do plików
	QString caption = "Otwórz plik";
	QString filters = "Baza danych (*.db);;Wszystkie pliki (*);;";

	// Pobranie ścieżki do pliku za pomocą okienka dialogowego
	QString filepath = QFileDialog::getOpenFileName(this, caption, desktopPath, filters);
	
	// Jeżeli użytkownik anulował wybieranie pliku
	if (filepath.isEmpty())
	{
		QString title("Brak wybranego pliku");
		QString message("Nie wybrano żadnego pliku. Operacja została anulowana. Aplikacja pozostaje w aktualnym stanie bez żadnych zmian.");
		QMessageBox::information(this, title, message);
		return;
	}

	// Jeżeli wczytano nowy plik i istniało już takie połączenie
	QFileInfo fileInfo = QFileInfo(filepath);
	QString fileName = fileInfo.fileName();
	if (this->databases.contains(fileName))
	{
		QString title("Wybrano duplikat");
		QString message("Baza danych z tego pliku jest już aktualnie w użyciu. Operacja została anulowana. Aplikacja pozostaje w aktualnym stanie bez żadnych zmian.");
		QMessageBox::information(this, title, message);
		return;
	}

	// Utworzenie połączenia z bazą danych
	this->databases[fileName] = new DBController(filepath);
	
	// Wczytanie tabel do zakładek
	this->setupTabs(fileName);
}

/// <summary>
/// Wczytuje dane z bazy danych do zakładek w oknie głównym
/// </summary>
void MainWindow::setupTabs(const QString& tabName)
{
	// Pobranie tabel z bazy danych
	QStringList tables = this->databases[tabName]->GetTables();
	int tablesCount = tables.size();

	QTabWidget* tablesTabs = new QTabWidget(this->dbTabs);

	for (int i = 0; i < tablesCount; i++)
	{
		// Utworzenie zakładki
		QWidget* singleTab = new QWidget(this->dbTabs);

		// Utworzenie układu dla zakładki
		QVBoxLayout* layoutTemp = new QVBoxLayout(singleTab);

		// Utworzenie tabeli z danymi z bazy danych
		QTableView* singleTable = new QTableView(this);
		singleTable->setModel(this->databases[tabName]->GetTableModel(tables[i]));

		// Dodanie tabeli do układu 
		layoutTemp->addWidget(singleTable);

		// Przypisanie układu do zakładki
		singleTab->setLayout(layoutTemp);

		// Dodanie zakładki do listy wszystkich zakładek
		tablesTabs->addTab(singleTab, tables[i]);
	}

	// Dodanie zakładek z nazwami tabel do głownej zakładki bazy danych
	this->dbTabs->addTab(tablesTabs, tabName);
}

/// <summary>
/// Usuwa połączenie z bazą danych oraz czyści zakładki
/// </summary>
void MainWindow::deleteDatabaseConnection()
{
	// Wyczyszczenie zakładek
	//while (this->tabs->count() > 0)
	//{
	//	QWidget* currentTab = this->tabs->widget(0);
	//	this->tabs->removeTab(0);
	//	delete currentTab;
	//}

	//// Usunięcie poprzedniego połączenia
	//if (this->db == nullptr)
	//	return;

	//delete this->db;
	//this->db = nullptr;
}

MainWindow::MainWindow(QWidget* parent)
{
	// Ustawienie rozmiaru okna oraz zablokowanie zmiany rozmiaru
	this->setFixedSize(1200, 700);
	this->setWindowTitle("Aplikacja Bazodanowa");
	
	// Ustawienie widgetu centralnego
	QWidget* centralWidget = new QWidget(this);
	this->setCentralWidget(centralWidget);
	QGridLayout* layout = new QGridLayout(centralWidget);
	
	QPushButton* buttonLoad = new QPushButton("Otwórz");
	QPushButton* buttonClose = new QPushButton("Zamknij");

	buttonLoad->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	buttonClose->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	connect(buttonLoad, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);
	connect(buttonClose, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);

	this->dbTabs = new QTabWidget(this);

	layout->setRowStretch(0, 1); 
	layout->setRowStretch(1, 20); 
	layout->setRowStretch(2, 1); 

	layout->setColumnStretch(0, 1); 
	layout->setColumnStretch(1, 4);

	centralWidget->setLayout(layout);

	layout->addWidget(buttonLoad, 0, 0);
	//layout->addWidget(buttonClose, 1, 0);
	layout->addWidget(dbTabs, 0, 1, 2, 1);
}

MainWindow::~MainWindow()
{
	for (auto& db : this->databases)
	{
		if (db != nullptr)
			delete db;
	}
}

void MainWindow::onCloseButtonClicked()
{
	this->deleteDatabaseConnection();
}
