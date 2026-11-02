# Digital Watch

Custom smartwatch built around the Nordic nRF5340, with a custom PCB designed in Altium Designer and firmware written on Zephyr / nRF Connect SDK.

## Status

- **Hardware:** schematic capture complete, PCB layout in progress
- **Firmware:** bring-up on the nRF5340 DK with sensor breakout boards
- **Display:** not finalized (TODO)

## Hardware

| Function | Part |
|---|---|
| MCU | nRF5340 |
| PMIC | nPM1300 |
| IMU | BMI270 |
| GPS | SAM-M10Q |
| PPG (heart rate / SpO2) | MAX30102 |
| Battery | 420 mAh LiPo, JST-PH 2-pin |

Design notes:

- Dual I2C bus architecture, 1.8 V bus with 3.3 kΩ pull-ups
- Partitioned power tree (rails: 1.8 V, 3.0 V, 3.3 V)
- No external flash on v1
- Ship mode disabled on v1 (SHPHLD tied to GND)

## Repository layout

```
Digital_Watch/
├── Firmware/
│   └── src/
│       ├── CMakeLists.txt
│       ├── prj.conf
│       ├── nrf5340dk_nrf5340_cpuapp.overlay
│       └── main.c
└── (Altium project files)   TODO: list schematic / PCB / libraries
```

## Building the firmware

Requirements:

- nRF Connect SDK v3.4.1 and matching toolchain
- VS Code with the nRF Connect extension, or `west`

Build from `Firmware/src`:

```
west build -b nrf5340dk/nrf5340/cpuapp --pristine -- -DCONF_FILE=prj.conf -DDTC_OVERLAY_FILE=nrf5340dk_nrf5340_cpuapp.overlay
```

Flash with the DK connected over USB:

```
west flash
```

Build output lands in `src/build/` (git-ignored).

## Firmware bring-up setup

The DK is powered from a multi-output bench supply with current readouts for the 1.8 V, 3.0 V, and 3.3 V rails. Sensors are connected as breakout boards. There is no nPM1300 breakout in the setup.

## Roadmap

- [ ] Finish PCB layout and order v1 boards
- [ ] Select display and add driver
- [ ] Sensor drivers: BMI270, MAX30102, SAM-M10Q
- [ ] Bluetooth LE
