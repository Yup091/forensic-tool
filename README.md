# Forensic Data Tool

A forensic data recovery and secure erasure tool designed for SystemRescue USB deployment.

## Project Structure

```
forensic-tool/
├── src/              # Source files
├── include/          # Header files
├── tests/            # Test files
├── scripts/          # Utility scripts
├── reports/          # Generated audit reports
├── logs/             # Operational logs
├── recovery/         # Recovered files storage
├── build/            # Build artifacts (generated)
├── Makefile          # Build configuration
└── README.md         # This file
```

## Phase 1: Milestone 1

- [x] Create repository
- [x] Create C++ project structure
- [x] Makefile setup
- [x] main.cpp with basic output
- [x] Compile successfully

## Building

```bash
make       # Build the project
make run   # Build and run
make clean # Clean build artifacts
```

## Features (Planned)

1. **Device Identification** - Detect and list available storage devices
2. **File Recovery** - PhotoRec integration for carving deleted files
3. **Secure Erasure** - Multiple erasure methods (shred, nwipe, etc.)
4. **Verification** - Post-operation verification of erasure
5. **Audit Logging** - Case-based audit trail with timestamps
6. **Report Generation** - Text and HTML report generation
7. **Report History** - View and search previous operation reports
8. **Terminal UI** - Interactive menu-driven interface

## Deployment

Integration with SystemRescue bootable USB for forensic operations in isolated environments.
