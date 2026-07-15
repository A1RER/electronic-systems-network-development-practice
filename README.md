# Electronic Systems and Network Development Practice

This repository contains coursework and project files for the short-semester comprehensive practice project **Electronic Systems and Network Development Practice**.

## Project Overview

The main embedded project is an STM32-based signal generator. It outputs selectable waveforms and provides LCD display, button control, sweep mode, and parameter persistence without changing the original hardware pin connections.

## STM32 Signal Generator Features

- Waveform output: square wave, sawtooth wave, and sine wave
- Frequency adjustment: 500 Hz to 2000 Hz
- Frequency step switching: 10 Hz, 100 Hz, and 500 Hz
- Output enable/disable control
- Square-wave duty-cycle adjustment
- Sine/sawtooth amplitude adjustment
- Sweep mode between 500 Hz and 2000 Hz
- LCD status display
- Flash parameter persistence after power-off

## Button Controls

| Button | Function |
| --- | --- |
| KEY1 short press | Switch waveform |
| KEY1 long press | Toggle output on/off |
| KEY2 | Decrease frequency |
| KEY3 | Increase frequency |
| KEY2 + KEY3 | Switch frequency step |
| KEY4 / KEY5 in square-wave mode | Adjust duty cycle |
| KEY4 / KEY5 in sine/sawtooth mode | Adjust amplitude |
| KEY4 + KEY5 | Toggle sweep mode |

## LCD Display

The LCD shows the current waveform, output state, frequency mode, frequency value, and duty-cycle or amplitude value.

Examples:

```text
SQR O:ON ST:100
F:1000Hz D:50%
```

```text
SIN O:ON SW:100
F:1500Hz A:75%
```

## Directory Overview

```text
PTProjects/
  260706/
  260707/
  260708/

STM32/
  example/
    User/
    Library/
    Start/
    Wave_Create/
    system/
    project1.uvprojx
```

## Build Notes

The STM32 project is built with Keil uVision. The generated firmware file is:

```text
STM32/example/Objects/project1.hex
```

Use this HEX file for downloading to the STM32 board after a successful build.

## Notes

This repository is used to track coursework, experiments, and project files during the semester.
