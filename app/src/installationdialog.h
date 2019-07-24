#pragma once
#include <QDialog>
#include <memory>

#include "ui_installationdialog.h"

class InstallationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InstallationDialog(const QString& defaultLocation, QWidget* parent = nullptr);

    bool shouldCancel() const;
    QString selectedInstallationLocation() const;

private:
    std::unique_ptr<Ui::Dialog> m_ui;
    bool wasCancelled;

    void selectInstallationLocation();

private slots:
    void onRejected();
    void onAccepted();

signals:
    void onEnd();
};
