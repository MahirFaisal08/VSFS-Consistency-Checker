# VSFS Consistency Checker  
![Language](https://img.shields.io/badge/language-C-blue)  
![License](https://img.shields.io/badge/license-MIT-green)  
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

```bash
# Clone the repository
git clone https://github.com/MahirFaisal08/VSFS-Consistency-Checker.git
cd VSFS-Consistency-Checker

# Compile the checker
gcc vsfsck.c -o vsfsck

# Run the checker
./vsfsck vsfs.img
```

## Example Usage
$ ./vsfsck vsfs.img
Checking superblock...
Checking inode bitmap...
Checking data bitmap...
File system is consistent.

## Project Specs
- Superblock validation
- Inode and data bitmap verification
- Detection of duplicate and invalid blocks
- Low-level file system analysis in C

## Future Improvements
- Automatic repair mode for detected inconsistencies
- Detailed error reporting and logging
- Support for larger and dynamic file system images

## Author  
- Mahir Faisal  
- [GitHub Profile](https://github.com/MahirFaisal08?tab=repositories)

## License

This project is licensed under the **MIT License** – you are free to use, modify, and distribute this software with proper credit.  
For more details, see the [LICENSE](LICENSE) file in this repository or visit the official [MIT License](https://opensource.org/licenses/MIT).
