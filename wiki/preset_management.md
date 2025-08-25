<!--- Wiki page for the preset management i.e export wizard and import dialog-->
# Preset Management
This page provides information about managing presets in the SimsSwitcher application.
## Overview
The Preset Management features allow users to export and import presets for mods, MCCC, and packs. This enables you to easily share your configurations with others or transfer them between different installations of SimsSwitcher. Presets can be exported to a json file, along with an optional zip file containing the necessary mods, and imported back into the application when needed. This is done on the [Settings page](settings.md).
## Features
- **Export Presets**: Export any number of your saved presets for mods, MCCC, or packs to a specified location. This creates json files containing the selected presets and optionally a zip file containing the mods needed for your presets that can be imported later or shared with others. Click the "Export Presets..." button on the Settings page to choose which presets to export and where to save the files.
- **Import Presets**: Import presets from a previously exported json file. This allows you to quickly add multiple presets to your SimsSwitcher application. Click the "Import Presets..." button on the Settings page to select the json file containing the presets you want to import and/or a zip file containing mods.
## Usage
### Exporting Presets
1. Navigate to the Settings page in the SimsSwitcher application.
2. Click the "Export Presets..." button to open the Export Presets dialog.
3. Select any mod presets that you want to export by selecting them from the list. You can slect multiple presets. All Highlighted presets will be exported.
4. Click Next to proceed to the next step.
5. Select the Pack presets that you want to export by selecting them from the list. You can select multiple presets. All Highlighted presets will be exported.
6. Click Next to proceed to the next step.
7. Select the MCCC presets that you want to export by selecting them from the list. You can select multiple presets. All Highlighted presets will be exported.
8. Click Next to proceed to the next step.
9. Select the Export Directory where you want to save the exported files by clicking the "Browse..." button and choosing a folder.
10. Optionally, check the box to include mods in a zip file. This will create a zip file containing all the mods needed for the selected mod presets.
11. Click the "Export" button to complete the export process. The presets will be saved to a json file in the selected directory, along with an optional zip file containing the mods.
12. Click "Finish" to close the Export Presets dialog.
### Importing Presets
1. Navigate to the Settings page in the SimsSwitcher application.
2. Click the "Import Presets..." button to open the Import Presets dialog.
3. In the file explorer dialog that appears, select the json file containing the presets you want to import.
4. You may need to adjust the file type filter to accommodate the proper file types.
5. Click "Open" to load the selected files.
## Notes
- Ensure that you have the necessary permissions to read and write files in the selected directories.
- MCCC presets will be exported as a .cfg file alongside the other preset files. The cfg file will be named based on the preset name.
## Troubleshooting
### Presets Not Exporting
- Ensure that you have selected at least one preset to export in the Export Presets dialog.
- Check that you have write permissions for the selected export directory.
- Ensure that there is sufficient disk space in the selected export directory.
- Restart the SimsSwitcher application and try exporting again.
### Presets Not Showing in Import Dialog
- Ensure that you are selecting the correct file type in the file explorer dialog when importing presets. You may need to adjust the file type filter to accommodate the proper file types. json for pack and mod presets, and cfg for MCCC presets.

