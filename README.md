# Vehicle Settings Copy

Satisfactory mod (SML). Adds **Copy** and **Paste** buttons to the route tab of trucks, tractors and explorers. Copy
takes the vehicle's timetable stops and the fuel in its fuel slot; Paste puts them on another vehicle and fills its
fuel slot from your inventory.

- How it works: [Docs/Architecture.md](Docs/Architecture.md)
- Logs and a test checklist: [Docs/Debugging.md](Docs/Debugging.md)
- Mod page texts: [Docs/Release](Docs/Release)

## Build

From `D:\SFMod\Tools\BuildScripts` (see its README.md):

- `sfmod package VehicleSettingsCopy local`: Windows client test zip
- `sfmod package VehicleSettingsCopy`: release zips (Windows client, Windows and Linux server)
- `sfmod release VehicleSettingsCopy`: GitHub release of the packaged zips

Output: `SatisfactoryModLoader\Saved\ArchivedPlugins\VehicleSettingsCopy\`. Icon: `python Tools/make_icon.py`.
