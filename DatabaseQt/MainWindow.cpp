#include "MainWindow.h"

void MainWindow::onLoadButtonClicked()
{
	// Pobranie ścieżki do pulpitu użytkownika
	// Głównie w celu wygody wybierania baz danych
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);

	// Przypisanie opisu okienka oraz filtrów do plików
	QString caption = "Otwórz plik";
	QString filters = "Baza danych (*.db)";

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

	// Aktualizacja ikonki
	this->updateIcon();
	this->updateSearchbar();
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
	connect(tablesTabs, &QTabWidget::currentChanged, this, &MainWindow::onChangeTab);

	// Ustawienia dla menu kontekstowego
	tablesTabs->setContextMenuPolicy(Qt::NoContextMenu);
	connect(tablesTabs, &QTableView::customContextMenuRequested, this, &MainWindow::onTableContextMenu);

	for (int i = 0; i < tablesCount; i++)
	{
		// Utworzenie zakładki
		QWidget* singleTab = new QWidget(tablesTabs);

		// Utworzenie układu dla zakładki
		QVBoxLayout* layoutTemp = new QVBoxLayout(singleTab);

		// Utworzenie tabeli z danymi z bazy danych
		QTableView* singleTable = new QTableView(tablesTabs);
		singleTable->setModel(this->databases[tabName]->GetTableModel(tables[i]));
		singleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		singleTable->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(singleTable, &QTableView::customContextMenuRequested, this, &MainWindow::onTableContextMenu);

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

	// Utworzenie przycisku do otwierania kreatora tabel
	QPushButton* openTableCreatorButton = new QPushButton("Przejdź do kreatora tabel", newTableTab);
	openTableCreatorButton->setStyleSheet("padding: 11px 0px; width: 300px;");
	connect(openTableCreatorButton, &QPushButton::clicked, this, &MainWindow::onTableCreatorRequest);
	
	// Dodanie przycisku do układu
	newTableTabLayout->addWidget(openTableCreatorButton);
	newTableTabLayout->setAlignment(Qt::AlignCenter);

	// Przypisanie układu do zakładki
	newTableTab->setLayout(newTableTabLayout);

	// Dodanie zakładki do listy wszystkich zakładek
	tablesTabs->addTab(newTableTab, "Nowa tabela");

	// Dodanie zakładek z nazwami tabel do głownej zakładki bazy danych
	this->dbTabs->insertTab(0, tablesTabs, tabName);
	this->dbTabs->setCurrentIndex(0);
}

QString MainWindow::activeDatabaseName()
{
	int activeIndex = this->dbTabs->currentIndex();
	return QString(this->dbTabs->tabText(activeIndex));
}

QString MainWindow::activeTableName()
{
	int activeIndexDB = this->dbTabs->currentIndex();
	QWidget* activeWidget = this->dbTabs->widget(activeIndexDB);
	QTabWidget* activeTabWidged = qobject_cast<QTabWidget*>(activeWidget);
	int activeIndexTable = activeTabWidged->currentIndex();

	return QString(activeTabWidged->tabText(activeIndexTable));
}

void MainWindow::updateIcon()
{
	if(this->databases.size() > 0)
		this->setWindowIcon(QIcon(":/new/icons/dbOnline.png"));
	else
		this->setWindowIcon(QIcon(":/new/icons/dbOffline.png"));
}

