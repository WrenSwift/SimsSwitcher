#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "exportwindow.h" // Add this include at the top if not already present
#include "MCCCSettings.h" // Add this include for MCCCSettings
#include "FileListItemWidget.h"
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QCheckBox>
#include <QFile>
#include <QProgressDialog>
#include <QDirIterator>
#include <QSettings>
#include <QDebug>
#include <QHash>
#include <QTextStream>
#include <QStringList>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QProcess>
#include <QCloseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QJsonArray>
#include <QEventLoop>

QString activeSubDirName = "Mods"; // This is where the active mods are stored
QString disabledSubDirName = "(d)Mods"; // Change this to your desired subdirectory
QString csvFilePath = "inc/packsDil.csv";
QString csvCloudPath = "https://wrenswift.com/packsDil.csv";
QString version = "1.1.0"; // Version of the application

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimsSwitcher)
{
    ui->setupUi(this);

    mcccCheck();
    updatePresetList();
    updatePackPresetList(); // Initialize pack presets
    doVersionCheck();
    do_patreonLink();

    // Set the window title
    this->setWindowTitle("SimsSwitcher");

    // Set the window icon (ensure the icon file exists in your project resources or a valid path)
    this->setWindowIcon(QIcon(":/icons/SimSwitcherIcon256.png"));

    QIcon icon(":/icons/SimSwitcherIcon.png");
    if (icon.isNull()) {
        qDebug() << "Failed to load icon!";
    }


    ui->mainStackedWidget->setCurrentIndex(0);

    // Retrieve the cached source directory path
    QSettings settings("Falcon", "SimsSwitcher");
    QString cachedSource = settings.value("rootDirectory", QString()).toString();
    QString cachedGameSource = settings.value("gameDirectory", QString()).toString();

    // Create a QDir object from the selected directory.
    QDir baseDir(cachedSource);

    // Append the subdirectory using QDir::filePath to get a platform-appropriate path.
    QString activeModsPath = baseDir.filePath(activeSubDirName);
    QString disabledModsPath = baseDir.filePath(disabledSubDirName);

    if (!cachedSource.isEmpty()) {
        ui->rootLineEdit->setText(cachedSource);
        populateFileList(activeModsPath,disabledModsPath);
    }
    if (!cachedGameSource.isEmpty()) {
        ui->gameLineEdit->setText(cachedGameSource);
        loadPacksCsv(csvCloudPath, csvFilePath);
    }

    ui->fileListWidget->setAcceptDrops(true);
    ui->fileListWidget->setDragDropMode(QAbstractItemView::DropOnly);

    qDebug() << "Resource exists:" << QFile::exists(":/inc/packsDil.csv");
}

QString MainWindow::getRootDir() const {
    return ui->rootLineEdit->text().trimmed();
}

void MainWindow::doVersionCheck() {
    // Set the version label with a hyperlink to the release page
    QString releaseUrl = "https://github.com/WrenSwift/SimsSwitcher/releases/tag/" + version;
    ui->verLabel->setText(QString("<a href=\"%1\">Version: %2</a>").arg(releaseUrl, version));
    ui->verLabel->setTextFormat(Qt::RichText);
    ui->verLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->verLabel->setOpenExternalLinks(true);
    // Check the latest release version from GitHub
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl("https://api.github.com/repos/WrenSwift/SimsSwitcher/releases/latest")));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        if (jsonDoc.isObject()) {
            QString latestTag = jsonDoc.object().value("tag_name").toString();

            // Helper lambda to parse version string "x.y.z" into a list of ints
            auto parseVersion = [](const QString& ver) -> QList<int> {
                QStringList parts = ver.split('.');
                QList<int> nums;
                for (const QString& part : parts) {
                    bool ok = false;
                    int n = part.toInt(&ok);
                    nums << (ok ? n : 0);
                }
                while (nums.size() < 3) nums << 0; // pad to 3 parts
                return nums;
            };

            if (!latestTag.isEmpty()) {
                QList<int> latestParts = parseVersion(latestTag);
                QList<int> localParts = parseVersion(version);

                bool isNewer = false;
                bool isOlder = false;
                for (int i = 0; i < 3; ++i) {
                    if (latestParts[i] > localParts[i]) {
                        isNewer = true;
                        break;
                    } else if (localParts[i] > latestParts[i]) {
                        isOlder = true;
                        break;
                    }
                }

                if (isNewer) {
                    QString downloadUrl = jsonDoc.object().value("html_url").toString();
                    QString message = QString("A new version is available: %1\nYou are running: %2").arg(latestTag, version);
                    if (!downloadUrl.isEmpty()) {
                        message += QString("<br><br>Get the latest version here: <a href=\"%1\">%1</a>").arg(downloadUrl);
                    }
                    // Use rich text and enable links in the QMessageBox
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("Update Available");
                    msgBox.setTextFormat(Qt::RichText);
                    msgBox.setText(message);
                    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.exec();
                    return;
                    QMessageBox::warning(nullptr, "Update Available", message);
                } else if (isOlder) {
                    if (QSettings("Falcon", "SimsSwitcher").value("dontShowExperimentalVersion", false).toBool()) {
                        qDebug() << "Experimental version check skipped by user setting.";
                        return;
                    }
                    QString downloadUrl = jsonDoc.object().value("html_url").toString();
                    QString expMsg = QString("You are running an newer version: %1\nLatest version is: %2 You may experience bugs or unexpected behaivor.").arg(version, latestTag);
                    if (!downloadUrl.isEmpty()) {
                        expMsg += QString("<br><br>You can get the latest release version here: <a href=\"%1\">%1</a>").arg(downloadUrl);
                    }
                    expMsg += "<br><br>Are you sure you want to continue?";
                    // Use rich text and enable links in the QMessageBox
                    QMessageBox msgBox;
                    msgBox.setWindowTitle("Experimental Version");
                    msgBox.setTextFormat(Qt::RichText);
                    msgBox.setText(expMsg);
                    msgBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
                    msgBox.setIcon(QMessageBox::Warning);
                    msgBox.setStandardButtons(QMessageBox::Ok);
                    msgBox.setDefaultButton(QMessageBox::Ok);
                    // Add do not show again checkbox
                    QCheckBox *dontShowAgain = new QCheckBox("Do not show this message again");
                    msgBox.setCheckBox(dontShowAgain);
                    // Add funtion to Checkbox to save setting
                    connect(dontShowAgain, &QCheckBox::stateChanged, this, [this, dontShowAgain](int state) {
                        QSettings settings("Falcon", "SimsSwitcher");
                        settings.setValue("dontShowExperimentalVersion", state == Qt::Checked);
                    });
                    msgBox.exec();   
                    return; 
                } else {
                    qDebug() << "Version check passed.";
                }
                } else {
                    qDebug() << "Version tag is empty.";
                }
            } else {
                qDebug() << "Failed to parse GitHub response.";
            }
        } else {
            qDebug() << "Failed to fetch latest release info:" << reply->errorString();
        }
        reply->deleteLater();
}

