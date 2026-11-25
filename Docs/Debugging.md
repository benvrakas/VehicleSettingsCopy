# Debugging Vehicle Settings Copy

Log category `LogVehicleSettingsCopy` in `%LOCALAPPDATA%\FactoryGame\Saved\Logs\FactoryGame.log` (client) and the
server's `FactoryGame/Saved/Logs/FactoryGame.log`.

Client:
- `UI hook added to CanvasPanel_0 ...`: the buttons are registered. An Error `panel ... not found` means the game
  changed the route tab.
- `Copy/Paste panel shown for <identifier>`: the buttons appeared in a vehicle menu.
- `Copied N stops and fuel X from <name>`, `Paste: sending ...`.
- `Remote call object not registered on the local player`: the server doesn't have the mod.

Server:
- `Paste by <player> onto <vehicle>: K of N stops kept, fuel X, M moved. <message>`.
- `Paste ... rejected: ... has no vehicle`.

## Test checklist

1. Truck A with 3 stops and coal in the fuel slot. Open its menu, Route tab, press Copy ("Copied 3 stops...").
2. Truck B with no stops and an empty fuel slot, coal in your inventory. Open its Route tab, press Paste: 3 stops
   listed, fuel slot full, coal gone from your inventory.
3. Repeat with B holding a different fuel: it goes into your inventory, coal replaces it.
4. Paste with no coal in your inventory: stops pasted, fuel slot unchanged, message says so.
5. Same on a tractor and an explorer; paste a truck route onto a tractor on a different network (stops dropped).
6. Paste while B is driving its route.
7. As a client on a dedicated server.
