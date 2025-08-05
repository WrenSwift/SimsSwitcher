#include "exportwindow.h"
#include "ui_exportwindow.h" // This header defines Ui::ExportWindow
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMessageBox>
#include <QDirIterator>
#include <QStandardPaths>
#include <QTemporaryDir>
#include <QProcess>

ExportWindow::ExportWindow(QWidget *parent) : QWizard(parent), ui(new Ui::SimsSwitcherExport) {
    ui->setupUi(this);
    // Enable multi-selection for both lists
    ui->modPresetList->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->packPresetList->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->mcccPresetList->setSelectionMode(QAbstractItemView::MultiSelection);
    loadPresets();
    loadMCCCPresets();
}

ExportWindow::~ExportWindow() { delete ui; }

void ExportWindow::loadMCCCPresets() {
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

void ExportWindow::loadPresets() {
    // Load mod and pack presets from QSettings and populate the UI lists
    QSettings settings("Falcon", "SimsSwitcher");
    settings.beginGroup("presets");
    QStringList modPresets = settings.childKeys();
    settings.endGroup();
    settings.beginGroup("packPresets");
    QStringList packPresets = settings.childKeys();
    settings.endGroup();

    ui->modPresetList->addItems(modPresets);
    ui->packPresetList->addItems(packPresets);
}

void ExportWindow::on_browseButton_clicked() {
    QString dirPath = QFileDialog::getExistingDirectory(this, "Select Export Folder", QString(),
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dirPath.isEmpty())
        ui->fileLineEdit->setText(dirPath);
}

// Add this helper function for zipping a folder using Windows' built-in zip support (or use a 3rd party library if preferred)
bool zipDirectory(const QString& sourceDir, const QString& zipFilePath) {
    // Use PowerShell's Compress-Archive for simplicity (Windows 10+)
    QString srcCopy = sourceDir;
    QString zipCopy = zipFilePath;
    srcCopy.replace("'", "''");
    zipCopy.replace("'", "''");
    QString command = QString(
        "powershell -Command \"Compress-Archive -Path '%1\\*' -DestinationPath '%2' -Force\""
    ).arg(srcCopy, zipCopy);
    int result = QProcess::execute(command);
    return (result == 0);
}

void ExportWindow::on_exportButton_clicked() {
    QString dirPath = ui->fileLineEdit->text();
    if (dirPath.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select a folder to export to.");
        return;
    }

    QDir exportDir(dirPath);
    if (!exportDir.exists()) {
        if (!exportDir.mkpath(".")) {
            QMessageBox::warning(this, "Error", "Failed to create export directory.");
            return;
        }
    }

    QSettings settings("Falcon", "SimsSwitcher");
    int exportedCount = 0;

    // Gather all selected presets
    QList<QListWidgetItem*> modItems = ui->modPresetList->selectedItems();
    QList<QListWidgetItem*> packItems = ui->packPresetList->selectedItems();
    int totalToExport = modItems.size() + packItems.size();

    // If exporting mod zips, count those too
    bool exportZips = ui->exportModsZipOption->isChecked();
    if (exportZips)
        totalToExport += modItems.size();

    // Setup progress bar
    ui->exportProgressBar->setMinimum(0);
    ui->exportProgressBar->setMaximum(totalToExport);
    ui->exportProgressBar->setValue(0);

    int progress = 0;

    // Export selected mod presets as individual files
    for (QListWidgetItem *item : modItems) {
        QString key = item->text();
        QStringList values = settings.value("presets/" + key).toStringList();

        QJsonObject presetObj;
        presetObj.insert("type", "modPreset");
        presetObj.insert("name", key);
        presetObj.insert("items", QJsonArray::fromStringList(values));

        QString fileName = QString("modPreset_%1.json").arg(key);
        QFile file(exportDir.filePath(fileName));
        bool jsonExported = false;
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(presetObj).toJson());
            file.close();
            jsonExported = true;
        }
        ui->exportProgressBar->setValue(++progress);
        QCoreApplication::processEvents(); // Allow UI to update

        // --- Export ZIP of all mods in this preset if option is checked ---
        bool zipExported = false;
        if (exportZips) {
            // Find the Mods directory (assume it's stored in settings or ask user to select if not found)
            QString modsDir = settings.value("rootDirectory").toString();
            if (modsDir.isEmpty()) {
                QMessageBox::warning(this, "Error", "Mods directory not set in settings.");
                ui->exportProgressBar->setValue(++progress);
                QCoreApplication::processEvents();
                continue;
            }
            QDir modsBaseDir(modsDir + "/Mods");
            if (!modsBaseDir.exists()) {
                QMessageBox::warning(this, "Error", "Mods directory does not exist: " + modsBaseDir.absolutePath());
                ui->exportProgressBar->setValue(++progress);
                QCoreApplication::processEvents();
                continue;
            }

            // Create a temporary directory to collect the files/folders for this preset
            QTemporaryDir tempDir;
            if (!tempDir.isValid()) {
                QMessageBox::warning(this, "Error", "Failed to create temporary directory for zipping.");
                ui->exportProgressBar->setValue(++progress);
                QCoreApplication::processEvents();
                continue;
            }
            QString tempPresetDir = tempDir.path();

            // Copy each mod in the preset to the temp directory
            for (const QString& modName : values) {
                QString srcPath = modsBaseDir.filePath(modName);
                QFileInfo srcInfo(srcPath);
                if (!srcInfo.exists())
                    continue;
                QString destPath = tempPresetDir + "/" + modName;
                if (srcInfo.isDir()) {
                    QDir().mkpath(destPath);
                    QDir srcDir(srcPath);
                    QDirIterator it(srcPath, QDir::NoDotAndDotDot | QDir::AllEntries, QDirIterator::Subdirectories);
                    while (it.hasNext()) {
                        QString srcFile = it.next();
                        QString relPath = srcDir.relativeFilePath(srcFile);
                        QString destFile = destPath + "/" + relPath;
                        QFileInfo fi(srcFile);
                        if (fi.isDir()) {
                            QDir().mkpath(destFile);
                        } else {
                            QFile::copy(srcFile, destFile);
                        }
                    }
                } else if (srcInfo.isFile()) {
                    QFile::copy(srcPath, destPath);
                }
            }

            // Zip the temp directory
            QString zipName = QString("modPreset_%1.zip").arg(key);
            QString zipPath = exportDir.filePath(zipName);
            if (zipDirectory(tempPresetDir, zipPath)) {
                zipExported = true;
            } else {
                QMessageBox::warning(this, "Error", QString("Failed to create ZIP for preset '%1'.").arg(key));
            }
            ui->exportProgressBar->setValue(++progress);
            QCoreApplication::processEvents();
        }

        // Only count as exported if at least one (json or zip) was created
        if (jsonExported || zipExported)
            ++exportedCount;
    }

    // Export selected pack presets as individual files
    for (QListWidgetItem *item : packItems) {
        QString key = item->text();
        QStringList values = settings.value("packPresets/" + key).toStringList();

        QJsonObject presetObj;
        presetObj.insert("type", "packPreset");
        presetObj.insert("name", key);
        presetObj.insert("items", QJsonArray::fromStringList(values));

        QString fileName = QString("packPreset_%1.json").arg(key);
        QFile file(exportDir.filePath(fileName));
        if (file.open(QIODevice::WriteOnly)) {
            file.write(QJsonDocument(presetObj).toJson());
            file.close();
            ++exportedCount;
        }
        ui->exportProgressBar->setValue(++progress);
        QCoreApplication::processEvents(); // Allow UI to update
    }

    // Export selected MCCC presets as individual files
    QList<QListWidgetItem*> mcccItems = ui->mcccPresetList->selectedItems();
    for (QListWidgetItem *item : mcccItems) {
        QString key = item->text();
        MainWindow *mainWindowPointer = qobject_cast<MainWindow *>(parentWidget());
        if (!mainWindowPointer) {
            QMessageBox::warning(this, "Error", "Main window pointer is invalid.");
            continue;
        }
        QString rootdir = mainWindowPointer->getRootDir();
        QString mcccPresetsDirPath = rootdir + "/mcccPresets";
        QDir mcccPresetsDir(mcccPresetsDirPath);
        if (!mcccPresetsDir.exists()) {
            QMessageBox::warning(this, "Error", "MCCC presets directory does not exist: " + mcccPresetsDirPath);
            continue;
        }
        QString presetFilePath = mcccPresetsDir.filePath(key + ".cfg");
        QFileInfo presetFileInfo(presetFilePath);
        if (!presetFileInfo.exists()) {
            QMessageBox::warning(this, "Error", "MCCC preset file does not exist: " + presetFilePath);
            continue;
        }

        QString destFilePath = exportDir.filePath(key + ".cfg");
        if (QFile::copy(presetFilePath, destFilePath)) {
            ++exportedCount;
        } else {
            QMessageBox::warning(this, "Error", "Failed to copy MCCC preset file: " + presetFilePath);
        }
        ui->exportProgressBar->setValue(++progress);
        QCoreApplication::processEvents(); // Allow UI to update
    }

    if (exportedCount > 0) {
        QMessageBox::information(this, "Success", QString("Exported %1 preset(s) successfully!").arg(exportedCount));
        accept();
    } else {
        QMessageBox::warning(this, "Error", "No presets were exported.");
    }
}