void MainWindow::loadPacksCsv(const QString &url, const QString &localPath)
{
    QNetworkAccessManager manager;
    QEventLoop loop;
    QNetworkReply *reply = manager.get(QNetworkRequest(QUrl(url)));
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QByteArray csvData;
    bool usedNetwork = false;

    if (reply->error() == QNetworkReply::NoError) {
        csvData = reply->readAll();
        usedNetwork = true;
        // Save the latest network version locally for future offline use
        QFile localFile(localPath);
        if (localFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            localFile.write(csvData);
            localFile.close();
        }
    } else {
        // Network failed, try to load from local file
        QFile localFile(localPath);
        if (localFile.open(QIODevice::ReadOnly)) {
            csvData = localFile.readAll();
            localFile.close();
            QMessageBox::warning(this, tr("Network Error"),
                tr("Failed to download packs CSV from the cloud. Using local backup copy instead."));
        } else {
            QMessageBox::warning(this, tr("Network Error"),
                tr("Failed to download packs CSV from the cloud and no local backup is available: %1").arg(reply->errorString()));
            reply->deleteLater();
            return;
        }
    }
    reply->deleteLater();

    // Parse CSV data (same as before)
    QHash<QString, QString> folderMapping;
    QTextStream in(csvData);
    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;
        if (firstLine) {
            firstLine = false;
            // continue; // Uncomment if your CSV has a header
        }
        QStringList parts = line.split(',');
        if (parts.size() >= 2) {
            QString originalName = parts.at(0).trimmed();
            QString friendlyName = parts.at(1).trimmed();
            folderMapping.insert(originalName, friendlyName);
        }
    }
    populatePacksListWidgetWithMapping(ui->gameLineEdit->text(), folderMapping);
}

void MainWindow::on_menuMods_clicked(){
    ui->mainStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_menuPacks_clicked(){
    ui->mainStackedWidget->setCurrentIndex(1);
    do_S4MPCheck();
}

void MainWindow::on_menuSettings_clicked(){
    ui->mainStackedWidget->setCurrentIndex(2);
}

//Mods Page code Below

void MainWindow::on_browseRootButton_clicked() {
    // Open a dialog for the user to select the Sims 4 Root Directory.
    QString rootDir = QFileDialog::getExistingDirectory(this, tr("Select Sims 4 Root Directory"));

    if (!rootDir.isEmpty()) {

        // Create a QDir object from the selected directory.
        QDir baseDir(rootDir);

        // Append the subdirectory using QDir::filePath to get a platform-appropriate path.
        QString activeModsPath = baseDir.filePath(activeSubDirName);
        QString disabledModsPath = baseDir.filePath(disabledSubDirName);

        // Optionally, create the subdirectory if it does not already exist.
        if (!baseDir.exists(activeSubDirName)) {
            if (!baseDir.mkpath(activeSubDirName)) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to create subdirectory: %1").arg(activeSubDirName));
                return;
            }
        }

        if (!baseDir.exists(disabledSubDirName)) {
            if (!baseDir.mkpath(disabledSubDirName)) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to create subdirectory: %1").arg(disabledSubDirName));
                return;
            }
        }

        ui->rootLineEdit->setText(rootDir);

        // Populate the file list using the full path (Sims 4 Root + Subdirectory).
        populateFileList(activeModsPath,disabledModsPath);

        // Use QSettings to store the full path including the subdirectory.
        QSettings settings("Falcon", "SimsSwitcher");
        settings.setValue("rootDirectory", rootDir);
    }
}

void MainWindow::mcccCheck() {
    // Check for "mc_cmd_center.package" in active mods.
    QString mcccPack = "mc_cmd_center.package";
    bool foundMccc = false;

    // Get the active mods directory from the root directory.
    QString rootDir = ui->rootLineEdit->text();
    QDir baseDir(rootDir);
    QString activeModsPath = baseDir.filePath(activeSubDirName);

    // Check if the mccc package exists in the active mods directory.
    if (QFile::exists(activeModsPath + "/" + mcccPack)) {
        foundMccc = true;
    }

    if (foundMccc) {
        // Make mcccButton visible
        ui->mcccButton->setVisible(true);
        ui->mcccButton->setEnabled(true);
    }else {
        // Hide mcccButton if not found
        ui->mcccButton->setVisible(false);
        ui->mcccButton->setEnabled(false);
    }
}

