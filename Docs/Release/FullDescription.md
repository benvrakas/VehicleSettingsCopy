## Vehicle Settings Copy

Setting up a fleet of trucks means adding the same stops and the same fuel to each one. This mod adds **Copy** and
**Paste** buttons to the route tab of a truck, tractor or explorer, so you set up one and copy it to the rest.

### How to use

1. Open a vehicle that is set up, go to its **Route** tab and press **Copy**.
2. Open another vehicle, go to its Route tab and press **Paste**. You can paste onto as many vehicles as you like.

### What is copied

- **All the timetable stops**, in order.
- **The fuel**: the type of fuel in the vehicle's fuel slot. On paste, the fuel slot is filled with that fuel **from your
  inventory**. If the slot already holds some, it is topped up. If it holds a different fuel, that goes back into your
  inventory first. If you have none of it, the slot is left as it was.

The vehicle's name and whether its autopilot is on are not copied.

A message tells you what was pasted. Stops that don't exist on the target's path network are skipped (a truck route
pasted onto a tractor on a separate road, for example).

### Who needs it

Both the server (or host) and the players who use the buttons.

### Source

[github.com/benvrakas/VehicleSettingsCopy](https://github.com/benvrakas/VehicleSettingsCopy). Bug reports and
questions go to its [issues](https://github.com/benvrakas/VehicleSettingsCopy/issues).
