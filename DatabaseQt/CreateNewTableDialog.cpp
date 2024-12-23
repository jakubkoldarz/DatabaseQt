#include "CreateNewTableDialog.h"

void CreateNewTableDialog::removeRows(const QModelIndexList& selectedRows)
{
	// Tworzymy listę indeksów wierszy
	QVector<int> rows;
	for (const QModelIndex& index : selectedRows)
		rows.append(index.row());

	// Sortujemy wiersze w kolejności malejącej
	std::sort(rows.begin(), rows.end(), std::greater<int>());

	// Usuwamy wiersze
	for (int row : rows)
		this->table->removeRow(row);
}

void CreateNewTableDialog::insertRows(int count)
{
	bool wasEmpty = false;
	if (this->table->rowCount() == 0)
		wasEmpty = true;

	for (int i = 0; i < count; i++)
	{
		int lastIndex = this->table->rowCount() == 0 ? 0 : this->table->rowCount();
		this->table->insertRow(lastIndex);
	}

	this->updateRows();

	if (wasEmpty)
		this->table->findChild<QCheckBox*>()->setChecked(true);
}

void CreateNewTableDialog::updateRows()
{
	// Dodawanie elementów do odpowiednich kolumn
	for (int row = 0; row < this->table->rowCount(); row++)
	{
		// Sprawdzenie czy to nowy wiersz
		if (this->table->cellWidget(row, 2) != nullptr)
			continue;

		// Dodawanie checkboxów
		QWidget* checkboxWidget = new QWidget(this->table);
		QHBoxLayout* checkBoxLayout = new QHBoxLayout(checkboxWidget);
		checkBoxLayout->setAlignment(Qt::AlignCenter);

		QCheckBox* checkBox = new QCheckBox(checkboxWidget);
		checkBox->setChecked(false);

		checkBoxLayout->setContentsMargins(0, 0, 0, 0);
		checkBoxLayout->addWidget(checkBox);

		this->table->setCellWidget(row, 2, checkboxWidget);
		this->checkboxes->addButton(checkBox);

		// Dodawanie comboboxów
		QComboBox* combobox = new QComboBox(this->table);
		combobox->addItem("INTEGER");
		combobox->addItem("TEXT");

		this->table->setCellWidget(row, 1, combobox);
	}
}

CreateNewTableDialog::CreateNewTableDialog(QWidget* parent)
	: QDialog(parent), checkboxes(new QButtonGroup(this))
{
	// Ustawienia okna
	this->setWindowTitle("Kreator tabel");
	this->setFixedSize(400, 600);

	this->checkboxes->setExclusive(true);

	// Przycisk potwierdzenia utworzenia tabeli
	QPushButton* acceptButton = new QPushButton("Gotowe", this);
	acceptButton->setStyleSheet("padding: 11px 0;");
	connect(acceptButton, &QPushButton::clicked, this, &QDialog::accept);

	// Układ dla całego okna dialogowego
	QVBoxLayout* layout = new QVBoxLayout(this);
	this->setLayout(layout);

	// Konfiguracja tabeli do wpisywania danych
	this->table = new QTableWidget(0, 3, this);
	QStringList columnTitles;
	columnTitles << "Nazwa" << "Typ" << "Klucz główny?";
	this->table->setHorizontalHeaderLabels(columnTitles);
	this->table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	this->table->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->table, &QTableView::customContextMenuRequested, this, &CreateNewTableDialog::onInsertRowRequest);

	this->insertRows(3);

	this->newTableName = new QLineEdit(this);
	this->newTableName->setPlaceholderText("Wpisz nazwę nowej tabeli");

	layout->addWidget(this->newTableName);
	layout->addWidget(this->table);
	layout->addWidget(acceptButton);
}

QString CreateNewTableDialog::GetQueryString() const
{
	if (this->table->rowCount() == 0)
		return NULL;
	
	QString queryString("CREATE TABLE IF NOT EXISTS %1 (");
	queryString = queryString.arg(this->newTableName->text());

	for (int row = 0; row < this->table->rowCount(); row++)
	{
		QString singleRowQuery("%1 %2 %3%4");
		
		// Pobranie nazwy
		QTableWidgetItem* itemName = this->table->item(row, 0);
		if (itemName == nullptr)
		{
			QMessageBox::critical(
				nullptr,
				"Wystąpił błąd",
				"Nazwa kolumny w tabeli nie może być pusta"
			);
			return NULL;
		}

		singleRowQuery = singleRowQuery.arg(itemName->text());

		// Pobranie typu
		QComboBox* itemType = qobject_cast<QComboBox*>(this->table->cellWidget(row, 1));
		singleRowQuery = singleRowQuery.arg(itemType->currentText());

		// Pobranie stanu checkboxa
		QWidget* itemPrimaryKeyWidget = this->table->cellWidget(row, 2);

		QCheckBox* itemPrimaryKey = itemPrimaryKeyWidget ? itemPrimaryKeyWidget->findChild<QCheckBox*>() : nullptr;

		if (itemPrimaryKey != nullptr && itemPrimaryKey->isChecked())
			singleRowQuery = singleRowQuery.arg("PRIMARY KEY AUTOINCREMENT");
		else
			singleRowQuery = singleRowQuery.arg("");

		bool isLast = (row == (this->table->rowCount() - 1));
		singleRowQuery = singleRowQuery.arg(isLast ? "" : ", ");

		queryString += singleRowQuery;
	}

	queryString += ")";

	return queryString;
}

void CreateNewTableDialog::onInsertRowRequest(const QPoint& pos)
{
	// Utworzenie menu konekstowego
	QMenu contextMenu(this);

	QAction* deleteAction = contextMenu.addAction("Usuń zaznaczone wiersze");
	QAction* insertRowsAction = contextMenu.addAction("Wstaw wiersze");
	QAction* selectedAction = contextMenu.exec(this->table->viewport()->mapToGlobal(pos));

	if (selectedAction == deleteAction)
	{
		// Pobranie zaznaczonych wierszów
		QModelIndexList selectedRows = this->table->selectionModel()->selectedRows();

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
		this->removeRows(selectedRows);

		// Odświeżenie widoku tabeli
		this->table->viewport()->update();
	}
	else if (selectedAction == insertRowsAction)
	{
		AddRowsDialog dialog(this);
		if (dialog.exec() == QDialog::Accepted) {
			int rowsToAdd = dialog.GetNumberOfRows();
			this->insertRows(rowsToAdd);
		}
	}
}