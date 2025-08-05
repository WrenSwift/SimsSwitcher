#include "mcccSettings.h"
#include "ui_mcccSettings.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QString>
#include <QDirIterator>


MCCCSettings::MCCCSettings(QWidget *parent) :
    QWizard(parent),
    ui(new Ui::MCCCSettings)
{
    ui->setupUi(this);

    populatePresetList();
    // Set the window title
    setWindowTitle("MCCC Settings");
}

void MCCCSettings::populatePresetList()
{
    ui->mcccPresetList->clear();

    MainWindow *mainWindowPointer = qobject_cast<MainWindow *>(parentWidget());
    if (!mainWindowPointer) {
        QMessageBox::warning(this, "Error", "Main window pointer is invalid.");
        return;
    }
    QString rootdir = mainWindowPointer->getRootDir();
    QDir mcccPresetsDir(rootdir + "/mcccPresets");
    if (!mcccPresetsDir.exists()) {
        // Directory does not exist, nothing to populate
        return;
    }

    QStringList filter;
    filter << "*.cfg";
    QFileInfoList fileList = mcccPresetsDir.entryInfoList(filter, QDir::Files);
    for (const QFileInfo &fileInfo : fileList) {
        QString fileName = fileInfo.completeBaseName(); // Get filename without extension
        ui->mcccPresetList->addItem(fileName);
    }
}

// On save button clicked, save MCCC settings as the name from settingsName LineEdit in the mcccPresets directory creating the directory if it does not exist.
void MCCCSettings::on_mcccSaveButton_clicked()
{
    QString settingsName = ui->settingsName->text().trimmed();
    if (settingsName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Settings name cannot be empty.");
        return;
    }

    MainWindow *mainWindowPointer = qobject_cast<MainWindow *>(parentWidget());
    if (!mainWindowPointer) {
        QMessageBox::warning(this, "Error", "Main window pointer is invalid.");
        return;
    }
    // Ensure you are not referencing ui->rootLineEdit here, use mainWindowPointer->getRootDir() as before
    QString rootdir = mainWindowPointer->getRootDir();
    QString mcccPresetsPath = rootdir + "/mcccPresets";
    QDir mcccPresetsDir(mcccPresetsPath);
    if (!mcccPresetsDir.exists()) {
        if (!QDir().mkpath(mcccPresetsPath)) {
            QString errorMessage = QString("Failed to create mcccPresets directory at %1.").arg(mcccPresetsPath);
            QMessageBox::warning(this, "Error", errorMessage);
            return;
        }
    }

    QString activeModsPath = rootdir + "/Mods";
    QString mcccPack = "mc_cmd_center.package"; // Assuming this is the MCCC settings file name
    QString mcccSetName = "mc_settings.cfg"; // The name to use in the Mods directory
    QString mcccDir = nullptr;

    if (QFile::exists(activeModsPath + "/" + mcccPack)) {
        QString mcccDir = activeModsPath;
    }else {
        QDirIterator it(activeModsPath, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            QString subDirPath = it.next();
            if (QFile::exists(subDirPath + "/" + mcccPack)) {
                QString mcccDir = subDirPath;
                break;
            }
        }
    }

    if (mcccDir == nullptr) {
        QMessageBox::warning(this, "Error", "MCCC settings file not found in Mods.");
        return;
    }

    QString sourceFilePath = mcccDir + "/" + mcccSetName;
    QString destFilePath = mcccPresetsDir.filePath(settingsName + ".cfg");
    if (QFile::exists(destFilePath)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Overwrite Confirmation",
                                      "A preset with this name already exists. Do you want to overwrite it?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return; // User chose not to overwrite
        }
        if (!QFile::remove(destFilePath)) {
            QMessageBox::warning(this, "Error", "Failed to remove existing preset file.");
            return;
        }
    }

    if (QFile::exists(sourceFilePath)) {
        if (QFile::copy(sourceFilePath, destFilePath)) {
            QMessageBox::information(this, "Success", "Settings copied successfully.");
        } else {
            QMessageBox::warning(this, "Error", "Failed to copy settings file.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Source settings file does not exist.");
    }
    populatePresetList();
}

