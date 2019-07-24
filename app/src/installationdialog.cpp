#include "installationdialog.h"
#include "ui_installationdialog.h"

#include <QFileDialog>

InstallationDialog::InstallationDialog(const QString& defaultLocation, QWidget* parent)
    : QDialog(parent)
    , m_ui(new Ui::Dialog)
    , wasCancelled(false)
{
    m_ui->setupUi(this);

    m_ui->installationLocation->setText(defaultLocation);

    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    connect(m_ui->buttonBox, &QDialogButtonBox::accepted,
            this, &InstallationDialog::onAccepted);
    connect(m_ui->buttonBox, &QDialogButtonBox::rejected,
            this, &InstallationDialog::onRejected);
    connect(m_ui->browseButton, &QPushButton::clicked,
            this, &InstallationDialog::selectInstallationLocation);
}

bool InstallationDialog::shouldCancel() const
{
    return this->wasCancelled;
}

QString InstallationDialog::selectedInstallationLocation() const
{
    return this->m_ui->installationLocation->text();
}

void InstallationDialog::selectInstallationLocation()
{
    auto currentPath = m_ui->installationLocation->text();
    QString selectedPath = QFileDialog::getExistingDirectory(this, "Select installation location", currentPath, QFileDialog::ShowDirsOnly);

    m_ui->installationLocation->setText(selectedPath);
}

void InstallationDialog::onRejected()
{
    this->wasCancelled = true;
    emit onEnd();
}

void InstallationDialog::onAccepted()
{
    this->wasCancelled = false;
    emit onEnd();
}
