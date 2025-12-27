# OTP Extraction - C++ Implementation

A robust C++ utility for extracting One-Time Passwords (OTPs) from unstructured SMS and email messages. This implementation uses intelligent pattern matching and contextual analysis to accurately identify OTPs from noisy, real-world text data.

## Overview

The project implements a multi-language OTP extraction system (multilang) with C++ as one of the implementations. It analyzes text messages to intelligently extract 4-8 digit OTP codes while filtering out false positives like amounts, account numbers, and card references.

## Architecture

### Core Classes

#### `Utility` (Abstract Base Class)
- **Purpose**: Defines the interface for OTP extraction operations
- **Key Methods**:
  - `getMasterDataFilePath()`: Returns the path to the input CSV file
  - `extractOtp(const string& message)`: Extracts OTP from a message
  - `readFileData()`: Reads CSV data with proper quote and escape handling
  - `writeResults(const vector<Result>&)`: Outputs results and statistics

#### `Child` (Implementation)
- **Extends**: `Utility`
- **Data Source**: `./reduced-master.csv`
- **OTP Extraction Logic**:
  1. **Normalization**: Converts to lowercase, removes special markers (`<#>`), handles spacing
  2. **Tokenization**: Splits text into alphanumeric tokens
  3. **Keyword Detection**: Identifies OTP-related keywords (otp, code, verification, etc.)
  4. **Candidate Filtering**: Ensures candidates are 4-8 digit numbers
  5. **False Positive Elimination**: Filters amounts (Rs, INR), card/account numbers
  6. **Scoring**: Ranks candidates by proximity to OTP keywords and position
  7. **Selection**: Returns the highest-scoring candidate

#### `Runner` (Main Entry Point)
- Orchestrates the entire extraction workflow
- Processes each row from the CSV file
- Generates accuracy metrics and output files

## Input/Output

### Input Format
CSV file with columns:
- **Column 1**: Raw message text (SMS/email content)
- **Column 2**: Expected OTP (ground truth)

Example:
```
Your verification code is 1234
Hello, your otp: 5678
```

### Output Files

1. **output.csv**: All results with extracted vs. expected OTPs
2. **failedCases.csv**: Only mismatched extractions
3. **Console Output**: Accuracy metrics
   - Total Cases
   - Failed Cases
   - Accuracy Percentage

## Compilation & Execution

### Prerequisites
- C++11 or later compiler (g++, clang, MSVC)

### Build
```bash
g++ -std=c++11 -o otp_extractor Runner.cpp
```

### Run
```bash
./otp_extractor
```

The program will:
1. Read `./reduced-master.csv`
2. Extract OTPs from each message
3. Write results to `output.csv` and `failedCases.csv`
4. Print accuracy statistics

## Key Features

✓ **CSV Parsing**: Handles quoted fields, escaped quotes, and multiline content  
✓ **Intelligent Filtering**: Distinguishes OTPs from amounts, card numbers, and other numeric patterns  
✓ **Context-Aware**: Uses keyword proximity and token positioning for accurate identification  
✓ **Performance Metrics**: Automatically calculates accuracy and failure analysis  
✓ **Error Handling**: Validates file paths and handles I/O errors gracefully  

## Algorithm Details

### Scoring System
- Base score: Distance from OTP keyword (closer = higher)
- Bonus: +1 if OTP appears after keyword
- Filters applied:
  - Must be 4–8 consecutive digits
  - Not preceded by amount keywords (rs, inr, rupees)
  - Not related to card/account identifiers

### Keyword Sets
- **OTP Keywords**: otp, code, verification, verify, authentication, authorization, password, login, passcode, secure
- **Amount Keywords**: rs, inr, rupees
- **Card Keywords**: card, account, ac, a, ending, xxxx, id

## Data Files

| File | Purpose |
|------|---------|
| `reduced-master.csv` | Input dataset with messages and expected OTPs |
| `output.csv` | Complete results with all extractions |
| `failedCases.csv` | Only extraction failures for review |
| `master_data.csv` | Full original dataset (reference) |

## Testing

Run the program with `reduced-master.csv` to validate accuracy:
```bash
./otp_extractor
```

Check the generated files:
- Verify extraction correctness in `output.csv`
- Review missed cases in `failedCases.csv`

## Performance Characteristics

- **Time Complexity**: O(n × m) where n = messages, m = average message length
- **Space Complexity**: O(n × k) where k = average tokens per message
- **CSV Parsing**: Single-pass with proper quote/escape handling

## Future Enhancements

- Multi-character OTP support (e.g., alphanumeric codes)
- Machine learning-based pattern recognition
- Support for additional languages
- Parallel processing for large datasets

## Project Structure

```
.
├── Child.cpp           # OTP extraction implementation
├── Utility.cpp         # Abstract interface & CSV utilities
├── Runner.cpp          # Main execution program
├── reduced-master.csv  # Test dataset
├── output.csv          # Generated results
├── failedCases.csv     # Generated failure log
└── README.md           # This file
```

---

**Part of**: OTP Extraction - MultiLanguage Project  
**Language**: C++11+  
**Last Updated**: December 2025
