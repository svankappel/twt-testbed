# TWT Testbed Scripts

## Available Scripts

### 1. `extractresults.py`
This script processes log files and generates a JSON file containing the results of all tests present in the logs.

**Usage:**
```bash
python extractresults.py <log_file>
```

### 2. `testreport.py`
This script generates test reports from a JSON file that contains test results.

**Usage:**
```bash
python testreport.py <results_json_file>
```

### 3. `histogram.py`
This script generates a PNG file from the results of a single test.

**Usage:**
```bash
python histogram.py <test_results_file>
```