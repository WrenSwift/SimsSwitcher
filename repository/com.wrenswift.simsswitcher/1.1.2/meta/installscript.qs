// Test log to verify script execution
console.log("[installscript.qs] Script loaded");


// If the maintenance tool exists, launch it and exit the installer

function launchMaintenanceIfInstalled() {
    var maintTool = installer.value("TargetDir") + "/maintenancetool.exe";
    if (installer.fileExists(maintTool)) {
        console.log("[installscript.qs] Maintenance tool found, launching instead of installer.");
        installer.executeDetached(maintTool, []);
        QMessageBox.information(null, "SimsSwitcher Already Installed", "SimsSwitcher is already installed. The maintenance tool has been launched for update or uninstall. Please close this window.");
        // Prevent further installation actions by throwing an exception
        throw "Maintenance tool launched. Aborting installation.";
    }
}

function Component() {
    // Set RemoveTargetDir on the component
    component.setValue("RemoveTargetDir", "true");
    // Check for maintenance tool and launch if present
    launchMaintenanceIfInstalled();
}

Component.prototype.beginInstallation = function() {
    // Add a checkbox to the Start Menu Directory page after all pages are created
    installer.addWizardPageItem(component, "StartMenuDirectoryPage", "CheckBox", "desktopShortcut", "Create Desktop Shortcut", false);
};


Component.prototype.createOperations = function()
{
    component.createOperations();
    if (installer.value("desktopShortcut") == "true") {
        // Create a desktop shortcut
        component.addOperation("CreateShortcut",
            "@TargetDir@/SimsSwitcher.exe",
            "@DesktopDir@/SimsSwitcher.lnk",
            "workingDirectory=@TargetDir@",
            "iconPath=@TargetDir@/SimsSwitcher.exe"
        );
    }
}