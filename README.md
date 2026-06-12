# Tubes PMP

Project bahasa C untuk tugas besar PMP.

## Compile
# Mode Interaktif
Untuk menjalankan aplikasi sistem inventaris dengan menu CLI interaktif:
```bash
make run
```
# Mode Testbench
Untuk menjalankan pengujian komprehensif dari file testcase.txt dan menghasilkan laporan di test/output/result.txt:
```bash
make run_test
```
# Clean Build
Untuk membersihkan file eksekusi (compiled files) dan riwayat output test:
```bash
make clean
```

# How to Commit

```bash
git add .
git commit -m "Your commit message here"
git push origin main
```

# Commit Message Guidelines
The commit type can be

1. feat: Commits, which adds a new feature
2. fix: Commits, that fixes a bug
3. refactor: refactored code that neither fixes a bug nor adds a feature but rewrites/restructures your code.
4. chore : Changes that do not relate to a fix or feature and don’t modify src or test files basically miscellaneous commits (for example, updating dependencies or modifying .gitignore file)
5. perf : Commits are special refactor commits, geared towards improving performance.
6. ci : Continuous integration related.
7. ops : Commits, that affect operational components like infrastructure, deployment, backup , recovery …
8. build : Changes that affect the build system build tool, ci pipeline, dependencies, project version, …
9. docs : Commits, that affect documentation, such as the README.
10. style : changes that do not affect the meaning of the code, likely related to code formatting such as white-space, missing semi-colons, etc.
11. revert: reverts a previous commit.
12. test:commits that add missing tests or correct existing tests

# Inventory Management System - Project Management Documentation

**Project:** Arduino Inventory Management System  
**Date Created:** May 26, 2026  
**Team Members:** Kyrie, Azka, Kuri, Kenneth

---

