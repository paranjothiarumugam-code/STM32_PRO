# 3-Tank Overflow Controller — Single 2 HP Pump

Bare-metal (register-level, no HAL/CubeMX) firmware for an STM32F103C8
("Blue Pill") that fills 3 separate water tanks from **one shared 2 HP
pump motor**, filling one tank at a time in priority order and cutting
the pump/valve the instant a tank is full — so it can never overflow.

## How it works

- Each tank has:
  - An **ultrasonic sensor** (HC-SR04 / JSN-SR04T) mounted at the top,
    giving a live level reading (distance to water surface).
  - A **float switch** mounted near the top as a hard overflow cutoff.
    The float switch always wins over the ultrasonic reading — if the
    ultrasonic sensor is unplugged, misreads, or fails, the float
    switch still stops that tank's fill. See `tank_is_full()` in
    `src/tank.c`.
- Only **one solenoid valve is ever open at a time**, and the pump only
  runs while a valve is open — there's a single delivery line off the
  one pump, not three independent pumps.
- Tanks fill in **fixed priority order**: tank 1, then tank 2, then
  tank 3 (`pump_ctrl_step()` in `src/pump.c`). When the active tank
  reaches its full threshold (ultrasonic) or its float switch trips,
  the firmware closes that valve, stops the pump, waits
  `VALVE_SWITCH_DELAY_MS` for the line to settle, then moves on to the
  next tank that needs water.
- A minimum pump-off time (`PUMP_MIN_OFF_MS`, default 5 s) prevents
  short-cycling the motor between tank switches.
- USART1 (PA9/PA10, 115200 8N1) prints tank distances, overflow flags,
  and pump state every 2 s for diagnostics. Optional — the control
  logic works without anything connected to it.
- A 20x4 I2C character LCD shows all 3 tank levels + pump status on one
  screen, refreshed every `DISPLAY_REFRESH_MS` (see `src/display.c`).
  Like the UART log, it's best-effort only: `i2c_write()` has a hard
  timeout on every wait, so a missing or stuck display degrades to a
  no-op instead of ever hanging the pump/valve control loop.

## Pin map (edit in `inc/config.h`)

| Signal              | Pin  | Notes                                   |
|---------------------|------|------------------------------------------|
| Float switch tank 1 | PA0  | Input, internal pull-up, active low       |
| Float switch tank 2 | PA1  | "                                          |
| Float switch tank 3 | PA2  | "                                          |
| Ultrasonic 1 TRIG   | PA3  | Output                                    |
| Ultrasonic 1 ECHO   | PA4  | Input                                     |
| Ultrasonic 2 TRIG   | PA5  | Output                                    |
| Ultrasonic 2 ECHO   | PA6  | Input                                     |
| Ultrasonic 3 TRIG   | PA7  | Output                                    |
| Ultrasonic 3 ECHO   | PA8  | Input                                     |
| UART1 TX / RX       | PA9/PA10 | Optional diagnostics                  |
| I2C1 SCL / SDA      | PB6/PB7  | LCD status display (5V-tolerant pins) |
| Pump relay          | PB12 | Output, active high, drives contactor coil circuit |
| Valve 1 relay       | PB13 | Output, active high                       |
| Valve 2 relay       | PB14 | Output, active high                       |
| Valve 3 relay       | PB15 | Output, active high                       |
| Onboard LED         | PC13 | On while pump is running (active low)     |

## Wiring / electrical safety — read before connecting a real 2 HP motor

The MCU's GPIO pins **cannot** switch a 2 HP motor directly. The
intended chain is:

```
PB12 (3.3V logic) -> opto-isolated relay module input
                   -> relay module's own 5V/12V supply drives its coil
                   -> relay's NO contact switches the LOW-VOLTAGE control
                      circuit of a motor CONTACTOR (rated for the motor's
                      full-load current)
                   -> contactor's power contacts switch the actual
                      motor supply (single- or three-phase)
```

A 2 HP motor also needs, at minimum, a thermal overload relay in series
with the contactor and appropriate branch circuit protection (breaker/
fuse) sized per the motor's actual running current and your local
electrical code. None of that is optional and none of it is something
firmware can substitute for — get it wired by someone qualified for the
voltage/current involved. The same applies to the 3 solenoid valves: use
valves rated for your line pressure and drive them through relays, not
directly from the MCU.

### Motor circuit ratings for this pump (230V single-phase, 9.2A running)

No nameplate was available; these ratings are sized off a measured
running current of 9.2A at 230V single-phase — re-check against the
nameplate if you find one, since starting current and duty cycle can
shift the right size.

| Component | Rating |
|---|---|
| Contactor | 16A, AC-3 (motor duty) rated, 2-pole (switches both line and neutral) |
| Thermal overload relay | Adjustable 6–10A range, dial set to 9.2–9.5A |
| Branch MCB | 16A, C-curve, 2-pole |
| Motor wiring | 2.5 sq mm copper |

