# Garage Management System

A console-based multi-floor garage management system built in C++ for the Structured Programming course at Ain Shams University (2025–2026).

## Overview

The system simulates a real-world parking garage operation — from vehicle check-in and automated slot assignment to fee calculation, service scheduling, and administrative reporting. It manages concurrent vehicles across multiple subscription plans while maintaining full data persistence across sessions.

## Features

### Parking Structure
- 100 slots across 4 floors: Floors 1–3 (regular, 25 slots each) and Floor 4 (25 VIP-exclusive slots)
- Automatic floor and slot assignment based on subscription tier

### Vehicle Management
- Supports Cars, Motorcycles, and Trucks
- Captures owner name, plate number, national ID, phone, brand, and color per vehicle
- Duplicate plate detection and active status tracking
- Family group registration — 2 vehicles linked under one bundle

### Subscription Tiers

| Tier | Description |
|------|-------------|
| Hourly | First-hour flat rate + per-extra-hour charge |
| Daily (24h) | Flat daily rate |
| Monthly Morning | Daytime monthly subscription |
| Monthly Overnight | Overnight monthly subscription |
| Family Bundle | Group pricing covering 2 vehicles |

### Add-on Services
- **VIP** — Priority assignment to Floor 4 with an add-on fee
- **Car Wash** — Optional cleaning service with automated scheduling:
  - Hourly / Daily: washed on arrival
  - VIP subscribers: every 3.5 days
  - Monthly subscribers: every 7 days

### Pricing Engine
- Fully configurable at runtime via the admin menu
- 13 independent pricing parameters covering all tiers, VIP, and cleaning add-ons

### Admin System
- Multi-admin authentication (username + password)
- Role-based access control — profit reports restricted to senior admin

### Logging & Reporting
- Full timestamped activity log for all check-ins and check-outs
- Today's task board: vehicles currently due for a wash
- Profit tracking: daily, weekly, and monthly totals

### Data Persistence
- System state saved and loaded across sessions via flat-file I/O
- Separate files for vehicles, slots, tickets, pricing, and profits

### Input Validation
| Field | Rule |
|-------|------|
| Owner Name | Letters only, 2–50 characters |
| License Plate | Alphanumeric, 6–7 characters |
| National ID | Exactly 14 digits (Egyptian format) |
| Phone | Digits with optional leading `+` |
| Brand | 2–30 characters |
| Color | Letters only, 2–20 characters |

## Tech Stack

- **Language**: C++ (C++11 or later)
- **Libraries**: Standard only — `<iostream>`, `<fstream>`, `<ctime>`, `<cmath>`, `<string>`
- **No external dependencies** — pure structured programming, no OOP

## Compilation

### Windows (MinGW / MSVC)
```bash
g++ -std=c++11 -o garage "SP Source Code.cpp"
.\garage.exe
```

### Linux / macOS

> ⚠️ The source uses `localtime_s`, which is Windows-specific (MSVC extension).  
> On Linux/macOS with GCC, replace every occurrence of:
> ```cpp
> localtime_s(&tm_struct, &timestamp);
> ```
> with:
> ```cpp
> localtime_r(&timestamp, &tm_struct);
> ```
> Note the reversed argument order. Then compile:
> ```bash
> g++ -std=c++11 -o garage "SP Source Code.cpp"
> ./garage
> ```

##  Security Notes

- Admin credentials are hardcoded in `initAdmins()` — **replace them before any real use or deployment**
- Passwords are compared in plain text — no hashing is applied
- This project was built for academic purposes and is not intended for production use

## Project Structure

```
Garage-Management-System/
├── SP Source Code.cpp    # Full source (single-file implementation)
├── .gitignore
└── README.md
```

> Runtime data files (`vehicles.txt`, `slots.txt`, `tickets.txt`, `pricing.txt`, `profits.txt`) are generated on first save and are excluded via `.gitignore`.

## Course

Structured Programming — Faculty of Computer and Information Sciences, Ain Shams University, 2025–2026

## Team

Built by a 7-students team from Ain Shams University.  
**1. Waleed Essam** — Team Leader

**2. Hamza Khaled**

**3. Mostafa Ahmed**

**4. Mohamed Assad**

**5. Omar Kamal**

**6. Ahmed Elsayed**

**7. Salama Salama**