## Table of Contents
1. [Task 1: Memory Management](#task-1-memory-management)
2. [Task 2: Inventory Functions](#task-2-inventory-functions)
3. [Task 3: Display Functions](#task-3-display-functions)
4. [Task 4: Memory Capacity Checks](#task-4-memory-capacity-checks)

---

# Task 1: Memory Management

**Assigned to:** Kyrie  
**Project:** Inventory Management System  
**Date Created:** May 26, 2026

## Objective
Implement memory management functions for the inventory system using a singly linked list data structure. Ensure safe allocation, deallocation, and node retrieval operations.

## Functions to Implement

### 1. `add_item()`
```c
void add_item(Node** head, const char* id, const char* item, 
    const char* category, int amount_of_stocks, const char* location, 
    const char* status, const char* owner, const char* pic);
```
- **Purpose**: Allocate memory and add a new node to the linked list
- **Responsibilities**:
  - Validate input parameters
  - Allocate memory for new Node
  - Copy data into node fields
  - Insert at appropriate position (head or end)
  - Handle memory allocation failures

### 2. `delete_item()`
```c
void delete_item(Node** head, const char* id);
```
- **Purpose**: Find and remove a node from the linked list
- **Responsibilities**:
  - Search for node by ID
  - Unlink node from list
  - Free allocated memory
  - Handle edge cases (empty list, single item)

### 3. `find_item()`
```c
void find_item(Node* head, const char* id, Node** found_item);
```
- **Purpose**: Search the linked list for a specific item by ID
- **Responsibilities**:
  - Traverse linked list
  - Compare IDs
  - Return pointer to found node
  - Handle not found cases

## Key Technical Considerations
- **Memory Safety**: Check all malloc returns; prevent memory leaks
- **Pointer Handling**: Manage double pointers for head manipulation
- **Edge Cases**: Empty list, single element, duplicate IDs
- **String Operations**: Safe string copying (strncpy or similar)
- **Arduino Constraints**: Limited RAM - optimize memory usage

## Dependencies
- Node structure defined in inventory.h
- Standard C library (stdlib.h, string.h)

## Deliverables
- [ ] Implemented functions in inventory.c
- [ ] Memory leak testing completed
- [ ] Unit tests passed
- [ ] Code documentation with comments

## Timeline
| Phase | Timeline |
|-------|----------|
| Design & Planning | - |
| Implementation | - |
| Testing | - |
| Code Review | - |

## Testing Checklist
- [ ] Add single item
- [ ] Add multiple items
- [ ] Delete existing item
- [ ] Delete non-existent item
- [ ] Find existing item
- [ ] Find non-existent item
- [ ] Verify memory not leaked
- [ ] Test with empty list

## Notes
---

# Task 2: Inventory Functions

**Assigned to:** Azka  
**Project:** Inventory Management System  
**Date Created:** May 26, 2026

## Objective
Implement core inventory operation functions that modify item quantities and statuses. These functions depend on Task 1's memory management layer.

## Functions to Implement

### 1. `update_stocks()`
```c
void update_stocks(Node* head, const char* id, int new_amount);
```
- **Purpose**: Update the stock quantity for a specific item
- **Responsibilities**:
  - Find item by ID (use find_item from Task 1)
  - Validate new_amount (non-negative)
  - Update amount_of_stocks field
  - Log changes for audit trail
  - Handle not found cases

### 2. `update_status()`
```c
void update_status(Node* head, const char* id, const char* new_status);
```
- **Purpose**: Change the status of an item
- **Responsibilities**:
  - Find item by ID
  - Validate status value (tersedia/dipinjam/rusak/habis)
  - Update status field
  - Prevent invalid state transitions
  - Handle invalid status values

## Valid Status Values
- `tersedia` - Available
- `dipinjam` - Borrowed
- `rusak` - Damaged
- `habis` - Out of stock

## Key Technical Considerations
- **Data Validation**: Ensure inputs are valid before updating
- **State Management**: Prevent conflicting status/stock combinations
- **Atomicity**: Updates should be atomic operations
- **Error Handling**: Return status codes or use output parameters
- **Dependency**: Must work with Task 1 functions

## Dependencies
- Task 1: Memory Management (add_item, delete_item, find_item)
- Node structure and linked list implementation

## Deliverables
- [ ] Implemented functions in inventory.c
- [ ] Input validation complete
- [ ] Integration with Task 1 verified
- [ ] Unit tests passed
- [ ] Function behavior documented

## Timeline
| Phase | Timeline |
|-------|----------|
| Review Task 1 Output | - |
| Implementation | - |
| Integration Testing | - |
| Validation | - |

## Testing Checklist
- [ ] Update stock for existing item
- [ ] Update stock to zero
- [ ] Update stock with negative value (should fail)
- [ ] Update status to valid value
- [ ] Update status to invalid value (should fail)
- [ ] Update non-existent item (should handle gracefully)
- [ ] Verify consistency with Task 1

## Notes
---

# Task 3: Display Functions

**Assigned to:** Kuri  
**Project:** Inventory Management System  
**Date Created:** May 26, 2026

## Objective
Implement display and reporting functions to present inventory data to users. Provide both detailed listing and summary statistics views.

## Functions to Implement

### 1. `display_all_items()`
```c
void display_all_items(Node* head);
```
- **Purpose**: Display complete details of all items in inventory
- **Responsibilities**:
  - Traverse entire linked list
  - Format and print each item's information
  - Display headers and borders for clarity
  - Handle empty inventory case
  - Display all 8 fields (ID, item, category, stock, location, status, owner, PIC)

### 2. `display_summaries()`
```c
void display_summaries(Node* head);
```
- **Purpose**: Display summary statistics and aggregate information
- **Responsibilities**:
  - Count total items in inventory
  - Display items by status (tersedia/dipinjam/rusak/habis)
  - Show total stock count
  - Calculate summary statistics
  - Display in organized format

## Display Format Suggestions

**For display_all_items():**
```
═══════════════════════════════════════════════════════════════
ID         Item              Category    Stock  Location  Status
───────────────────────────────────────────────────────────────
[data]     [data]            [data]      [data] [data]    [data]
═══════════════════════════════════════════════════════════════
```

**For display_summaries():**
```
INVENTORY SUMMARY
─────────────────
Total Items: X
Total Stock: X
Status Breakdown:
  - Tersedia: X
  - Dipinjam: X
  - Rusak: X
  - Habis: X
```

## Key Technical Considerations
- **Formatting**: Clear, aligned columns for readability
- **Empty List**: Handle display when no items exist
- **Data Presentation**: Make information easy to understand
- **Serial Output**: Optimize for Arduino serial communication
- **Performance**: Efficient single-pass traversal

## Dependencies
- Task 1: Memory Management (linked list navigation)
- Task 2: Inventory Functions (for accurate data)

## Deliverables
- [ ] display_all_items() implemented with proper formatting
- [ ] display_summaries() implemented with statistics
- [ ] Output tested on Arduino serial monitor
- [ ] Empty inventory case handled
- [ ] Code includes formatting comments

## Timeline
| Phase | Timeline |
|-------|----------|
| Design Output Format | - |
| Implementation | - |
| Testing on Hardware | - |
| Refinement | - |

## Testing Checklist
- [ ] Display empty inventory
- [ ] Display single item
- [ ] Display multiple items
- [ ] Verify all fields displayed correctly
- [ ] Check summary calculations
- [ ] Test status breakdown
- [ ] Verify column alignment
- [ ] Test serial output formatting

## Notes
---

# Task 4: Memory Capacity Checks

**Assigned to:** Kenneth  
**Project:** Inventory Management System  
**Date Created:** May 26, 2026

## Objective
Implement validation and monitoring functions to check for duplicate entries and verify system capacity constraints. Ensure data integrity and prevent memory overflow on Arduino.

## Functions to Implement

### 1. `check_duplicate()`
```c
void check_duplicate(Node* head, const char* id, int* is_duplicate);
```
- **Purpose**: Detect if an ID already exists in the inventory
- **Responsibilities**:
  - Traverse linked list searching for matching ID
  - Set is_duplicate flag (1 = duplicate found, 0 = unique)
  - Handle null pointer cases
  - Optimize search (stop on first match)
  - Return result via output parameter

### 2. `check_memory_capacity()`
```c
void check_memory_capacity(Node* head, int* is_full);
```
- **Purpose**: Monitor and verify remaining system memory capacity
- **Responsibilities**:
  - Count current nodes in list
  - Calculate estimated memory usage
  - Check against Arduino memory limits
  - Set is_full flag based on capacity threshold
  - Consider memory fragmentation
  - Provide buffer for other operations

## Arduino Memory Considerations

**Typical Arduino Capacities:**
- Uno/Nano: 2 KB SRAM
- Mega2560: 8 KB SRAM
- MKR1000: 32 KB SRAM

**Memory Allocation per Node:**
```
sizeof(Node) = 12 + 30 + 20 + 4 + 20 + 20 + 20 + 20 + 8 = 154 bytes
```

## Key Technical Considerations
- **Efficiency**: Minimal overhead for frequent checks
- **Capacity Threshold**: Set conservative limit (e.g., 80% of available)
- **Duplicate Prevention**: Ensure add_item uses this function first
- **Memory Fragmentation**: Account for linked list overhead
- **Arduino Constraints**: Work within strict memory limits
- **Performance**: Quick validation without full traversal when possible

## Dependencies
- Task 1: Memory Management (linked list traversal)
- All previous tasks should integrate this function

## Deliverables
- [ ] check_duplicate() implemented correctly
- [ ] check_memory_capacity() calculates accurately
- [ ] Capacity threshold defined and documented
- [ ] Integration with add_item() verified
- [ ] Edge cases tested

## Timeline
| Phase | Timeline |
|-------|----------|
| Analyze Arduino Constraints | - |
| Implementation | - |
| Capacity Testing | - |
| Integration | - |

## Testing Checklist
- [ ] Detect duplicate ID when exists
- [ ] Return no duplicate when ID unique
- [ ] Detect full memory when approaching limit
- [ ] Return not full when capacity available
- [ ] Test with empty list
- [ ] Test with single item
- [ ] Test with many items
- [ ] Verify memory calculation accuracy
- [ ] Test edge cases (maximum capacity)

## Memory Calculation Formula
```
Total Memory Used = (Number of Nodes × sizeof(Node)) + Overhead
Capacity Available = Total SRAM - Reserved Space
Is Full = (Total Memory Used / Capacity Available) > Threshold
```

## Notes
---

## Project Summary

### Team Roles
| Name | Task | Responsibility |
|------|------|-----------------|
| Kyrie | Task 1 | Memory Management (add, delete, find) |
| Azka | Task 2 | Inventory Functions (update stocks/status) |
| Kuri | Task 3 | Display Functions (output formatting) |
| Kenneth | Task 4 | Capacity Checks (validation & monitoring) |

### Task Dependencies
```
Task 1 (Memory Management)
    ↓
Task 2 (Inventory Functions)
    ↓
Task 3 (Display Functions)
    ↓
Task 4 (Memory Capacity Checks)
```

### Overall Timeline
| Phase | Timeline |
|-------|----------|
| Requirements Review | - |
| Task 1 Implementation | - |
| Task 2 Integration | - |
| Task 3 Implementation | - |
| Task 4 Integration | - |
| System Testing | - |
| Final Review & Deployment | - |

### Success Criteria
- [ ] All functions implemented and tested
- [ ] No memory leaks detected
- [ ] Arduino deployment successful
- [ ] All team members complete their tasks
- [ ] Code documentation complete
- [ ] Integration testing passed

---

**Last Updated:** May 26, 2026  
**Version:** 1.0
