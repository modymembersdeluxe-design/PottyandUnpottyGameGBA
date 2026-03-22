# Windows 8.1 Tutorial: Build a Release `.gba` ROM

This tutorial shows how to build a **release-ready** `potty_unpotty.gba` on **Windows 8.1**.

> Note: Windows 8.1 is legacy/unsupported by many modern tools. Use this guide as best-effort setup.

---

## 1) Install required software

## A) MSYS2 (preferred shell/tools)
1. Download MSYS2 installer from <https://www.msys2.org/>.
2. Install to default path (for example `C:\msys64`).
3. Launch **MSYS2 UCRT64** terminal.

## B) Update MSYS2 packages
In MSYS2 terminal:
```bash
pacman -Syu
```
If prompted to restart shell, close/reopen MSYS2 and run:
```bash
pacman -Su
```

## C) Install basic tools
```bash
pacman -S --needed make git unzip
```

## D) Install devkitPro and GBA toolchain
Follow official devkitPro setup for Windows and install GBA packages.
Typical command in devkitPro-enabled shell:
```bash
sudo dkp-pacman -S gba-dev
```

---

## 2) Get project source

Clone your project:
```bash
git clone <your-repo-url> PottyandUnpottyGameGBA
cd PottyandUnpottyGameGBA
```

Or copy project files into this folder and open terminal there.

---

## 3) Verify toolchain before building

Run:
```bash
arm-none-eabi-gcc --version
arm-none-eabi-objcopy --version
```

If either command fails, toolchain is not correctly installed on `PATH`.

---

## 4) Build release ROM

From project root:
```bash
make clean
make
```

Expected artifact:
- `build/potty_unpotty.gba`

---

## 5) Confirm ROM output

Check that file exists:
```bash
ls -lh build/potty_unpotty.gba
```

Optional checksum for release tracking:
```bash
sha256sum build/potty_unpotty.gba
```

Record checksum in release notes so users can verify downloads.

---

## 6) Test the release build

Open the ROM in emulator (mGBA recommended):
- `build/potty_unpotty.gba`

Quick release smoke test:
1. Start game from title.
2. Enter each mini-game once (Dash/Memory/Rhythm).
3. Verify pause works with START.
4. Verify returning to results/title works after progress.

---

## 7) Package a release zip

Create distributable package:
```bash
mkdir -p dist
cp build/potty_unpotty.gba dist/
cp README.md dist/
cp GAMEPLAY_TUTORIAL.md dist/
cp BUILD_TUTORIAL.md dist/
```

Zip it (PowerShell alternative shown below):
```powershell
Compress-Archive -Path dist\* -DestinationPath potty_unpotty_gba_windows81_release.zip -Force
```

---

## 8) Windows 8.1-specific troubleshooting

### `arm-none-eabi-gcc: command not found`
- Ensure you are in **devkitPro/MSYS2 shell**, not plain `cmd.exe`.
- Re-run `dkp-pacman -S gba-dev`.

### TLS/download issues on old Windows 8.1
- Install all latest Windows 8.1 updates.
- Try downloading installers in a modern browser.
- If package mirrors fail, retry later or use another network.

### Emulator performance issues
- Disable background apps.
- Disable fast-forward.
- Use native resolution and default video backend.

---

## 9) Release checklist

Before publishing a `.gba` release:
- [ ] `make clean && make` succeeds
- [ ] `build/potty_unpotty.gba` exists
- [ ] SHA-256 checksum recorded
- [ ] ROM launches and controls work
- [ ] ZIP package includes ROM + docs
