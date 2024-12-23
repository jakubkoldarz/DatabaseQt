#include "InsertRowsDialog.h"

AddRowsDialog::AddRowsDialog(QWidget* parent)
	: QDialog(parent), spinBox(new QSpinBox(this))
{
	// Ustawienie tytułu
	this->setWindowTitle("Wstawianie wierszy");

	// Ustawienia rozmiaru okienka dialogowego
	this->setMinimumSize(300, 100);

	// Ustawienia zakresu ilości wierszy
	this->spinBox->setMinimum(1);
	this->spinBox->setMaximum(50);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QWidget* spinnerPanel = new QWidget(this);
	QHBoxLayout* spinnerPanelLayout = new QHBoxLayout(spinnerPanel);

	spinnerPanelLayout->addWidget(new QLabel("Liczba wierszy do wstawienia:"));
	spinnerPanelLayout->addWidget(this->spinBox);

	spinnerPanel->setLayout(spinnerPanelLayout);

	layout->addWidget(spinnerPanel);

	// Dodanie przycisku do akceptacji
	QPushButton* acceptButton = new QPushButton("Wstaw wiersze", this);
	layout->addWidget(acceptButton);

	this->setLayout(layout);

	connect(acceptButton, &QPushButton::clicked, this, &QDialog::accept);
}

int AddRowsDialog::GetNumberOfRows() const
{
	return this->spinBox->value();
}
