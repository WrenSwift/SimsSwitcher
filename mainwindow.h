#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

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
    void on_modsSaveButton_clicked();
    void on_modsLoadButton_clicked();
    void onPresetSelected(QListWidgetItem* item);
    void on_presetDeleteButton_clicked();
    void on_launchSaveButton_clicked();
    void on_packsLoadButton_clicked();
    void on_presetPackDeleteButton_clicked();
    void onPackPresetSelected(QListWidgetItem* item);
    void on_menuSettings_clicked();

private:
    Ui::SimsSwitcher *ui;
    void populateFileList(const QString& firstDir, const QString& secondDir);
    bool copyDirectory(const QString& sourcePath, const QString& destPath);
    void populatePacksListWidget(const QString &folderPath, const QString &csvFilePath);
    QString getDisablePacksStringFromListWidget();
    void savePacksSelection();
    void sortPacksListByCategory();
    QStringList presetList();
    void savePreset(const QString& presetName);
    void loadPreset(const QString& presetName);
    void updatePresetList();
    QStringList packPresetList();
    void savePackPreset(const QString& presetName);
    void loadPackPreset(const QString& presetName);
    void updatePackPresetList();
};
#endif // MAINWINDOW_H
