# ASCEND UAV – Autonomous Charging Station

An autonomous contact-based docking and charging station developed as part of the **ISRO IRoC-U 2026 – ASCEND (Autonomous Surveyor Challenge for Exploration, Navigation and Dynamics)** project.

The system enables an autonomous UAV to return to the base station, dock using a guided mechanical mechanism, establish electrical contact, automatically determine terminal polarity, and charge its 4S LiPo battery using a controlled charging architecture.

---

## 🚁 Project Overview

The ASCEND platform consists of an autonomous UAV and a ground-based station responsible for:

- Autonomous docking
- Electrical contact establishment
- Charging terminal detection
- Automatic polarity verification
- Reverse-polarity handling
- Controlled battery charging
- Battery voltage monitoring
- H-bridge based charging control
- Charging termination
- Mechanical repositioning of the UAV after landing

The charging station was designed to support repeated **dock → charge → redeploy** operation with minimal human intervention.

---

## 🎯 Objective

The objective of this subsystem was to develop a reliable base station capable of:

1. Receiving the returning UAV.
2. Mechanically aligning the UAV with the charging terminals.
3. Detecting whether electrical contact has been established.
4. Determining the polarity presented by the UAV.
5. Selecting the appropriate charging polarity.
6. Enabling controlled charging only after valid contact conditions.
7. Monitoring the charging state.
8. Terminating charging when the battery reaches the required condition.
9. Returning the docking mechanism to its home position.

---

## ⚡ Charging Architecture

The charging system consists of the following major stages:

```text
External DC Supply
        │
        ▼
   DC-DC Converter
        │
        ▼
  Charging Controller
        │
        ▼
     H-Bridge
        │
        ▼
Polarity / Contact Interface
        │
        ▼
    UAV Battery
        │
        ▼
       BMS
```

---

The charging system was designed around a 4S LiPo battery, with a maximum charging voltage of approximately 16.8 V.

The charging architecture uses a CC-CV (Constant Current – Constant Voltage) charging strategy.

During the constant-current stage, the battery is charged at controlled current. As the battery voltage approaches the maximum charging voltage, the system transitions toward constant-voltage operation and the charging current decreases.

### 🔋 Charging Specifications

| Parameter | Value |
|---|---|
| Battery configuration | 4S LiPo |
| Maximum battery voltage | 16.8 V |
| Charging architecture | CC-CV |
| Charging current | Up to 5 A to UAV battery |
| Total charging-system current | Approximately 7 A |
| BMS | 4S LiPo BMS |
| Charging interface | Contact-based |
| Polarity handling | Automatic |
| Charging control | H-Bridge |
| Controller | Arduino |

The charging system was designed to supply approximately 7 A total, with approximately 2 A allocated for the Jetson system and 5 A available for UAV battery charging.

---

## 🔌 Polarity Detection

One of the important challenges in the autonomous charging system is that the UAV may dock with either orientation.

Therefore, the charging station must determine the polarity appearing at the docking terminals before enabling the charging path.

The polarity-detection circuit uses:

- Docking terminals
- Resistor networks
- Reverse-protection diodes
- Optocouplers
- Microcontroller input signals

The optocouplers provide electrical isolation between the high-voltage charging side and the low-voltage control electronics.

**Polarity Detection Concept**

```text
             UAV Docking Terminals
                  │       │
                  │       │
              Terminal 1  Terminal 2
                  │       │
                  ▼       ▼
             Polarity Detection
              /             \
        Optocoupler 1    Optocoupler 2
              │             │
              ▼             ▼
           MCU Input      MCU Input
              │             │
              └──────┬──────┘
                     ▼
              Polarity Logic
                     │
                     ▼
                H-Bridge

---

## ⚙️ H-Bridge Charging Control

The H-bridge is used to select the charging polarity according to the detected terminal orientation.

The microcontroller monitors the two docking-terminal signals and selects the appropriate H-bridge control path.

**Simplified logic:**

```text
              Terminal State
                    │
          ┌─────────┴─────────┐
          │                   │
     Polarity A          Polarity B
          │                   │
          ▼                   ▼
     H-Bridge Path 1      H-Bridge Path 2
          │                   │
          └─────────┬─────────┘
                    ▼
             UAV Battery
```

Charging is disabled when the expected terminal/contact conditions are not satisfied.

---

## 🛬 Mechanical Docking Mechanism

The base station uses a mechanically guided docking system to establish reliable electrical contact between the UAV and charging terminals.

The docking mechanism uses a stepper motor-driven positioning system.

The microcontroller controls four stepper-motor outputs and executes a predefined stepping sequence.

**Docking sequence**

```text
UAV lands
   │
   ▼
Mechanical docking mechanism activated
   │
   ▼
Stepper motor moves guiding mechanism
   │
   ▼
UAV aligned with charging contacts
   │
   ▼
Terminal contact detected
   │
   ▼
Polarity verified
   │
   ▼