The overload relay is what actually protects the motor winding on a
stall or overheat — don't omit it even though the contactor alone would
technically switch the load. A C-curve (not B-curve) breaker is
important too: a B-curve trips on the motor's normal starting inrush.

Float switches and ultrasonic sensors are low-voltage/low-current and
can be wired straight to the GPIO pins as configured (see `tank_init()`
in `src/tank.c` for the exact pull-up/floating-input setup).

### Status display wiring

The 20x4 LCD's I2C backpack (PCF8574) can be powered from the same 5V
rail as the rest of the low-voltage electronics — PB6/PB7 are 5V-tolerant
on the STM32F103, so no level shifter is needed between the 5V I2C bus
and the 3.3V MCU. Wire SDA→PB7, SCL→PB6, plus the module's VCC/GND. Most
backpacks already carry their own pull-up resistors on SDA/SCL; if yours
doesn't, add 4.7kΩ pull-ups to 5V on both lines. Check the module for its
PCF8574 address (0x27 or 0x3F are the two common variants) and set
`LCD_I2C_ADDR` in `inc/config.h` to match if the screen stays blank.

## Tuning

Edit `inc/config.h`:

- `TANK_FULL_CM` / `TANK_REFILL_CM` — ultrasonic distance (sensor to
  water) that counts as "full" vs. "needs a refill". Distance shrinks
  as the tank fills, so full = **small** number, empty = **large**
  number. Measure your actual tank depth and sensor mounting height and
  adjust per tank if they differ (currently shared across all 3 —
  split into per-tank constants if your tanks are different sizes).
- `PUMP_MIN_OFF_MS` — anti-short-cycle delay for the motor.
- `VALVE_SWITCH_DELAY_MS` — settle time when switching tanks.
- `ENABLE_UART_LOG` — set to 0 to disable serial logging and free PA9/PA10.
- `TANK_EMPTY_CM` — ultrasonic distance the status display treats as 0%
  full. Only affects the displayed percentage, not fill/overflow control.
- `ENABLE_DISPLAY` — set to 0 to disable the LCD and free PB6/PB7.
- `LCD_I2C_ADDR` / `DISPLAY_REFRESH_MS` — display I2C address and refresh
  interval.

## Build

Requires the `arm-none-eabi-gcc` toolchain (`apt install gcc-arm-none-eabi`
on Debian/Ubuntu).

```sh
make            # builds build/tank_controller.elf/.bin/.hex
make flash      # flashes via st-flash (ST-Link)
make flash-cube # flashes via STM32CubeProgrammer's CLI (see below)
make clean
```

### Flashing with STM32CubeProgrammer

Install [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)
and connect the Blue Pill via an ST-Link (or a USB-serial adapter for
UART bootloader flashing). Two ways to flash the build:

- **GUI**: open STM32CubeProgrammer, connect (ST-Link, SWD, default
  settings), then under "Erasing & Programming" browse to
  `build/tank_controller.hex`, tick "Verify programming" and "Run after
  programming", and click **Start Programming**.
- **CLI** (same tool, scriptable): run `make flash-cube`. It calls
  `STM32_Programmer_CLI` directly — make sure that binary is on your
  PATH (it ships inside the CubeProgrammer install, e.g. under
  `STM32CubeProgrammer/bin` on Windows/Linux or inside the `.app` on
  macOS). If you're flashing over UART instead of SWD, replace
  `port=SWD` in the Makefile's `flash-cube` target with your serial
  port, e.g. `port=/dev/ttyUSB0` or `port=COM5`.

Either way, only `build/tank_controller.hex` is needed — it already
encodes the 0x08000000 flash start address, so there's nothing else to
configure.

No CubeMX project, HAL, or CMSIS device pack is required — `inc/stm32f103_regs.h`
defines just the registers this project touches, and `startup/startup_stm32f103xb.s`
+ `linker/STM32F103C8_FLASH.ld` are a from-scratch vector table and memory
layout for the STM32F103C8's 64 KB flash / 20 KB RAM.

## Known limitations / next steps

- If a tank's ultrasonic sensor reads no echo (unplugged, out of range),
  that tank is treated as "don't start a fill" rather than guessing —
  it will only be filled again once the sensor is working, though its
  float switch will still protect it against overflow from any other
  water source.
- No manual override / bypass switch is wired in. Add one on a spare
  GPIO (e.g. PB0) if you want to force-fill or force-stop a tank by
  hand — `pump_ctrl_step()` is the place to add that check.
- Per-tank full/refill thresholds are currently shared constants; split
  them into arrays in `config.h` if your 3 tanks differ in size/shape.