void MainWindow::on_mcccButton_clicked() {
    
    MCCCSettings mcccSettings(this);
    mcccSettings.exec(); // Show as a modal dialog
}

void MainWindow::on_fileSelectAllButton_clicked()
{
    int itemCount = ui->fileListWidget->count();
    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        QWidget* widget = ui->fileListWidget->itemWidget(item);
        auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
        if (fileWidget)
            fileWidget->setChecked(true);
    }
}

void MainWindow::on_fileDeselectAllButton_clicked()
{
    int itemCount = ui->fileListWidget->count();
    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        QWidget* widget = ui->fileListWidget->itemWidget(item);
        auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
        if (fileWidget)
            fileWidget->setChecked(false);
    }
}

void MainWindow::populateFileList(const QString& firstDir, const QString& secondDir) {
    ui->fileListWidget->clear();

    // Gather file/folder names from both directories to check for duplicates
    QDir dirFirst(firstDir);
    QDir dirSecond(secondDir);

    QSet<QString> firstNames;
    QSet<QString> secondNames;

    // Collect names from firstDir (Mods)
    QFileInfoList firstList = dirFirst.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo& fileInfo : firstList) {
        if (fileInfo.fileName().compare("Resource.cfg", Qt::CaseInsensitive) == 0)
        continue; // Ignore Resource.cfg
        firstNames.insert(fileInfo.fileName());
    }

    // Collect names from secondDir ((d)Mods)
    QFileInfoList secondList = dirSecond.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);
    for (const QFileInfo& fileInfo : secondList) {
        if (fileInfo.fileName().compare("Resource.cfg", Qt::CaseInsensitive) == 0)
        continue; // Ignore Resource.cfg
        secondNames.insert(fileInfo.fileName());
    }

    // Find duplicates
    QSet<QString> duplicates = firstNames & secondNames;
    if (!duplicates.isEmpty()) {
        QStringList duplicateList = duplicates.values();
        QString msg = tr(
            "Warning: The following files or folders exist in both '%1' and '%2':\n\n%3\n\n"
            "This can cause issues with mod management. Please delete or move the duplicates from the incorrect location."
        ).arg(activeSubDirName, disabledSubDirName, duplicateList.join("\n"));

        QMessageBox::warning(this, tr("Duplicate Mods Detected"), msg);
    }

    // A helper lambda that takes a directory path and a flag indicating if the items should be pre-checked.
    auto addDirectoryItems = [this](const QString &dirPath, bool preChecked) {
        QDir dir(dirPath);
        QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        if (!fileList.isEmpty()) {
            // Determine a friendly header based on which directory this is
            QString friendlyHeader;
            if (dirPath.endsWith("/" + activeSubDirName) || dirPath.endsWith("\\" + activeSubDirName)) {
                friendlyHeader = "Active Mods";
            } else if (dirPath.endsWith("/" + disabledSubDirName) || dirPath.endsWith("\\" + disabledSubDirName)) {
                friendlyHeader = "Disabled Mods";
            } else {
                friendlyHeader = "Items from: " + dirPath;
            }
            QListWidgetItem* headerItem = new QListWidgetItem(friendlyHeader);
            // Make the header non-checkable
            headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsUserCheckable);
            headerItem->setBackground(Qt::darkGreen);
            // Set the tooltip to show the full path on hover
            headerItem->setToolTip(dirPath);
            ui->fileListWidget->addItem(headerItem);
        }

        mcccCheck(); // Check if MCCC is present in the active mods directory

        for (const QFileInfo& fileInfo : fileList) {
            if (fileInfo.fileName().compare("Resource.cfg", Qt::CaseInsensitive) == 0)
            continue; // Ignore Resource.cfg

            QString displayName = fileInfo.fileName();
            QString fullPath = dir.filePath(displayName);

            QListWidgetItem* item = new QListWidgetItem(ui->fileListWidget);
            item->setSizeHint(QSize(0, 20)); // Adjust height as needed

            auto* widget = new FileListItemWidget(displayName);
            widget->setChecked(preChecked); // Set initial state
            ui->fileListWidget->setItemWidget(item, widget);
            item->setData(Qt::UserRole, fullPath);

            // --- Add delete functionality ---
            connect(widget, &FileListItemWidget::deleteRequested, this, [this, fullPath, item]() {
                if (QMessageBox::question(this, tr("Delete Item"),
                    tr("Are you sure you want to delete '%1'?\nThis will move the item to the system trash.").arg(fullPath),
                    QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
                {
                #if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
                    if (!QFile::moveToTrash(fullPath)) {
                        QMessageBox::warning(this, tr("Delete Failed"), tr("Failed to move '%1' to trash.").arg(fullPath));
                    } else {
                        delete ui->fileListWidget->takeItem(ui->fileListWidget->row(item));
                    }
                #else
                    // Qt 5 fallback: permanently delete (or use platform-specific trash)
                    if (!QFile::remove(fullPath)) {
                        QMessageBox::warning(this, tr("Delete Failed"), tr("Failed to delete '%1'.").arg(fullPath));
                    } else {
                        delete ui->fileListWidget->takeItem(ui->fileListWidget->row(item));
                    }
                #endif
                }
            });
        }
    };

    // Process the first directory: files will be pre-checked.
    addDirectoryItems(firstDir, true);

    // Process the second directory: files will be unchecked.
    addDirectoryItems(secondDir, false);
}

QStringList MainWindow::presetList() {
    QSettings settings("Falcon", "SimsSwitcher");
    settings.beginGroup("presets");
    QStringList keys = settings.childKeys();
    settings.endGroup();
    return keys;
}

