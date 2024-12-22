#include "MainWindow.h"


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

void MainWindow::setupTabs(const QString& tabName)
{
	// Pobranie tabel z bazy danych
	QStringList tables = this->databases[tabName]->GetTables();
	int tablesCount = tables.size();

	// Utworzenie zakładek dla tabel w bazie danych
	QTabWidget* tablesTabs = new QTabWidget(this->dbTabs);
	tablesTabs->setTabsClosable(true);
	connect(tablesTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onTableRemoveRequest);

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

	// Utworzenie zakładki do dodawania nowych tabel
	QWidget* newTableTab = new QWidget(this->dbTabs);

	// Utworzenie układu dla zakładki
	QVBoxLayout* newTableTabLayout = new QVBoxLayout(newTableTab);

	// Dodanie tabeli do układu 
	newTableTabLayout->addWidget(new QLabel("Nowa tabela"));

	// Przypisanie układu do zakładki
	newTableTab->setLayout(newTableTabLayout);

	// Dodanie zakładki do listy wszystkich zakładek
	tablesTabs->addTab(newTableTab, "Nowa tabela");

	// Dodanie zakładek z nazwami tabel do głownej zakładki bazy danych
	this->dbTabs->insertTab(0, tablesTabs, tabName);
	this->dbTabs->setCurrentIndex(0);
}

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
	QGridLayout* centralLayout = new QGridLayout(centralWidget);

	QString buttonStyle = "padding: 11px 0px; width: 200px;";
	QPushButton* buttonLoad = new QPushButton("Otwórz bazę danych");
	buttonLoad->setStyleSheet(buttonStyle);
	QPushButton* buttonNew = new QPushButton("Stwórz nową bazę danych");
	buttonNew->setStyleSheet(buttonStyle);

	connect(buttonLoad, &QPushButton::clicked, this, &MainWindow::onLoadButtonClicked);
	connect(buttonNew, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);

	// Utworzenie kontenera dla zakładek z baz danych
	this->dbTabs = new QTabWidget(this);

	// Ustawienie możliwości zamykania zakładek
	this->dbTabs->setTabsClosable(true);
	
	// Obsługa zamykania zakładek
	connect(dbTabs, &QTabWidget::tabCloseRequested, this, &MainWindow::onDBCloseRequest);

	// Tworzymy nową zakładkę
	QWidget* createNew = new QWidget(this); // Tworzymy widget dla zakładki "Nowa"
	QVBoxLayout* createNewLayout = new QVBoxLayout(createNew); // Układ dla zakładki "Nowa"

	// Tworzymy panel przycisków
	QWidget* buttonsPanel = new QWidget(createNew);
	QHBoxLayout* buttonsPanelLayout = new QHBoxLayout(buttonsPanel);

	buttonsPanelLayout->setAlignment(Qt::AlignCenter);
	buttonsPanelLayout->setContentsMargins(0, 0, 0, 0);

	buttonsPanelLayout->addWidget(buttonLoad);
	buttonsPanelLayout->addWidget(new QLabel("lub"));
	buttonsPanelLayout->addWidget(buttonNew);
	buttonsPanel->setLayout(buttonsPanelLayout);

	// Dodajemy panel do układu zakładki
	createNewLayout->addWidget(buttonsPanel);  // Dodajemy panel przycisków do layoutu zakładki
	createNew->setLayout(createNewLayout);     // Przypisanie układu do widgetu zakładki

	// Dodajemy zakładkę do QTabWidget
	this->dbTabs->addTab(createNew, "Nowa baza danych");

	// Ustawienie szerokości wierszy okna głównego
	centralLayout->setRowStretch(0, 1);
	centralLayout->setRowStretch(1, 20);

	// Przypisanie głównemu widgetowi układu
	centralWidget->setLayout(centralLayout);

	// Dodajemy QTabWidget do głównego układu
	centralLayout->addWidget(dbTabs, 1, 0);
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

void MainWindow::onDBCloseRequest(int index)
{
	// Blokowanie opcji zamknięcia zakładki z kreatorem nowej bazy danych
	if (index == this->dbTabs->count() - 1)
		return;

	// Pobranie nazwy zakładki
	QString tabName = this->dbTabs->tabText(index);


	// Wyświetlenie powiadomienia
	QString title("Próba zamknięcia połączenia z bazą danych");
	QString text("Czy chcesz zapisać zmiany w bazie danych przed zamknięciem połączenia?");

	int result = QMessageBox::question(
		this, title, text,
		QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
		QMessageBox::Yes
	);

	if (result == QMessageBox::Cancel)
		return;

	int saveStatus = (result == QMessageBox::Yes);
	this->databases[tabName]->SetSaveStatus(saveStatus);

	// Uzyskanie wskaźnika na zakładkę, z której chcemy usunąć zaznaczenie
	QWidget* tabWidget = this->dbTabs->widget(index);
	if (tabWidget)
	{
		// Wyszukiwanie widoku QTableView w zakładce
		for(QTableView* tableView : tabWidget->findChildren<QTableView*>())
		if (tableView)
		{
			// Usunięcie zaznaczenia (odznaczenie komórek)
			QItemSelectionModel* selectionModel = tableView->selectionModel();
			if (selectionModel)
				selectionModel->clearSelection(); 
		}
	}

	// Usunięcie zakładki i zamknięcie połączenia z bazą danych
	this->dbTabs->removeTab(index);
	delete this->databases[tabName];
	this->databases.remove(tabName);
}

void MainWindow::onTableRemoveRequest(int index)
{
	// Pobranie elementu który wysłał sygnał
	QTabWidget* senderTabs = qobject_cast<QTabWidget*>(sender());

	// Blokowanie opcji zamknięcia zakładki z kreatorem nowej tabeli
	if (index == senderTabs->count() - 1)
		return;
	
	// Wyświetlenie ostrzeżenia o usuwaniu tabeli
	QString title("Potwierdzenie usunięcia tabeli");
	QString text("Czy na pewno chcesz usunąć tabelę wraz ze wszystkimi danymi w niej zawartymi? Operacja jest nieodwracalna i wszystkie dane zostaną trwale utracone");
	int result = QMessageBox::warning(
		this,
		title,
		text,
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No
	);

	if (result == QMessageBox::No)
		return;

	// Pobranie nazwy tabeli do usunięcia
	QString tableName = senderTabs->tabText(index);

	// Pobranie nazwy bazy danych z której będziemy usuwać tabele
	int currentActiveDatabaseIndex = this->dbTabs->currentIndex();
	QString dbName = this->dbTabs->tabText(currentActiveDatabaseIndex);
	
	// Wykonanie usuwania tabeli na bazie danych
	int dropResult = this->databases[dbName]->DropTable(tableName);

	if (dropResult == false)
		return;

	// Usunięcie zakładki w przypadku powodzenia 
	senderTabs->removeTab(index);
}
