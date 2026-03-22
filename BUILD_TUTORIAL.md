# Build Tutorial: Potty & Unpotty GBA (Windows, macOS, Linux)

This guide explains how to build `potty_unpotty.gba` on major desktop platforms.

---

## 1) Prerequisites

You need:
- `make`
- `devkitPro` with `devkitARM`
- `libgba`

Project output ROM:
- `build/potty_unpotty.gba`

---

## 2) Windows (recommended: MSYS2 + devkitPro)

### Step A: Install MSYS2
1. Download and install MSYS2 from: <https://www.msys2.org/>
2. Open **MSYS2 UCRT64** terminal.
3. Update base packages:
   ```bash
   pacman -Syu
   ```
   If prompted to close terminal, reopen and run:
   ```bash
   pacman -Su
   ```

### Step B: Install build tools
```bash
pacman -S --needed make git
```

### Step C: Install devkitPro + GBA tools
1. Install devkitPro pacman support: <https://devkitpro.org/wiki/Getting_Started>
2. In devkitPro/MSYS shell, install:
   ```bash
   sudo dkp-pacman -S gba-dev
   ```

### Step D: Build this project
From project root:
```bash
make clean
make
```

If successful, ROM is at:
- `build/potty_unpotty.gba`

---

## 3) macOS

### Step A: Install Xcode command line tools
```bash
xcode-select --install
```

### Step B: Install Homebrew (if missing)
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Step C: Install make/git
```bash
brew install make git
```

### Step D: Install devkitPro packages
Follow devkitPro official instructions for macOS and install `gba-dev`.

Typical command in devkitPro-enabled shell:
```bash
sudo dkp-pacman -S gba-dev
```

### Step E: Build
```bash
make clean
make
```

ROM output:
- `build/potty_unpotty.gba`

---

## 4) Linux (Ubuntu/Debian example)

### Step A: Install base tools
```bash
sudo apt update
sudo apt install -y build-essential make git curl
```

### Step B: Install devkitPro and GBA packages
Follow official devkitPro Linux setup instructions, then install:
```bash
sudo dkp-pacman -S gba-dev
```

### Step C: Build
```bash
make clean
make
```

ROM output:
- `build/potty_unpotty.gba`

---

## 5) Verify toolchain quickly

Run these commands in your build shell:
```bash
arm-none-eabi-gcc --version
arm-none-eabi-objcopy --version
```

If both commands print versions, your toolchain is ready.

---

## 6) Run the ROM

Use a GBA emulator such as:
- mGBA
- VBA-M

Open:
- `build/potty_unpotty.gba`

---

## 7) Common issues

### `arm-none-eabi-gcc: No such file or directory`
Your `devkitARM` toolchain is not installed or not on `PATH`.

### `cannot find -lgba`
`libgba` is missing. Install `gba-dev` via `dkp-pacman`.

### `make: *** No rule to make target`
Run `make` from the repository root where the `Makefile` exists.

---

## 8) Clean rebuild

```bash
make clean
make
```

This removes old build artifacts and compiles a fresh ROM.