void MainWindow::savePreset(const QString& presetName) {
    QStringList enabledItems;
    for (int i = 0; i < ui->fileListWidget->count(); ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        QWidget* widget = ui->fileListWidget->itemWidget(item);
        auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
        if (fileWidget && fileWidget->isChecked()) {
            enabledItems << fileWidget->fileName();
        }
    }
    QSettings settings("Falcon", "SimsSwitcher");
    settings.setValue("presets/" + presetName, enabledItems);
}

void MainWindow::loadPreset(const QString& presetName) {
    QSettings settings("Falcon", "SimsSwitcher");
    QStringList enabledItems = settings.value("presets/" + presetName).toStringList();
    for (int i = 0; i < ui->fileListWidget->count(); ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        QWidget* widget = ui->fileListWidget->itemWidget(item);
        auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
        if (fileWidget)
            fileWidget->setChecked(enabledItems.contains(fileWidget->fileName()));
    }
    on_activeButton_clicked();
}

void MainWindow::on_presetDeleteButton_clicked()
{
    // Get the selected preset name from the list widget
    if (!ui->presetList->currentItem()) {
        QMessageBox::warning(this, tr("Error"), tr("No preset selected."));
        return;
    }
    QString presetName = ui->presetList->currentItem()->text();

    // Confirm deletion with the user
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Preset"),
                                  tr("Are you sure you want to delete the preset '%1'?").arg(presetName),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSettings settings("Falcon", "SimsSwitcher");
        settings.remove("presets/" + presetName);
        updatePresetList();
    }
}

void MainWindow::on_modsLoadButton_clicked()
{
    // Get the selected preset name from the list widget
    if (!ui->presetList->currentItem()) {
        QMessageBox::warning(this, tr("Error"), tr("No preset selected."));
        return;
    }
    QString presetName = ui->presetList->currentItem()->text();

    // Error checking: Ensure all items from preset are present in the list widget
    QSettings settings("Falcon", "SimsSwitcher");
    QStringList enabledItems = settings.value("presets/" + presetName).toStringList();

    QStringList missingItems;
    for (const QString& presetItem : enabledItems) {
        bool found = false;
        for (int i = 0; i < ui->fileListWidget->count(); ++i) {
            QListWidgetItem* item = ui->fileListWidget->item(i);
            QWidget* widget = ui->fileListWidget->itemWidget(item);
            auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
            if (fileWidget && fileWidget->fileName() == presetItem) {
                found = true;
                break;
            }
        }
        if (!found) {
            missingItems << presetItem;
        }
    }

    if (!missingItems.isEmpty()) {
        QMessageBox::warning(this, tr("Preset Load Warning"),
                             tr("The following items from the preset were not found in the disabled or enabled mods:\n%1")
                             .arg(missingItems.join(", ")));
                             tr("Please ensure that all items in the preset are present in the list before loading. If a new version has been added, please update the preset.");
    }

    // Ensure preset exists before loading
    if (!settings.contains("presets/" + presetName)) {
        QMessageBox::warning(this, tr("Error"), tr("Preset '%1' does not exist. Please create preset '%1' before trying to load it.").arg(presetName));
        return;
    }   
    
    // Load the preset using the existing load function
    loadPreset(presetName);

    // Optionally, you can also update the preset line edit to show the loaded preset name
    ui->presetLineEdit->setText(presetName);
}

void MainWindow::on_modsSaveButton_clicked()
{
    // Get the preset name from the line edit
    QString presetName = ui->presetLineEdit->text().trimmed();

    if (presetName.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Preset name cannot be empty."));
        return;
    }

    // If the preset name exists, show an overwrite confirmation message
    QSettings settings("Falcon", "SimsSwitcher");
    if (settings.contains("presets/" + presetName)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Preset Overwrite"),
                                      tr("Preset '%1' already exists. Do you want to overwrite it?").arg(presetName),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return; // User chose not to overwrite
        }
    }
    
    // Save the preset using the existing save function
    savePreset(presetName);

    // Update the preset list in the UI
    updatePresetList();

}

void MainWindow::updatePresetList()
{
    QStringList presets = presetList();
    ui->presetList->clear();
    ui->presetList->addItems(presets);
}

void MainWindow::onPresetSelected(QListWidgetItem* item) {
    if (item) {
        ui->presetLineEdit->setText(item->text());
    }
}

