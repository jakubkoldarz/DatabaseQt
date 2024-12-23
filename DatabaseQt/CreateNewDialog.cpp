#include "CreateNewDialog.h"

CreateNewDialog::CreateNewDialog(QWidget* parent)
	: QDialog(parent), newName(new QLineEdit(this))
{
	// Ustawienie tytułu
	this->setWindowTitle("Tworzenie nowej bazy");

	// Ustawienia rozmiaru okienka dialogowego
	this->setMinimumSize(300, 100);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QWidget* spinnerPanel = new QWidget(this);
	QHBoxLayout* spinnerPanelLayout = new QHBoxLayout(spinnerPanel);

	spinnerPanelLayout->addWidget(new QLabel("Nazwa nowej bazy danych:"));
	spinnerPanelLayout->addWidget(this->newName);
	spinnerPanelLayout->addWidget(new QLabel(".db"));

	spinnerPanel->setLayout(spinnerPanelLayout);

	layout->addWidget(spinnerPanel);

	// Dodanie przycisku do akceptacji
	QPushButton* acceptButton = new QPushButton("Wstaw wiersze", this);
	layout->addWidget(acceptButton);

	this->setLayout(layout);

	connect(acceptButton, &QPushButton::clicked, this, &QDialog::accept);
}

QString CreateNewDialog::GetNewName() const
{
	return this->newName->text();
}
