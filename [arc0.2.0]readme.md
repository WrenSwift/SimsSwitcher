# SimsSwitcher
v0.2.0

**SimsSwitcher** is a Windows desktop application for managing The Sims 4 mods and packs. It allows you to easily enable/disable mods, create and switch between presets, and launch the game with custom pack selections.

---

## Features

- **Enable/Disable Mods:**  
  Quickly move mods between active and inactive folders.

- **Presets:**  
  Save and load sets of enabled mods as named presets for different play styles or testing setups.

- **Packs Management:**  
  View, enable, or disable Sims 4 packs using friendly names loaded from a CSV file.

- **Game Launcher:**  
  Launch The Sims 4 directly from the app, with optional arguments to disable selected packs.

- **Persistent Settings:**  
  Remembers your last used directories and presets between sessions.

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

1. **Select Sims 4 Root Directory:**  
   Click "Browse" and choose your Sims 4 mods folder.

2. **Enable/Disable Mods:**  
   Use the checkboxes to select which mods are active. Use "Select All" or "Deselect All" for convenience.

3. **Presets:**  
   - Save your current selection as a preset by entering a name and clicking "Save Preset".
   - Load a preset by selecting it from the list and clicking "Load Preset".
   - Delete a preset with the "Delete" button.

4. **Packs Management:**  
   - Select your Sims 4 install directory.
   - Enable/disable packs using the checkboxes. Friendly names are loaded from `inc/packsDil.csv`.

5. **Launch Game:**  
   Click "Launch" to start The Sims 4 with your selected packs and mods.

---

## File Structure

- `mainwindow.cpp` / `mainwindow.h` — Main application logic and UI.
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

---

## License

MIT License

---

## Credits

- Developed by WrenSwift
- Uses [Qt](https://www.qt.io/) for the UI framework

---
