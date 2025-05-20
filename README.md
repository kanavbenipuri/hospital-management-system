# Hospital Management System

A comprehensive C++ application for managing hospital patient records with efficient data structures and algorithms.

## Features

- **Patient Management**
  - Add new patients with validation
  - Update patient information
  - Delete patient records
  - Search patients by ID, name, or date range

- **Department Organization**
  - View patients by department
  - Track department-wise statistics
  - Monitor patient distribution

- **Room Management**
  - Track room occupancy
  - Monitor room availability
  - Prevent room overbooking

- **Data Persistence**
  - CSV file-based storage
  - Automatic data saving
  - Data validation on load/save

- **Search & Analytics**
  - Multiple search criteria
  - Statistical reporting
  - Date range filtering

## Data Structures Used

- **Vector**: For primary patient storage
- **Unordered Maps**: For efficient indexing and lookups
  - ID to Index mapping
  - Name to Indices mapping (case-insensitive)
  - Department to Indices mapping
  - Condition to Indices mapping
  - Room to Indices mapping

## Getting Started

### Prerequisites

- C++ compiler with C++11 support
- Basic command line interface knowledge

### Compilation

```bash
g++ -std=c++11 hospital_system2.cpp -o hospital_system
```

### Running the Program

```bash
.\hospital_system
```

When prompted, enter the CSV file name (e.g., `patients.csv`).

## Data Format

The system uses a CSV file with the following columns:

```
ID,Name,MedicalHistory,Department,Condition,AdmissionDate,DischargeDate,RoomNumber
```

Example:
```
1,John Doe,Diabetes Type 2,Endocrinology,Stable,15-01-2025,22-01-2025,101
```

## Features in Detail

### Patient Records
- Unique patient ID
- Patient name (2-50 characters)
- Medical history (up to 200 characters)
- Department assignment
- Current condition
- Admission and discharge dates
- Room number assignment

### Search Capabilities
- Search by patient ID (O(1) lookup)
- Search by patient name (case-insensitive)
- Search by date range
- Filter by department
- Filter by condition
- Filter by room number

### Statistics and Reporting
- Total patient count
- Department-wise distribution
- Condition-wise distribution
- Room occupancy status
- Currently admitted vs discharged patients

## Implementation Details

- Case-insensitive string comparisons for better search results
- Date validation and comparison functionality
- Efficient indexing for O(1) lookups
- Input validation for data integrity
- Error handling for file operations

## Best Practices

- Regular data backups
- Validate all input data
- Keep patient records up to date
- Monitor room availability
- Review department distributions