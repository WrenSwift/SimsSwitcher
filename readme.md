# SimsSwitcher
v1.0.0

**SimsSwitcher** is a Windows desktop application for managing The Sims 4 mods and packs. It allows you to easily enable/disable mods, create and switch between presets for both mods and packs, export/import presets, and launch the game with custom pack selections.

---

## What's New in 1.0.0

- **Export/Import Presets:**  
  Export your mod and pack presets as individual JSON files, and optionally export all mods from a preset as a ZIP archive. Import presets on any device running SimsSwitcher.
- **Bulk Export:**  
  Select multiple presets for export at once, with progress feedback.
- **Multi-Select Preset Lists:**  
  Easily select multiple mod or pack presets for export or deletion.
- **Improved Duplicate Detection:**  
  The app now warns you if the same mod exists in both active and disabled folders, helping prevent mod conflicts.
- **Progress Bar for Export:**  
  Visual feedback during export operations.
- **UI Improvements:**  
  More robust error handling, clearer warnings, and improved workflow for managing and exporting presets.
- **Bug Fixes and Stability:**  
  Many fixes and refinements for a smoother experience.

---

## Features

- **Enable/Disable Mods:**  
  Quickly move mods between active and inactive folders.

- **Mod Presets:**  
  Save and load sets of enabled mods as named presets for different play styles or testing setups.

- **Packs Management:**  
  View, enable, or disable Sims 4 packs using friendly names loaded from a CSV file.

- **Pack Presets:**  
  Save and load sets of enabled packs as named presets, making it easy to switch between different pack configurations.

- **Export/Import Presets:**  
  Export mod and pack presets as individual JSON files, and import them on another device. Optionally export all mods from a preset as a ZIP archive.

- **Game Launcher:**  
  Launch The Sims 4 directly from the app, with optional arguments to disable selected packs.

- **Persistent Settings:**  
  Remembers your last used directories, mod presets, and pack presets between sessions.

---

## Getting Started

### Installation

1. **Download the latest release:**
    - Go to the [Releases page](https://github.com/WrenSwift/SimsSwitcher/releases) of the GitHub repository.
    - Download the `.zip` file for the latest version.

2. **Unpack:**
    - Extract the contents of the `.zip` file to a folder of your choice.

3. **Run:**
    - Double-click `SimsSwitcher.exe` inside the extracted folder to start the application.

No installation is required. If you encounter issues running the app, ensure you have the latest Visual C++ Redistributable installed.

---

## Usage

1. **Select Sims 4 Data Directory:**  
   Go to the Settings tab and click "Browse..." to choose your Sims 4 data directory.

2. **Select Sims 4 Install Directory:**  
   In the Settings tab, click "Browse..." to choose your Sims 4 install directory.

3. **Enable/Disable Mods:**  
   Use the checkboxes in the Mods tab to select which mods are active. Use "Select All" or "Deselect All" for convenience.

4. **Mod Presets:**  
   - Save your current mod selection as a preset by entering a name and clicking "Save Preset".
   - Load a preset by selecting it from the list and clicking "Load Preset".
   - Delete a preset with the "Delete" button.

5. **Enable/Disable Packs:**  
   Use the checkboxes in the Packs tab to select which packs are enabled. Use "Select All" or "Deselect All" for convenience.

6. **Pack Presets:**  
   - Save your current pack selection as a preset by entering a name and clicking "Save Preset".
   - Load a preset by selecting it from the list and clicking "Load Preset".
   - Delete a preset with the "Delete" button.

7. **Export/Import Presets:**  
   - Click "Export" to open the export wizard, select one or more presets, and export them as JSON (and optionally ZIP) files.
   - Click "Import" to select one or more preset files and add them to your SimsSwitcher.

8. **Launch Game:**  
   Click "Launch with selected packs" to start The Sims 4 with your selected packs and mods.

---

## File Structure

- `mainwindow.cpp` / `mainwindow.h` — Main application logic and UI.
- `exportwindow.cpp` / `exportwindow.h` — Export/import wizard logic.
- `resources.qrc` — Qt resource file (icons, CSV).
- `inc/packsDil.csv` — CSV mapping for pack names.
- `icons/` — Application icons.
- `CMakeLists.txt` — Build configuration.

---

## Troubleshooting

- **Presets not saving:**  
  Ensure you have write permissions in your user directory. Presets are stored using `QSettings` under the "Falcon" organization.

- **CSV file not found:**  
  Make sure `inc/packsDil.csv` is present.

- **UI elements missing or not working:**  
  Ensure you are using the latest version and that all files from the release are present.

- **Duplicate Mods Warning:**  
  If you see a warning about mods existing in both active and disabled folders, resolve the duplicates to avoid conflicts.

---

## License

MIT License

---

## Credits

- Developed by WrenSwift
- Uses [Qt](https://www.qt.io/)