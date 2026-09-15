# LPM (Lucy Package Manager)

LPM is a high-performance, secure package manager designed exclusively for the Lucy programming language ecosystem. Engineered in C++ for Linux systems, it provides a proprietary obfuscated packaging format (`.plm`) and recursive dependency resolution.

## Architecture & Features

*   **Secure Packaging (.plm):** Compresses and obfuscates project directories using a proprietary XOR-based cipher, preventing standard archiving tools from extracting the source code.
*   **Recursive Dependency Tree:** Automatically detects, resolves, and installs nested dependencies located within the `/lib` directory structure.
*   **Sleek CLI Interface:** Features a custom C++ UI engine with minimal loading indicators, colored terminal output, and tabular data representation.
*   **Global Standard Library Management:** Handles installation, uninstallation, and metadata 

## Installation

The pre-compiled `lpm` binary is included in this repository. You do not need to compile it yourself. Just run the `install.sh` script:

```bash
chmod +x install.sh
./install.sh
```

## Command Reference

| Command | Description |
| :--- | :--- |
| `lpm init` | Scaffolds a new Lucy package environment (`my-package/`). |
| `lpm package` | Compresses and encrypts the current project into a `.plm` archive. |
| `lpm install` | Installs the current local project globally to the Lucy stdlib. |
| `lpm install --lpm <file>` | Decrypts, extracts, and recursively installs a `.plm`/`.lpm` archive. |
| `lpm uninstall <pkg>` | Completely removes a specified package and its metadata. |
| `lpm list` | Displays a formatted table of all globally installed packages. |
| `lpm info <pkg>` | Outputs detailed metadata and Lucy version requirements for a package. |

## Manifest Structure (`package.toml`)

LPM relies on TOML for package configuration. A standard initialization yields:

```toml
[package]
name = "my_package"
version = "1.0.0"
version-lucy = ">=1.0"

[dependencies]
# "some_lib" = ">=1.2.0"
```
** اگه dependencies ای نداری برای پروژت کلا خالی بزارش نه کامنت **
```toml
[package]
name = "my_package"
version = "1.0.0"
version-lucy = ">=1.0"

[dependencies]

```
## Contact & Support

Developed by **Dr.F0G3J3**  
Telegram: [@n0broot](https://t.me/n0broot)