void MainWindow::updateSearchbar()
{
	QTabWidget* tableTabs = qobject_cast<QTabWidget*>(this->dbTabs->currentWidget());

	if (this->searchbarText == nullptr || this->searchbarColumns == nullptr)
		return;

	// Reset paska wyszukiwania
	this->searchbarText->setText("");

	// Reset wyboru kolumny
	this->searchbarColumns->clear();
	this->searchbarColumns->addItem("Nazwa kolumny");
	this->searchbarColumns->setCurrentIndex(0);

	if (this->dbTabs->count() <= 1 || tableTabs == nullptr || tableTabs->count() <= 1 || tableTabs->currentIndex() == tableTabs->count() - 1)
	{
		// Blokowanie paska wyszukiwania
		this->searchbarText->setDisabled(true);
		this->searchbarColumns->setDisabled(true);
		this->searchbarButton->setDisabled(true);
	}
	else
	{
		// Aktywacja wszyskich pól z paska wyszukiwania
		this->searchbarText->setDisabled(false);
		this->searchbarColumns->setDisabled(false);
		this->searchbarButton->setDisabled(false);

		// Pobranie nazw kolumn z aktywnej tabeli
		QWidget* currentWidget = tableTabs->currentWidget();
		QTableView* currentTable = currentWidget->findChild<QTableView*>();
		
		// Gdy użytkownik przełącza na baze danych w której aktualna zakładka to kreator tabel
		if (currentTable == nullptr)
			return;

		// Dodanie do selecta nazw kolumn
		for (int i = 0; i < currentTable->model()->columnCount(); i++)
		{
			QString colName = currentTable->model()->headerData(i, Qt::Horizontal).toString();
			this->searchbarColumns->addItem(colName);
		}
	}
}