Charging enabled
```

After the required docking movement, the mechanism reverses and returns to its home position.

---

## 🧠 Microcontroller Firmware

The base station controller is implemented using an Arduino-compatible microcontroller.

The firmware performs:

- Terminal-state monitoring
- Charging-state monitoring
- H-bridge control
- Polarity-dependent charging
- Stepper motor control
- Docking sequence execution
- Return-to-home movement
- Serial monitoring and debugging

### 🔧 Pin Configuration

**Charging / Polarity Detection**

| Function | Arduino Pin |
|---|---|
| Voltage / charging-state input | A1 |
| Terminal 1 | A2 |
| Terminal 2 | A3 |
| H-Bridge PWM / charging path 1 | D9 |
| H-Bridge PWM / charging path 2 | D10 |
| H-Bridge control | D8 |
| H-Bridge control | D7 |

**Stepper Motor**

| Stepper Signal | Arduino Pin |
|---|---|
| IN1 | D3 |
| IN2 | D4 |
| IN3 | D5 |
| IN4 | D6 |

---

## 🔄 Firmware Logic

The firmware continuously monitors the docking terminals and charging-state input.

```text
                  START
                    │
                    ▼
             Monitor terminals
                    │
                    ▼
           Is docking active?
              /           \
            NO             YES
            │               │
            ▼               ▼
       Bridge OFF      Check polarity
                            │
                    ┌───────┴───────┐
                    │               │
               Polarity 1      Polarity 2
                    │               │
                    ▼               ▼
              H-Bridge Path 1  H-Bridge Path 2
                    │               │
                    └───────┬───────┘
                            ▼
                         CHARGE
                            │
                            ▼
                  Battery/terminal state
                            │
                       Charge complete?
                         /       \
                       NO         YES
                       │           │
                       ▼           ▼
                    Continue    Bridge OFF
                                  │
                                  ▼
                                 STOP
```

---

## 🪜 Stepper Motor Control

The docking mechanism uses a four-step sequence:

```
Step 1 → {1,0,0,1}
Step 2 → {1,0,1,0}
Step 3 → {0,1,1,0}
Step 4 → {0,1,0,1}
```

The motor performs the required forward movement during docking and subsequently executes the reverse sequence to return to the home position.

The firmware also checks terminal states during the forward movement so that the mechanism can stop when the expected docking/contact condition is detected.

---

## 🛡️ Safety and Protection

The charging architecture incorporates multiple protection layers.

**Battery-side protection**

The UAV uses a BMS providing:

- Over-voltage protection
- Under-voltage protection
- Over-current protection
- Short-circuit protection
- Cell balancing
- Cell-level voltage monitoring

**Charging-side protection**

The base station incorporates:

- Controlled charging voltage
- Current limiting
- Reverse-polarity protection
- Electrical contact verification
- Charging enable/disable logic
- H-bridge isolation from the charging path when charging is disabled

Charging is enabled only after the docking and electrical conditions are considered valid.

---

## 🧪 Testing

The charging system was evaluated under multiple operating conditions.

Testing included:

- BMS short-circuit protection
- Under-voltage protection
- Cell balancing
- Battery recovery after deep discharge
- Constant-current charging
- Constant-voltage charging
- Charging termination
- Docking/contact establishment
- Polarity detection
- H-bridge switching

During constant-voltage testing, the charger maintained approximately 16.8 V, while charging current gradually decreased as the battery approached full charge.

---

## 📷 Hardware

Charging Polarity Detection Circuit

---

## 🧩 System Architecture

```text
                         ASCEND UAV
                             │
                             │
                    Autonomous Landing
                             │
                             ▼
                  ┌─────────────────────┐
                  │   Docking Platform  │
                  │                     │
                  │ Guiding Mechanism   │
                  │         │           │
                  │         ▼           │
                  │ Charging Contacts   │
                  └─────────┬───────────┘
                            │
                            ▼
                    Polarity Detection
                            │
                            ▼
                       Microcontroller
                            │
                  ┌─────────┴─────────┐
                  │                   │
                  ▼                   ▼
             Stepper Motor        H-Bridge
              Controller          Controller
                                      │
                                      ▼
                               DC-DC Converter
                                      │
                                      ▼
                                  DC Supply
                                      │
                                      ▼
                                  4S LiPo
                                      │
                                      ▼
                                    BMS
```

---


## 🚀 Key Features

- Autonomous UAV docking
- Contact-based charging
- Automatic terminal detection
- Reverse-polarity handling
- H-bridge controlled charging
- 4S LiPo charging
- 16.8 V maximum charging voltage
- CC-CV charging architecture
- BMS-based battery protection
- Stepper-controlled mechanical docking
- Automatic docking mechanism return
- Charging-state monitoring
- Safety-oriented charging control

---

## 🏆 Project Context

This charging station was developed as part of the ISRO IRoC-U 2026 ASCEND challenge, which required an autonomous UAV system capable of navigation, landing, power transfer and autonomous operation.

The complete ASCEND system integrates:

- Autonomous UAV navigation
- GPS-denied perception
- Precision landing
- Wireless communication
- Edge computing
- Autonomous docking
- Battery charging

This repository focuses specifically on the ground-based autonomous charging and docking subsystem.

---

## 📚 Documentation

For detailed system architecture, experimental results, BMS testing and charging-system analysis, refer to the project documentation included in this repository.
