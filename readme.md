# SimsSwitcher
v1.1.1

**SimsSwitcher** is a Windows desktop application for managing The Sims 4 mods and packs. It allows you to easily enable/disable mods, create and switch between presets for both mods and packs, export/import presets, and launch the game with custom pack selections.

---

## What's New in 1.1

- **Drag-and-Drop Support:**  
  You can now drag files and folders directly into the Mods or (d)Mods list to add them to your game.
- **Delete Mods to Trash:**  
  Each mod entry now has an "X" button to delete files or folders, moving them to the system trash after confirmation.
- **Cloud CSV Support:**  
  Pack names are now loaded from a cloud CSV file, with automatic fallback to a local copy if offline.
- **Improved Preset Handling:**  
  Preset loading and saving now use the new custom list widget, ensuring accurate selection and compatibility.
- **MCCC Settings Presets:**
  Preset loading and saving of MCCC settings file is now available when MCCC is detected in your active mods. Also works with existing import/export functionality. 
- **Better Error Handling:**  
  More robust checks for file operations, drag-and-drop, and preset management.
- **UI Tweaks:**  
  Improved checkbox appearance, layout, and feedback for all file list operations.
- **Bug Fixes and Stability:**  
  Many fixes and refinements for a smoother experience.

---

## Features

- **Enable/Disable Mods:**  
  Quickly move mods between active and inactive folders.

- **Mod Presets:**  
  Save and load sets of enabled mods as named presets for different play styles or testing setups.

- **Packs Management:**  
  View, enable, or disable Sims 4 packs using friendly names loaded from a cloud CSV file (with local fallback).

- **Pack Presets:**  
  Save and load sets of enabled packs as named presets, making it easy to switch between different pack configurations.

- **MCCC Settings Presets:**
  Save and load your MCCC settings as a named preset allowing you to easily switch between configurations. 

- **Export/Import Presets:**  
  Export mod and pack presets as individual JSON files and MCCC settings as named CFG files allowing you to import them on another device or at a later date. Optionally export all mods from a preset as a ZIP archive.

- **Game Launcher:**  
  Launch The Sims 4 directly from the app, with optional arguments to disable selected packs.

- **Bulk Operations:**  
  Select all/deselect all mods or packs, and perform batch exports or deletions.

- **Duplicate Detection:**  
  Warns if the same mod exists in both active and disabled folders.

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

4. **MCCC Settings Management**
    -If MCCC is detected in your active mods easily manage your MCCC settings file by selecting the MCCC settings button(Only shows if MCCC is detected).
    -Save, load, and delete MCCC Presets by selecting them and using the appropriate buttons.  

5. **Mod Presets:**  
   - Save your current mod selection as a preset by entering a name and clicking "Save Preset".
   - Load a preset by selecting it from the list and clicking "Load Preset".
   - Delete a preset with the "Delete" button.    

6. **Enable/Disable Packs:**  
   Use the checkboxes in the Packs tab to select which packs are enabled. Use "Select All" or "Deselect All" for convenience.

7. **Pack Presets:**  
   - Save your current pack selection as a preset by entering a name and clicking "Save Preset".
   - Load a preset by selecting it from the list and clicking "Load Preset".
   - Delete a preset with the "Delete" button.

8. **Export/Import Presets:**  
   - Click "Export" to open the export wizard, select one or more presets, and export them as JSON (and optionally ZIP) files.
   - Click "Import" to select one or more preset files and add them to your SimsSwitcher.

9. **Drag-and-Drop Mods:**  
   Drag files or folders from Windows Explorer directly into the Mods or (d)Mods list to add them.

10. **Delete Mods:**  
   Click the "X" button next to any mod or folder to move it to the system trash.

11. **Launch Game:**  
    Click "Launch with selected packs" to start The Sims 4 with your selected packs and mods.

---

## File Structure

- `mainwindow.cpp` / `mainwindow.h` — Main application logic and UI.
- `FileListItemWidget.cpp` / `FileListItemWidget.h` — Custom widget for mod list items.
- `exportwindow.cpp` / `exportwindow.h` — Export/import wizard logic.
- `mcccSettings.cpp` / `mcccSettings.h` — MCCC Presets wizard logic.
- `resources.qrc` — Qt resource file (icons, CSV).
- `inc/packsDil.csv` — Local CSV mapping for pack names (used as fallback).
- `icons/` — Application icons.
- `CMakeLists.txt` — Build configuration.

---

## Troubleshooting

- **Presets not saving:**  
  Ensure you have write permissions in your user directory. Presets are stored using `QSettings` under the "Falcon" organization.
  
  MCCC settings presets are stored in a created folder in your "Sims 4 Data Directory" ensure you have appropriate permissions and files are present.

- **CSV file not found:**  
  Make sure `inc/packsDil.csv` is present for offline use.

- **UI elements missing or not working:**  
  Ensure you are using the latest version and that all files from the release are present.

- **Duplicate Mods Warning:**  
  If you see a warning about mods existing in both active and disabled folders, resolve the duplicates to avoid conflicts.

- **Drag-and-Drop Issues:**  
  Only local files and folders can be added via drag-and-drop. Files from archives or browsers may not work.

---

## License

MIT License

---

## Credits

- Developed by WrenSwift
- Uses [Qt](https://www.qt.io/) for the UI framework

---

<!-- EA disclaimer -->
 This project is not affiliated with or endorsed by Electronic Arts Inc. or Maxis. The Sims 4 and related content are trademarks of Electronic Arts Inc. All rights reserved.