void MainWindow::on_activeButton_clicked() {
    QString rootDir = ui->rootLineEdit->text();
    QDir baseDir(rootDir);
    QString m_firstDir = baseDir.filePath(activeSubDirName);
    QString m_secondDir = baseDir.filePath(disabledSubDirName);

    // Verify that these directories have been set.
    if (m_firstDir.isEmpty() || m_secondDir.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Source directories are not set."));
        return;
    }

    // Collect all items from the list that are real files/folders (skip headers)
    QList<QPair<QListWidgetItem*, FileListItemWidget*>> itemsToProcess;
    int totalItems = ui->fileListWidget->count();
    for (int i = 0; i < totalItems; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        QWidget* widget = ui->fileListWidget->itemWidget(item);
        auto* fileWidget = qobject_cast<FileListItemWidget*>(widget);
        if (!fileWidget)
            continue; // Skip headers or non-file items
        itemsToProcess.append(qMakePair(item, fileWidget));
    }

    if (itemsToProcess.isEmpty()) {
        QMessageBox::information(this, tr("No Items"), tr("No items to move."));
        return;
    }

    // Create a progress dialog for visual feedback.
    QProgressDialog progress("Moving items...", "Cancel", 0, itemsToProcess.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    int progressValue = 0;
    for (const auto& pair : itemsToProcess) {
        QListWidgetItem* item = pair.first;
        FileListItemWidget* fileWidget = pair.second;

        QString srcPath = item->data(Qt::UserRole).toString();
        QFileInfo fileInfo(srcPath);

        // Ensure the file/folder exists.
        if (!fileInfo.exists()) {
            qDebug() << "File not found:" << srcPath;
            continue;
        }

        // Determine the desired destination based on the custom widget's checked state.
        // Checked items go to m_firstDir (Active), unchecked to m_secondDir (Inactive).
        QString desiredDestDir = fileWidget->isChecked() ? m_firstDir : m_secondDir;

        // Get the current parent directory of the item.
        QString currentDir = fileInfo.absolutePath();

        // If the item is already in the desired folder, skip moving it.
        if (QDir(currentDir) == QDir(desiredDestDir)) {
            qDebug() << "Item" << fileInfo.fileName() << "is already in the desired directory.";
            continue;
        }

        // Construct the destination path using the desired folder plus the file/folder name.
        QString destPath = desiredDestDir + "/" + fileInfo.fileName();

        bool success = false;
        if (fileInfo.isDir()) {
            // Attempt to move the directory using QDir::rename
            QDir srcDir(srcPath);
            success = srcDir.rename(srcPath, destPath);
            if (!success) {
                QMessageBox::warning(this, tr("Move Error"),
                                     tr("Failed to move directory: %1").arg(fileInfo.fileName()));
            }
        } else if (fileInfo.isFile()) {
            // Use QFile::rename for individual files.
            success = QFile::rename(srcPath, destPath);
            if (!success) {
                QMessageBox::warning(this, tr("Move Error"),
                                     tr("Failed to move file: %1").arg(fileInfo.fileName()));
            }
        } else {
            QMessageBox::warning(this, tr("Move Error"),
                                 tr("Unknown item type: %1").arg(fileInfo.fileName()));
        }

        progress.setValue(++progressValue);
        if (progress.wasCanceled())
            break;
    }

    progress.setValue(itemsToProcess.size());
    QMessageBox::information(this, tr("Move Completed"),
                             tr("Items have been moved successfully."));

    // After moving, repopulate the file list to reflect the changes.
    populateFileList(m_firstDir, m_secondDir);
}

bool MainWindow::copyDirectory(const QString& sourcePath, const QString& destPath) {
    QDir sourceDir(sourcePath);
    if (!sourceDir.exists()) {
        return false;
    }

    QDir destDir(destPath);
    if (!destDir.exists()) {
        destDir.mkpath(destPath);
    }

    foreach (QString entry, sourceDir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries)) {
        QString srcFilePath = sourcePath + "/" + entry;
        QString destFilePath = destPath + "/" + entry;

        QFileInfo fileInfo(srcFilePath);
        if (fileInfo.isDir()) {
            if (!copyDirectory(srcFilePath, destFilePath)) {
                return false;
            }
        } else {
            if (!QFile::copy(srcFilePath, destFilePath)) {
                return false;
            }
        }
    }
    return true;
}

//Packs Page code Below

void MainWindow::do_S4MPCheck() {
    // Check for "S4MP Launcher Windows.exe" in active mods.
    QString s4mpExeName = "S4MP Launcher Windows.exe";
    bool foundS4MP = false;

    // Get the active mods directory from the root directory.
    QString rootDir = ui->rootLineEdit->text();
    QDir baseDir(rootDir);
    QString activeModsPath = baseDir.filePath(activeSubDirName);

    // Check if the S4MP executable exists in the active mods directory.
    if (QFile::exists(activeModsPath + "/" + s4mpExeName)) {
        foundS4MP = true;
    }

    if (foundS4MP) {
        QMessageBox::warning(this, tr("S4MP Detected"),
            tr("'%1' is present in your active mods. "
               "If you plan to use S4MP please use their launcher for pack selection after desired mods are selected.").arg(s4mpExeName));
    }
}

QHash<QString, QString> loadFolderNameMappings(const QString &csvFilePath)
{
    QHash<QString, QString> folderMapping;
    QFile file(csvFilePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open CSV file:" << csvFilePath;
        return folderMapping;
    }

    QTextStream in(&file);
    bool firstLine = true;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty())
            continue;

        // If you have a header line you want to skip, enable this block.
        if (firstLine) {
            firstLine = false;
            // Uncomment the next line if your CSV file contains a header row.
            // continue;
        }

        QStringList parts = line.split(',');
        // Ensure that there are at least two parts (original name, friendly name)
        if (parts.size() >= 2) {
            QString originalName = parts.at(0).trimmed();
            QString friendlyName = parts.at(1).trimmed();
            folderMapping.insert(originalName, friendlyName);
        } else {
            qWarning() << "CSV line does not have enough parts:" << line;
        }
    }
    file.close();
    return folderMapping;
}

