#include "mainwindow.h"
#include "ui_mainwindow.h"
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

QString acitveSubDirName = "Mods"; // Change this to your desired subdirectory
QString disabledSubDirName = "(d)Mods"; // Change this to your desired subdirectory
QString csvFilePath = "inc/packsDil.csv";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SimsSwitcher)
{
    ui->setupUi(this);

    updatePresetList();

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
    QString activeModsPath = baseDir.filePath(acitveSubDirName);
    QString disabledModsPath = baseDir.filePath(disabledSubDirName);

    if (!cachedSource.isEmpty()) {
        ui->rootLineEdit->setText(cachedSource);
        populateFileList(activeModsPath,disabledModsPath);
    }
    if (!cachedGameSource.isEmpty()) {
        ui->gameLineEdit->setText(cachedGameSource);
        populatePacksListWidget(cachedGameSource,csvFilePath);
    }

    qDebug() << "Resource exists:" << QFile::exists(":/inc/packsDil.csv");
}

void MainWindow::on_menuMods_clicked(){
    ui->mainStackedWidget->setCurrentIndex(0);
}

void MainWindow::on_menuPacks_clicked(){
    ui->mainStackedWidget->setCurrentIndex(1);
}

//Mods Page code Below

void MainWindow::on_browseRootButton_clicked() {
    // Open a dialog for the user to select the Sims 4 Root Directory.
    QString rootDir = QFileDialog::getExistingDirectory(this, tr("Select Sims 4 Root Directory"));

    if (!rootDir.isEmpty()) {

        // Create a QDir object from the selected directory.
        QDir baseDir(rootDir);

        // Append the subdirectory using QDir::filePath to get a platform-appropriate path.
        QString activeModsPath = baseDir.filePath(acitveSubDirName);
        QString disabledModsPath = baseDir.filePath(disabledSubDirName);

        // Optionally, create the subdirectory if it does not already exist.
        if (!baseDir.exists(acitveSubDirName)) {
            if (!baseDir.mkpath(acitveSubDirName)) {
                QMessageBox::warning(this, tr("Error"), tr("Failed to create subdirectory: %1").arg(acitveSubDirName));
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

void MainWindow::on_fileSelectAllButton_clicked()
{
    int itemCount = ui->fileListWidget->count();

    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);

        // Ensure the item is checkable before changing its state
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Checked);
        }
    }
}

void MainWindow::on_fileDeselectAllButton_clicked()
{
    int itemCount = ui->fileListWidget->count();

    for (int i = 0; i < itemCount; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);

        // Ensure the item is checkable before changing its state
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(Qt::Unchecked);
        }
    }
}

void MainWindow::populateFileList(const QString& firstDir, const QString& secondDir) {
    ui->fileListWidget->clear();

    // A helper lambda that takes a directory path and a flag indicating if the items should be pre-checked.
    auto addDirectoryItems = [this](const QString &dirPath, bool preChecked) {
        QDir dir(dirPath);
        // Retrieve all files and folders (ignoring "." and "..")
        QFileInfoList fileList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::AllEntries);

        if (!fileList.isEmpty()) {
            // Optionally add a header item to indicate which directory these files are coming from.
            QListWidgetItem* headerItem = new QListWidgetItem("Items from: " + dirPath);
            // Make the header non-checkable
            headerItem->setFlags(headerItem->flags() & ~Qt::ItemIsUserCheckable);
            headerItem->setBackground(Qt::darkGreen);
            ui->fileListWidget->addItem(headerItem);
        }

        // Add each file/folder from the current directory
        for (const QFileInfo& fileInfo : fileList) {
            QString displayName = fileInfo.fileName();
            QListWidgetItem* item = new QListWidgetItem(displayName);
            // Allow items to be checkable
            item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
            // Set the check state based on the flag
            item->setCheckState(preChecked ? Qt::Checked : Qt::Unchecked);

            // Step 1: Save the full file path in the item’s data.
            // This ensures that later you know which exact file or folder
            // (including its originating directory) is being processed.
            QString fullPath = dir.filePath(displayName);
            item->setData(Qt::UserRole, fullPath);

            // Optional: For debugging, print the full path.
            qDebug() << "Added item:" << displayName << "with full path:" << fullPath;

            ui->fileListWidget->addItem(item);
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
        if ((item->flags() & Qt::ItemIsUserCheckable) && item->checkState() == Qt::Checked) {
            enabledItems << item->text();
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
        if (item->flags() & Qt::ItemIsUserCheckable) {
            item->setCheckState(enabledItems.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
        }
    }

    // Run the activeButton function to move the checked items to the active directory
    on_activeButton_clicked();
}

void MainWindow::on_presetDeleteButton_clicked()
{
    // Get the selected preset name from the list widget
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
            if ((item->flags() & Qt::ItemIsUserCheckable) && item->text() == presetItem) {
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
    QString m_firstDir = baseDir.filePath(acitveSubDirName);
    QString m_secondDir = baseDir.filePath(disabledSubDirName);

    // Verify that these directories have been set.
    if (m_firstDir.isEmpty() || m_secondDir.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Source directories are not set."));
        return;
    }

    // Collect all checkable (i.e. real file/folder) items from the list.
    QList<QListWidgetItem*> itemsToProcess;
    int totalItems = ui->fileListWidget->count();
    for (int i = 0; i < totalItems; ++i) {
        QListWidgetItem* item = ui->fileListWidget->item(i);
        // Skip items that are not marked as user-checkable (like our header items)
        if (!(item->flags() & Qt::ItemIsUserCheckable))
            continue;
        itemsToProcess.append(item);
    }

    if (itemsToProcess.isEmpty()) {
        QMessageBox::information(this, tr("No Items"), tr("No items to move."));
        return;
    }

    // Create a progress dialog for visual feedback.
    QProgressDialog progress("Moving items...", "Cancel", 0, itemsToProcess.size(), this);
    progress.setWindowModality(Qt::WindowModal);

    int progressValue = 0;
    for (QListWidgetItem* item : itemsToProcess) {
        // Retrieve the full original source path that was stored in the UserRole.
        QString srcPath = item->data(Qt::UserRole).toString();
        QFileInfo fileInfo(srcPath);

        // Ensure the file/folder exists.
        if (!fileInfo.exists()) {
            qDebug() << "File not found:" << srcPath;
            continue;
        }

        // Determine the desired destination based solely on the item's check state.
        // Checked items should end up in m_firstDir (Active), unchecked in m_secondDir (Inactive).
        QString desiredDestDir = (item->checkState() == Qt::Checked) ? m_firstDir : m_secondDir;

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
                // Optionally, you could implement a copy-then-delete fallback here.
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
        populatePacksListWidget(gameDir,csvFilePath);

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

//Close Event

void MainWindow::closeEvent(QCloseEvent *event)
{
    // Save the current packs selection before closing.
    savePacksSelection();

    // Now, pass the event to the base class to carry on with the usual closing process.
    QMainWindow::closeEvent(event);
}

MainWindow::~MainWindow()
{
    delete ui;
}