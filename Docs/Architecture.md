# Architecture

## What is copied

A vehicle's "timetable" is its route: `AFGWheeledVehicleIdentifier::mVehicleRoute`, a list of docking station GUIDs.
The fuel is the item in slot 0 of `AFGWheeledVehicle::GetFuelInventory()`; if the slot is empty, the identifier's
last fuel type. The fuel type picker in the route tab only previews route cost; the fuel actually burned is whatever
is in the slot, so that is what is copied.

The vehicle's name, autopilot on/off state and position are not copied.

## UI

`UVSCPanel` (two vanilla `Widget_StandardButton`s) is added with an SML widget blueprint hook to `CanvasPanel_0` of
`BPW_VehiclePathManager`, the route tab inside the vehicle menu (`Widget_VehicleTrunk`), at the right end of the
"Current route" header. The tab's `mVehicleIdentifier` property tells which vehicle it is for.

The copied settings live in a client-side global, so they carry from one vehicle menu to the next, and survive until
the game closes. They are not saved.

## Paste

The client calls `UVSCRemoteCallObject::Server_PasteSettings(identifier, route, fuelClass)`. On the server:

1. `SetVehicleRoute`, then `SanitizeVehicleRoute`, which drops stops that don't exist on the vehicle's path network
   (for example pasting a truck route onto a tractor on a separate network). The message says how many were kept.
2. Fuel, from the pasting player's inventory:
   - Slot already holds the fuel: topped up to a full stack.
   - Slot empty: filled with up to a full stack.
   - Slot holds a different fuel: that fuel goes back to the player (only if it fits), then the slot is filled.
   - The player has none of the fuel, or the vehicle doesn't accept it: the slot is left alone.
3. `Client_Notify` shows the result as a game notification.

Both the server and the player need the mod (the paste is a remote call).

## Unverified assumptions

- Calling `PopulateCurrentStations` / `UpdateCurrentRoute` on the route tab redraws its stop list after a paste.
- The autopilot copes with a route replaced while the vehicle is mid-trip, as it does for the vanilla stop edits.