// On load button clicked, load MCCC settings from the selected item in the mcccPresetList moving it to active mods and renaming it to mc_settings.cfg.
void MCCCSettings::on_mcccLoadButton_clicked()
{
    QListWidgetItem *selectedItem = ui->mcccPresetList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Error", "No preset selected.");
        return;
    }

    QString settingsName = selectedItem->text().trimmed();
    if (settingsName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected preset name is empty.");
        return;
    }

    MainWindow *mainWindowPointer = qobject_cast<MainWindow *>(parentWidget());
    if (!mainWindowPointer) {
        QMessageBox::warning(this, "Error", "Main window pointer is invalid.");
        return;
    }
    QString rootdir = mainWindowPointer->getRootDir();
    QDir mcccPresetsDir(rootdir + "/mcccPresets");
    QString sourceFilePath = mcccPresetsDir.filePath(settingsName + ".cfg");
    QString activeModsPath = rootdir + "/Mods";
    QString mcccPack = "mc_cmd_center.package"; // Assuming this is the MCCC settings
    QString mcccSetName = "mc_settings.cfg"; // The name to use in the Mods directory
    QString mcccDir = nullptr;
    if (QFile::exists(activeModsPath + "/" + mcccPack)) {
        mcccDir = activeModsPath;
    }else {
        QDirIterator it(activeModsPath, QDir::Dirs | QDir::NoDotAndDotDot);
        while (it.hasNext()) {
            QString subDirPath = it.next();
            if (QFile::exists(subDirPath + "/" + mcccPack)) {
                mcccDir = subDirPath;
                break;
            }
        }
    }


    QString destFilePath = mcccDir + "/" + mcccSetName;

    if (QFile::exists(sourceFilePath)) {
        // Remove existing mc_settings.cfg if it exists and warn the user that it will be overwritten with prompt to continue or cancel.
        if (QFile::exists(destFilePath)) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::question(this, "Overwrite Confirmation", "mc_settings.cfg already exists in Mods. Do you want to overwrite it?",
                          QMessageBox::Yes|QMessageBox::No);
            if (reply == QMessageBox::No) {
            return; // User chose not to overwrite, exit the function.
            }
            if (!QFile::remove(destFilePath)) {
            QMessageBox::warning(this, "Error", "Failed to remove existing mc_settings.cfg.");
            return;
            }
        }
        // Only copy after user confirms or if file doesn't exist
        if (QFile::copy(sourceFilePath, destFilePath)) {
            QMessageBox::information(this, "Success", "Settings loaded successfully.");
            ui->settingsName->setText(settingsName); // Set the settings name to the loaded preset name
        } else {
            QMessageBox::warning(this, "Error", "Failed to copy settings file.");
        }
    } else {
        QMessageBox::warning(this, "Error", "Source settings file does not exist.");
    }
}

void MCCCSettings::on_mcccDeleteButton_clicked()
{
    QListWidgetItem *selectedItem = ui->mcccPresetList->currentItem();
    if (!selectedItem) {
        QMessageBox::warning(this, "Error", "No preset selected.");
        return;
    }

    QString settingsName = selectedItem->text().trimmed();
    if (settingsName.isEmpty()) {
        QMessageBox::warning(this, "Error", "Selected preset name is empty.");
        return;
    }

    MainWindow *mainWindowPointer = qobject_cast<MainWindow *>(parentWidget());
    if (!mainWindowPointer) {
        QMessageBox::warning(this, "Error", "Main window pointer is invalid.");
        return;
    }
    QString rootdir = mainWindowPointer->getRootDir();
    QDir mcccPresetsDir(rootdir + "/mcccPresets");
    QString filePath = mcccPresetsDir.filePath(settingsName + ".cfg");

    if (QFile::exists(filePath)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Delete Confirmation",
                                     "Are you sure you want to delete this preset?",
                                     QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            if (QFile::remove(filePath)) {
                QMessageBox::information(this, "Success", "Preset deleted successfully.");
                populatePresetList(); // Refresh the list after deletion
            } else {
                QMessageBox::warning(this, "Error", "Failed to delete preset file.");
            }
        }
        // If No, do nothing
    } else {
        QMessageBox::warning(this, "Error", "Preset file does not exist.");
    }
}

MCCCSettings::~MCCCSettings()
{
    delete ui;
}