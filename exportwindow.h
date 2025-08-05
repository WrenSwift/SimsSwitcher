#ifndef EXPORTWINDOW_H
#define EXPORTWINDOW_H

#include <QWizard>

namespace Ui {
class SimsSwitcherExport;
}

class ExportWindow : public QWizard
{
    Q_OBJECT

public:
    explicit ExportWindow(QWidget *parent = nullptr);
    ~ExportWindow();

private slots:
    void on_browseButton_clicked();
    void on_exportButton_clicked();

private:
    void loadMCCCPresets();
    void loadPresets();
    Ui::SimsSwitcherExport *ui;
};

#endif // EXPORTWINDOW_H