void MainWindow::on_packsSelectAllButton_clicked()
{
    int itemCount = ui->packsListWidget->count();

    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->packsListWidget->item(i);

        // Ensure the item is checkable before changing its state
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void MainWindow::on_packsDeselectAllButton_clicked()
{
    int itemCount = ui->packsListWidget->count();

    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->packsListWidget->item(i);

        // Ensure the item is checkable before changing its state
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void MainWindow::populatePacksListWidget(const QString &folderPath, const QString &csvFilePath)
{
    // Clear the list widget first.
    ui->packsListWidget->clear();

    // Retrieve previously selected folder names from QSettings.
    QSettings settings("Falcon", "SimsSwitcher");
    QStringList checkedPacks = settings.value("packsSelection").toStringList();

    // Load the folder name mappings from the CSV file.
    QHash<QString, QString> folderMapping = loadFolderNameMappings(csvFilePath);

    // Use QDir to list only directories (ignoring "." and "..") in the given folder path.
    QDir dir(folderPath);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Loop through each folder info.
    for (const QFileInfo &info : folderList) {
        QString originalName = info.fileName();

        // Only process folders that have been renamed (i.e., exist in the mapping).
        if (!folderMapping.contains(originalName))
            continue;

        // Look up the friendly name in the mapping; if not found, use the original name.
        QString displayName = folderMapping.value(originalName);

        // Create a new list widget item with the friendly display text.
        QListWidgetItem *item = new QListWidgetItem(displayName);

        // Make the item checkable and set its default check state to unchecked.
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);

        // Restore the check state from the saved selection.
        if (checkedPacks.contains(originalName))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);

        // Store the full absolute path in the default UserRole (if needed later).
        item->setData(Qt::UserRole, info.absoluteFilePath());
        // Store the original folder name in a custom role (UserRole+1) for later reference.
        item->setData(Qt::UserRole + 1, originalName);

        // Add the item to the list widget.
        ui->packsListWidget->addItem(item);



        // Optional debug output.
        qDebug() << "Added folder:" << originalName << "as" << displayName
                 << "Full path:" << info.absoluteFilePath();
    }
    // Automatically call the sorting function to group items into categories.
    sortPacksListByCategory();
}

void MainWindow::populatePacksListWidgetWithMapping(const QString &folderPath, const QHash<QString, QString> &folderMapping)
{
    ui->packsListWidget->clear();

    QSettings settings("Falcon", "SimsSwitcher");
    QStringList checkedPacks = settings.value("packsSelection").toStringList();

    QDir dir(folderPath);
    QFileInfoList folderList = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

    for (const QFileInfo &info : folderList) {
        QString originalName = info.fileName();
        if (!folderMapping.contains(originalName))
            continue;
        QString displayName = folderMapping.value(originalName);
        QListWidgetItem *item = new QListWidgetItem(displayName);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        if (checkedPacks.contains(originalName))
            item->setCheckState(Qt::Checked);
        else
            item->setCheckState(Qt::Unchecked);
        item->setData(Qt::UserRole, info.absoluteFilePath());
        item->setData(Qt::UserRole + 1, originalName);
        ui->packsListWidget->addItem(item);
        qDebug() << "Added folder:" << originalName << "as" << displayName
                 << "Full path:" << info.absoluteFilePath();
    }
    // If any packs are present in the source directory that are not in the mapping, display a warning.
    QStringList mappedNames = folderMapping.keys();
    // Check for unmapped packs in the directory.
    for (const QFileInfo &info : folderList) {
        // Get the original name from the QFileInfo.
        QString originalName = info.fileName();
        // Sort out original names that are not in the format of "EP01", "GP02", etc.
        if (originalName.length() < 4 || !originalName.startsWith("EP") && !originalName.startsWith("GP") &&
            !originalName.startsWith("SP") && !originalName.startsWith("FP")) {
            continue; // Skip names that do not match the expected format.
        }
        if (!mappedNames.contains(originalName)) {
            // If the original name is not in the mapping, it means it's an unmapped pack.
            qDebug() << "Unmapped pack found:" << originalName;
            QMessageBox::warning(this, tr("Unmapped Pack Warning"),
                                 tr("The pack '%1' is not mapped in the CSV file. "
                                    "This could mean the packs data is out of date or another error is occuring. "
                                    "If you have not recieved a Network Error please report the issue.").arg(originalName));
        }
    }
    // Automatically call the sorting function to group items into categories.
    sortPacksListByCategory();
}

void MainWindow::sortPacksListByCategory()
{
    // Define alias-to-description mappings
    QMap<QString, QString> aliasDescriptions = {
        {"EP", "Expansion Packs"},
        {"FP", "Free Packs"},
        {"GP", "Game Packs"},
        {"SP", "Stuff Packs and Kits"}
    };

    QMap<QString, QList<QListWidgetItem*>> categoryMap;

    // Group items by their prefix (first two letters of original names)
    int itemCount = ui->packsListWidget->count();
    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->packsListWidget->takeItem(0); // Remove items one by one

        QString originalName = item->data(Qt::UserRole + 1).toString();
        QString prefix = originalName.left(2).toUpper(); // Extract the first two letters

        // Ensure the prefix is valid and belongs to our defined aliases
        if (aliasDescriptions.contains(prefix)) {
            categoryMap[prefix].append(item);
        } else {
            qDebug() << "Uncategorized item:" << originalName; // Debug for unexpected prefixes
        }
    }

    ui->packsListWidget->clear();

    // Re-add items with headers based on categories and descriptions
    for (auto it = categoryMap.constBegin(); it != categoryMap.constEnd(); ++it) {
        const QString& prefix = it.key();
        const QList<QListWidgetItem*>& items = it.value();

        if (!items.isEmpty()) { // Ensure the category has items
            // Generate the header text using the alias-to-description map
            QString headerText = aliasDescriptions.value(prefix, "Unknown Category");

            QListWidgetItem* headerItem = new QListWidgetItem(headerText);
            headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsUserCheckable); // Make header non-checkable
            headerItem->setBackground(Qt::darkGreen); // Optional: Style the header
            ui->packsListWidget->addItem(headerItem);

            for (QListWidgetItem* item : items) {
                ui->packsListWidget->addItem(item);
            }
        }
    }
}

QString MainWindow::getDisablePacksStringFromListWidget()
{
    QStringList disableList;
    int itemCount = ui->packsListWidget->count();

    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem *item = ui->packsListWidget->item(i);
        // Now, invert the condition: add items that are unchecked.
        if (item->checkState() == Qt::Unchecked) {
            // Retrieve the original folder name stored in (for example) Qt::UserRole+1.
            QString originalName = item->data(Qt::UserRole + 1).toString();
            if (!originalName.isEmpty())
                disableList.append(originalName);
        }
    }

    if (disableList.isEmpty())
        return "";

    // Build the final output string.
    return "-disablepacks:" + disableList.join(",");
}