MainWindow::MainWindow(QWidget* parent)
{
	// Ustawienie ikonki dla aplikacji
	this->updateIcon();

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
	connect(buttonNew, &QPushButton::clicked, this, &MainWindow::onNewButtonClicked);

	// Utworzenie kontenera dla zakładek z baz danych
	this->dbTabs = new QTabWidget(this);

	// Ustawienie możliwości zamykania zakładek
	this->dbTabs->setTabsClosable(true);
	connect(this->dbTabs, &QTabWidget::currentChanged, this, &MainWindow::onChangeTab);
	
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

	// Ustawienia searchbaru
	QWidget* searchbarContainer = new QWidget(this);
	searchbarContainer->setStyleSheet("margin-bottom: 10px;");
	QHBoxLayout* searchbarContainerLayout = new QHBoxLayout(searchbarContainer);
	searchbarContainer->setLayout(searchbarContainerLayout);

	this->searchbarText = new QLineEdit(this);
	this->searchbarText->setPlaceholderText("Wyszukaj w tabeli...");
	this->searchbarText->setStyleSheet("padding: 5px 8px; height: 20px;");

	this->searchbarButton = new QPushButton("Wyszukaj", searchbarContainer);
	this->searchbarButton->setStyleSheet("padding: 8px 8px; width: 200px; height: 20px;");

	connect(this->searchbarButton, &QPushButton::clicked, this, &MainWindow::onSearchButtonClicked);

	this->searchbarColumns = new QComboBox(searchbarContainer);
	this->searchbarColumns->setStyleSheet("padding: 8px; height: 20px;");
	this->searchbarColumns->addItem("Nazwa kolumny");
	this->searchbarColumns->setCurrentIndex(0);

	searchbarContainerLayout->addWidget(this->searchbarText);
	searchbarContainerLayout->addWidget(this->searchbarColumns);
	searchbarContainerLayout->addWidget(this->searchbarButton);

	// Odświeżenie searchbaru
	this->updateSearchbar();

	// Dodanie searchbaru do głównego układu
	centralLayout->addWidget(searchbarContainer, 0, 0);

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

void MainWindow::onNewButtonClicked()
{
	// Wyświetlenie okienka do pobrania nazwy nowej bazy danych
	CreateNewDialog dialog(this);
	QString newDatabaseName;
	if (dialog.exec() == QDialog::Accepted)
		newDatabaseName = dialog.GetNewName();

	if (newDatabaseName.isEmpty())
	{
		QMessageBox::critical(
			this,
			"Wystąpił błąd",
			"Wystąpił błąd podczas dodawania nowej bazy danych. Podano pusty ciąg znaków. Proszę wpisać poprawny ciąg znaków"
		);

		return;
	}

	// Sprawdzenie czy nazwa nie zawiera żadnych niepoprawnych znaków
	QRegularExpression regex("^[a-zA-Z0-9]+$");
	int matchResult = regex.match(newDatabaseName).hasMatch();
	if (!matchResult)
	{
		QMessageBox::critical(
			this,
			"Wystąpił błąd",
			"Wystąpił błąd podczas dodawania nowej bazy danych. Nazwa nowej bazy danych może zawierać tylko małe lub duże litery bez polskich znaków."
		);
		return;
	}

	// Utworzenie ścieżki do nowej bazy danych, (na pulpit)
	QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
	QString newDatabasePath = QString("%1/%2.db").arg(desktopPath).arg(newDatabaseName);

	// Sprawdzenie czy przypadkiem nie istnieje już taki plik na pulpicie lub w aktualnie wczytanych bazach danych
	QFileInfo fileInfo(newDatabasePath);
	if (fileInfo.exists() || this->databases.contains(QString("%1.db").arg(newDatabaseName)))
	{
		QMessageBox::warning(
			this,
			"Wystąpił błąd",
			"Baza danych z podaną nazwą już istnieje. Spróbuj użyć innej nazwy dla nowej bazy danych."
		);
		return;
	}

	// Utworznie nowego połączenia
	this->databases[fileInfo.fileName()] = new DBController(newDatabasePath, true);

	// Wczytanie tabel do zakładek
	this->setupTabs(fileInfo.fileName());

	// Aktualizacja ikonki i paska wyszukiwania
	this->updateIcon();

	this->updateSearchbar();
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
	QString text("Czy chcesz zapisać zmiany w bazie danych przed zamknięciem połączenia? Jeśli nie zapiszesz zmian, wszystkie niezapisane dane zostaną utracone po zamknięciu połączenia z bazą. Możesz również anulować zamknięcie, aby dokończyć wprowadzanie zmian. Upewnij się, że nie stracisz ważnych danych.");
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

	// Aktualizacja ikonki aplikacji i paska wyszukiwania
	this->updateIcon();
	this->updateSearchbar();
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

	// Wykonanie usuwania tabeli na bazie danych
	int dropResult = this->databases[activeDatabaseName()]->DropTable(tableName);

	if (dropResult == false)
	{
		QString errorMsg;
		QMessageBox::critical(
			this,
			"Operacja usuwania tabeli zakończona niepowodzeniem",
			"Podczas próby usunięcia tabeli wystąpił nieoczekiwany błąd. Może to wynikać z problemów z połączeniem z bazą danych, braku odpowiednich uprawnień lub innych nieprzewidzianych okoliczności."
		);
		return;
	}

	// Usunięcie zakładki w przypadku powodzenia 
	senderTabs->removeTab(index);
}

void MainWindow::onTableContextMenu(const QPoint& pos)
{
	// Pobranie widoku tabeli, który wywołał menu kontekstowe
	QTableView* senderTableView = qobject_cast<QTableView*>(sender());
	
	// Pobranie modelu tabeli
	QSqlTableModel* model = qobject_cast<QSqlTableModel*>(senderTableView->model());

	// Utworzenie menu konekstowego
	QMenu contextMenu(this);
	QAction* deleteAction = contextMenu.addAction("Usuń zaznaczone wiersze");
	QAction* insertRowsAction = contextMenu.addAction("Wstaw wiersze");
	QAction* saveAction = contextMenu.addAction("Zapisz rekordy");
	QAction* selectedAction = contextMenu.exec(senderTableView->viewport()->mapToGlobal(pos));

	if (selectedAction == deleteAction)
	{
		// Pobranie zaznaczonych wierszów
		QModelIndexList selectedRows = senderTableView->selectionModel()->selectedRows();

		// Zakończenie usuwania jeśli nie wybrano żadnego wiersza
		if (selectedRows.isEmpty())
		{
			QString title("Brak zaznaczenia");
			QString txt("Nie wybrano żadnego wiersza do usunięcia. Pamiętaj, że należy wybrać cały wiersz do usunięcia a nie tylko poszczególne komórki.");
			QMessageBox::information(this, title, txt, QMessageBox::Ok);
			return;
		}

		// Potwierdzenie usunięcia
		int result = QMessageBox::question(
			this,
			"Potwierdzenie usunięcia wierszy",
			"Czy na pewno chcesz usunąć wszystkie wybrane wiersze?",
			QMessageBox::Yes | QMessageBox::No,
			QMessageBox::Yes
		);

		// Zatrzymanie usuwania
		if (result == QMessageBox::No)
			return;

		// Usunięcie wierszy
		QString tableName = activeTableName();
		this->databases[this->activeDatabaseName()]->RemoveRows(tableName, selectedRows);

		// Odświeżenie widoku tabeli
		senderTableView->viewport()->update();
	}
	else if (selectedAction == insertRowsAction)
	{
		AddRowsDialog dialog(this);
		if (dialog.exec() == QDialog::Accepted) 
		{
			int rowsToAdd = dialog.GetNumberOfRows();
			this->databases[activeDatabaseName()]->InsertRows(activeTableName(), 0, rowsToAdd);
		}
	}
	else if (selectedAction == saveAction)
	{
		bool saveResult = this->databases[activeDatabaseName()]->SaveTable(activeTableName());
		if (saveResult)
			QMessageBox::information(this, "Sukces", "Pomyślnie zapisano wszystkie zmiany w rekordach do tabeli w bazie danych.");
		else
			QMessageBox::critical(this, "Wystąpił błąd", "Wystąpił błąd krytyczny podczas próby zapisu danych do bazy danych.");
	}
}

void MainWindow::onTableCreatorRequest()
{
	CreateNewTableDialog dialogC(this);
	if (dialogC.exec() == QDialog::Accepted)
	{
		QStringList queryData = dialogC.GetQueryString();

		if (queryData.isEmpty())
		{
			QMessageBox::critical(this, "Wystąpił błąd", "Nie udało się wygenerować polecenia dla bazy danych.");
			return;
		}

		bool queryStatus = this->databases[activeDatabaseName()]->Query(queryData[1]);
		if (queryStatus == false)
		{
			QMessageBox::critical(this, "Wystąpił błąd", "Nie udało się dodać tabeli do bazy danych. Sprawdź konsole po więcej informacji.");
			return;
		}

		// Odświeżenie zakładki po dodaniu tabeli
		QTabWidget* currentDB = qobject_cast<QTabWidget*>(this->dbTabs->currentWidget());
		
		// Utworzenie zakładki
		QWidget* singleTab = new QWidget(currentDB);

		// Utworzenie układu dla zakładki
		QVBoxLayout* layoutTemp = new QVBoxLayout(singleTab);

		// Utworzenie tabeli z danymi z bazy danych
		QTableView* singleTable = new QTableView(currentDB);
		singleTable->setModel(this->databases[activeDatabaseName()]->GetTableModel(queryData[0]));
		singleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

		singleTable->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(singleTable, &QTableView::customContextMenuRequested, this, &MainWindow::onTableContextMenu);

		// Dodanie tabeli do układu 
		layoutTemp->addWidget(singleTable);

		// Przypisanie układu do zakładki
		singleTab->setLayout(layoutTemp);

		// Dodanie zakładki do listy wszystkich zakładek
		currentDB->insertTab(0, singleTab, queryData[0]);
		currentDB->setCurrentIndex(0);

		// Aktualizacja paska wyszukiwania
		this->updateSearchbar();
	}
}

void MainWindow::onSearchButtonClicked()
{
	QString tableName = activeTableName();
	QString columnName = this->searchbarColumns->currentText();
	QString params = this->searchbarText->text();

	if (this->searchbarColumns->currentIndex() == 0)
	{
		this->databases[activeDatabaseName()]->SelectAll(tableName);
		return;
	}

	this->databases[activeDatabaseName()]->FilterTable(tableName, columnName, params);
}

void MainWindow::onChangeTab()
{
	this->updateSearchbar();
}
