#ifndef MCCCSETTINGS_H
#define MCCCSETTINGS_H

#include <QWizard>

namespace Ui {
class MCCCSettings;
}
class MCCCSettings : public QWizard
{
    Q_OBJECT
public:
    explicit MCCCSettings(QWidget *parent = nullptr);
    ~MCCCSettings();

private slots:
    void on_mcccSaveButton_clicked();
    void on_mcccLoadButton_clicked();
    void on_mcccDeleteButton_clicked();

private:
    Ui::MCCCSettings *ui;
    void populatePresetList();
};

#endif // MCCCSETTINGS_H