void MainWindow::on_browseGameButton_clicked() {
    // Open a dialog for the user to select the Sims 4 Root Directory.
    QString gameDir = QFileDialog::getExistingDirectory(this, tr("Select Sims 4 Install Directory"));

    if (!gameDir.isEmpty()) {

        ui->gameLineEdit->setText(gameDir);

        // Populate the file list using the full path (Sims 4 Root + Subdirectory).
        loadPacksCsv(csvCloudPath, csvFilePath);

        // Use QSettings to store the full path including the subdirectory.
        QSettings settings("Falcon", "SimsSwitcher");
        settings.setValue("gameDirectory", gameDir);
    }
}

void MainWindow::on_launchButton_clicked()
{
    // 1. Get the root selection.
    QString gameSelection = ui->gameLineEdit->text().trimmed();
    if (gameSelection.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("No game directory has been selected."));
        return;
    }

    // 2. Build the path to the process executable.
    //    For example, if the executable is located in a subdirectory "SubDirectory"
    //    inside the selected folder, combine the paths using QDir::filePath():
    QDir rootDir(gameSelection);
    QString executablePath = rootDir.filePath("Game/Bin/TS4_Launcher_x64.exe");
    if (!QFile::exists(executablePath)) {
        QMessageBox::warning(this, tr("Error"), tr("Process executable not found at: %1").arg(executablePath));
        return;
    }

    // 3. Generate the disable packs argument from the QListWidget.
    //    This function should collect the original names from unchecked items
    //    and produce a string like: "-disablepacks:EP01,EP02,EP03".
    QString disablePacksArg = getDisablePacksStringFromListWidget();
    // (If no packs are disabled you might choose to omit the argument.)

    // 4. (Optional) You may want to pass other arguments—let's reuse the selected path,
    //    for example, as an argument.
    //    Here, we'll create an argument with the full root selection.
    QString rootArg = "-root:" + gameSelection;

    // 5. Prepare the argument list. (If disablePacksArg is empty, you could choose not to include it.)
    QStringList arguments;
    if (!disablePacksArg.isEmpty())
        arguments << disablePacksArg;
    arguments << rootArg;

    // You can print the arguments for debugging:
    qDebug() << "Launching process at:" << executablePath << "with arguments:" << arguments;

    // 6. Launch the process.
    //    You can use QProcess::startDetached if you want the process to run independently:
    bool started = QProcess::startDetached(executablePath, arguments);
    if (!started) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to launch process."));
    }
}

void MainWindow::savePacksSelection()
{
    QStringList selectedPacks;
    int count = ui->packsListWidget->count();

    // Iterate over all items.
    for (int i = 0; i < count; ++i) {
        QListWidgetItem *item = ui->packsListWidget->item(i);
        // Only consider items that are checkable.
        if (item->flags() & Qt::ItemIsUserCheckable) {
            if (item->checkState() == Qt::Checked) {
                // Retrieve the original folder name stored in UserRole + 1.
                QString originalName = item->data(Qt::UserRole + 1).toString();
                selectedPacks.append(originalName);
            }
        }
    }

    QSettings settings("Falcon", "SimsSwitcher");
    settings.setValue("packsSelection", selectedPacks);
    qDebug() << "Saved packs selection:" << selectedPacks;
}
// --- Packs Preset Functionality ---
QStringList MainWindow::packPresetList() {
    QSettings settings("Falcon", "SimsSwitcher");
    settings.beginGroup("packPresets");
    QStringList keys = settings.childKeys();
    settings.endGroup();
    return keys;
}

void MainWindow::savePackPreset(const QString& presetName) {
    QStringList enabledPacks;
    for (int i = 0; i < ui->packsListWidget->count(); ++i) {
        QListWidgetItem* item = ui->packsListWidget->item(i);
        if ((item->flags() & Qt::ItemIsUserCheckable) && item->checkState() == Qt::Checked) {
            // Store the original folder name for reliability
            QString originalName = item->data(Qt::UserRole + 1).toString();
            enabledPacks << originalName;
        }
    }
    QSettings settings("Falcon", "SimsSwitcher");
    settings.setValue("packPresets/" + presetName, enabledPacks);
}

void MainWindow::loadPackPreset(const QString& presetName) {
    QSettings settings("Falcon", "SimsSwitcher");
    QStringList enabledPacks = settings.value("packPresets/" + presetName).toStringList();
    for (int i = 0; i < ui->packsListWidget->count(); ++i) {
        QListWidgetItem* item = ui->packsListWidget->item(i);
        if (item->flags() & Qt::ItemIsUserCheckable) {
            QString originalName = item->data(Qt::UserRole + 1).toString();
            item->setCheckState(enabledPacks.contains(originalName) ? Qt::Checked : Qt::Unchecked);
        }
    }
}

void MainWindow::updatePackPresetList() {
    QStringList presets = packPresetList();
    ui->presetPackList->clear();
    ui->presetPackList->addItems(presets);
}

void MainWindow::on_launchSaveButton_clicked() {
    QString presetName = ui->presetPackLineEdit->text().trimmed();
    if (presetName.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Pack preset name cannot be empty."));
        return;
    }
    QSettings settings("Falcon", "SimsSwitcher");
    if (settings.contains("packPresets/" + presetName)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, tr("Preset Overwrite"),
                                      tr("Pack preset '%1' already exists. Do you want to overwrite it?").arg(presetName),
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::No) {
            return;
        }
    }
    savePackPreset(presetName);
    updatePackPresetList();
}

