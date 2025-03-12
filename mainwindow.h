#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class SimsSwitcher;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_browseRootButton_clicked();
    void on_activeButton_clicked();
    void on_menuPacks_clicked();
    void on_menuMods_clicked();
    void on_browseGameButton_clicked();
    void on_launchButton_clicked();
    void on_fileSelectAllButton_clicked();
    void on_fileDeselectAllButton_clicked();
    void on_packsSelectAllButton_clicked();
    void on_packsDeselectAllButton_clicked();

private:
    Ui::SimsSwitcher *ui;
    void populateFileList(const QString& firstDir, const QString& secondDir);
    bool copyDirectory(const QString& sourcePath, const QString& destPath);
    void populatePacksListWidget(const QString &folderPath, const QString &csvFilePath);
    QString getDisablePacksStringFromListWidget();
    void savePacksSelection();
    void sortPacksListByCategory();


};
#endif // MAINWINDOW_H
