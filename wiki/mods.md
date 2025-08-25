<!--- Wiki page for the Mods management page for SimsSwitcher application.-->
# Mods
This page provides information about managing mods in the SimsSwitcher application.
## Overview
The Mods section allows users to view, enable, disable, and manage mods for their Sims 4 game. This allows you to easily manage mods from multiple sources and keep your game organized, as well as allowing you to save mods presets to quickly switch between different mod setups.
## Features
- **View Mods**: See a list of all installed mods and statuses (enabled/disabled). Names are derived from the file or folder names within the Mods directory. You can bundle multiple files into a single folder to keep things organized and allow for easier enabling/disabling of related mod files.
- **Enable/Disable Mods**: Toggle the status of individual mods or entire folders of mod files by selecting the checkbox next to each mod or folder. Mods with the checkbox checked are enabled, while those without the check are disabled when the "Set Checked as Active" button is clicked. Note that changes are not applied immediately; you must click the button to apply changes. Disabled mods are moved to "(d)Mods" folder within the Sims 4 Data directory.
- **Presets**: Save and load mod presets to quickly switch between different mod setups. This is useful for testing different mod combinations or for switching between different mods for different save games. Presets can be created by inputting a name in the text box and clicking "Save Preset". Load a preset by selecting it from the list and clicking "Load Preset". Deleting a preset is done by selecting it from the list and clicking the "Delete" button located at the top right of the preset list.
- **MCCC Integration**: If you have the MCCC mod installed, SimsSwitcher can automatically detect and manage it. You can save and load MCCC presets along with your other mods allowing for easy changes of MCCC configurations. See the [MCCC page](mccc.md) for more information on managing MCCC presets.
## Usage
1. Ensure that your Data directory is correctly set in the [Settings page](settings.md).
2. Navigate to the Mods page.
3. View the list of installed mods and their statuses.
4. Enable or disable mods by checking or unchecking the boxes next to them.
5. Click "Set Checked as Active" to apply changes.
6. To manage presets, enter a name in the text box and click "Save Preset" to save the current mod configuration. Select a preset from the list and click "Load Preset" to apply it, or click "Delete" to remove it. (You can overwrite an existing preset by saving a new preset with the same name.)
7. If you have MCCC installed, manage its presets alongside your other mods by selecting "MCCC Settings..." button below the preset list. See the [MCCC page](mccc.md) for more details.
## Notes
- MCCC Settings... button will only appear if MCCC is detected in your Mods folder or a subfolder. This is done by checking for the presence of "mc_cmd_center.package" file.
- Mods are moved to a "(d)Mods" folder when disabled. This folder is created automatically if it does not exist.
- Ensure that you have the necessary permissions to modify files in the Sims 4 Data directory.
- If you are using CurseForge to manage mod downloads and updates, it will not be aware of mods being moved to the "(d)Mods" folder. You may need to manually manage updates for those mods or re-enable them temporarily to allow CurseForge to update them.
- As the application modifies files in your Sims 4 Data directory, it is recommended to back up your mods and game data before making changes.
## Troubleshooting
### Mods Not Appearing
- Ensure that your Data directory is correctly set in the [Settings page](settings.md).
- Check that the mods are placed in the correct "Mods" or "(d)Mods" folder within the Sims 4 Data directory.
- Ensure that you have the necessary permissions to read files in the Sims 4 Data directory.
- Restart the SimsSwitcher application to refresh the mod list.
### Changes Not Applying
- Ensure that you click the "Set Checked as Active" button after making changes to enable or disable mods.
- Check that the mods are not being used by the Sims 4 game while making changes.
- Verify that the "(d)Mods" folder exists and is writable.
- Ensure that no other application is locking the mod files.
### Presets Not Saving or Loading
- Ensure that you have entered a valid name in the text box before clicking "Save Preset".
- Make sure you select a preset from the list before clicking "Load Preset" or "Delete".
- Ensure that you have the necessary permissions in your user data directory where presets are stored.
- Restart the SimsSwitcher application to refresh the preset list.
### MCCC Not Detected
- Ensure that MCCC is installed in the "Mods" folder or a subfolder within the Sims 4 Data directory.
- Check for the presence of the "mc_cmd_center.package" file in the Mods directory.