void MainWindow::on_packsLoadButton_clicked() {
    if (!ui->presetPackList->currentItem()) {
        QMessageBox::warning(this, tr("Error"), tr("No pack preset selected."));
        return;
    }
    QString presetName = ui->presetPackList->currentItem()->text();
    QSettings settings("Falcon", "SimsSwitcher");
    if (!settings.contains("packPresets/" + presetName)) {
        QMessageBox::warning(this, tr("Error"), tr("Pack preset '%1' does not exist.").arg(presetName));
        return;
    }
    loadPackPreset(presetName);
    ui->presetPackLineEdit->setText(presetName);
}

void MainWindow::on_presetPackDeleteButton_clicked() {
    if (!ui->presetPackList->currentItem()) {
        QMessageBox::warning(this, tr("Error"), tr("No pack preset selected."));
        return;
    }
    QString presetName = ui->presetPackList->currentItem()->text();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Delete Pack Preset"),
                                  tr("Are you sure you want to delete the pack preset '%1'?").arg(presetName),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QSettings settings("Falcon", "SimsSwitcher");
        settings.remove("packPresets/" + presetName);
        updatePackPresetList();
    }
}

void MainWindow::onPackPresetSelected(QListWidgetItem* item) {
    if (item) {
        ui->presetPackLineEdit->setText(item->text());
    }
}

//Settings page code below

void MainWindow::on_exportButton_clicked()
{
    qDebug() << "Export button clicked, opening ExportWindow.";
    ExportWindow exportWizard(this);
    exportWizard.exec();
}

void MainWindow::on_importButton_clicked()
{
    // Open a file dialog for selecting one or more preset files to import
    QStringList fileNames = QFileDialog::getOpenFileNames(
        this,
        tr("Import Preset Files"),
        QString(),
        tr("Preset Files (*.json);;MCCC Preset Files (*.cfg);;All Files (*.*)")
    );

    if (fileNames.isEmpty())
        return;

    QSettings settings("Falcon", "SimsSwitcher");
    int importedCount = 0;
    // Iterate through each selected file with extension .json 
    for (const QString& filePath : fileNames) {
        QFileInfo fileInfo(filePath);
        QString ext = fileInfo.suffix().toLower();

        if (ext == "json") {
            QFile file(filePath);
            if (!file.open(QIODevice::ReadOnly))
                continue;

            QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (!doc.isObject())
                continue;

            QJsonObject obj = doc.object();
            QString type = obj.value("type").toString();
            QString name = obj.value("name").toString();
            QJsonArray itemsArray = obj.value("items").toArray();
            QStringList items;
            for (const QJsonValue& v : itemsArray)
                items << v.toString();

            if (type == "modPreset" && !name.isEmpty()) {
                settings.setValue("presets/" + name, items);
                ++importedCount;
            } else if (type == "packPreset" && !name.isEmpty()) {
                settings.setValue("packPresets/" + name, items);
                ++importedCount;
            }
        } else if (ext == "cfg") {
            // Copy .cfg files to rootDir + "/mcccPresets"
            QString rootDir = ui->rootLineEdit->text();
            QDir baseDir(rootDir);
            QString mcccPresetsDir = baseDir.filePath("mcccPresets");
            QDir().mkpath(mcccPresetsDir);
            QString destPath = QDir(mcccPresetsDir).filePath(fileInfo.fileName());
            if (QFile::copy(filePath, destPath)) {
                ++importedCount;
            }
        }
    }
    
    updatePresetList();
    updatePackPresetList();

    if (importedCount > 0) {
        QMessageBox::information(this, tr("Import Complete"),
                                 tr("Successfully imported %1 preset(s).").arg(importedCount));
    } else {
        QMessageBox::warning(this, tr("Import Failed"),
                             tr("No valid presets were imported."));
    }
}

void MainWindow::do_patreonLink()
{
    ui->settingsTextBrowser->setTextInteractionFlags(Qt::TextBrowserInteraction);
    ui->settingsTextBrowser->setOpenExternalLinks(true);
}

void MainWindow::on_reenableButton_clicked()
{
    // Re-enable the expiremental warning dialog.
    QSettings settings("Falcon", "SimsSwitcher");
    settings.setValue("dontShowExperimentalVersion", false);
}

//Close Event

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save the current packs selection before closing.
    savePacksSelection();

    // Now, pass the event to the base class to carry on with the usual closing process.
    QMainWindow::closeEvent(event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasUrls())
        return;

    // Decide target directory: Mods or (d)Mods
    QString rootDir = ui->rootLineEdit->text();
    QDir baseDir(rootDir);
    // You can add a toggle or context to choose which folder to drop into.
    // Here, we default to Mods (active)
    QString targetDir = baseDir.filePath(activeSubDirName);

    // Optionally, check if user is viewing disabled mods and drop there instead:
    // QString targetDir = ...;

    for (const QUrl &url : event->mimeData()->urls()) {
        QString localPath = url.toLocalFile();
        QFileInfo info(localPath);
        QString destPath = QDir(targetDir).filePath(info.fileName());

        if (info.isDir()) {
            // Copy directory recursively
            if (!copyDirectory(localPath, destPath)) {
                QMessageBox::warning(this, tr("Copy Failed"),
                    tr("Failed to copy folder: %1").arg(info.fileName()));
            }
        } else if (info.isFile()) {
            if (!QFile::copy(localPath, destPath)) {
                QMessageBox::warning(this, tr("Copy Failed"),
                    tr("Failed to copy file: %1").arg(info.fileName()));
            }
        }
    }

    // Refresh the file list
    populateFileList(baseDir.filePath(activeSubDirName), baseDir.filePath(disabledSubDirName));
}

MainWindow::~MainWindow()
{
    delete ui;
    ui = nullptr;
}