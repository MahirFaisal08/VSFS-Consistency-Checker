# VSFS Consistency Checker
![Language](https://img.shields.io/badge/language-C-blue)
![Status](https://img.shields.io/badge/status-completed-success)

---

## Overview

VSFS Consistency Checker is a tool written in C that verifies and repairs inconsistencies in the **Very Simple File System (VSFS)**.  
It analyzes a file system image and ensures that core metadata structures remain consistent and valid.

---

## Features

- Superblock validation
- Inode bitmap consistency checking
- Data bitmap verification
- Detection of invalid and duplicate data blocks
- Ensures file system structural integrity

---

## File System Layout

- **Block Size:** 4096 bytes  
- **Total Blocks:** 64  
- **Inode Table:** Blocks 3–7  
- **Data Blocks:** Blocks 8–63  

---

## Compilation & Run

### Compile
# Clone the repository
git clone https://github.com/MahirFaisal08/VSFS-Consistency-Checker.git
cd VSFS-Consistency-Checker

# Compile the checker
gcc vsfsck.c -o vsfsck

# Run the checker
./vsfsck vsfs.img

