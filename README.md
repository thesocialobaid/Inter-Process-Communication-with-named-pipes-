# OS Lab 5 — Inter-Process Communication with Named Pipes (FIFOs)
**Student:** Obaid | **Roll No:** 24L-0509

---

## Overview

This lab explores **Inter-Process Communication (IPC)** in Linux using **Named Pipes (FIFOs)**. Named pipes allow two independent processes to communicate by creating a special file in the filesystem that acts as a data channel — one process writes to it, and another reads from it.

A key property demonstrated throughout this lab is **blocking behavior**: when a reader opens a FIFO before any writer exists, it blocks (freezes) at the `open()` call until a writer connects, and vice versa. This synchronization is enforced by the kernel.

---

## Lab Structure

```
os_lab_5_24l0509/
│
├── Q1_named_pipe_demo/
│   ├── reader.c
│   └── writer.c
│
├── Q2_calculator/
│   ├── server.c
│   └── client.c
│
├── Q3_student_lookup/
│   ├── server.c
│   └── client.c
│
└── README.md
```

---

## Question 1 — Blocking Behavior Demonstration

### Objective
Demonstrate the blocking nature of named pipes by running the reader before the writer and observing what happens.

### Files
| File | Role |
|------|------|
| `reader.c` | Opens the FIFO for reading, blocks until a writer connects |
| `writer.c` | Opens the FIFO for writing, sends a message, then exits |

### How to Run

```bash
# Step 1: Create the named pipe
mkfifo myfifo

# Step 2: Compile both programs
gcc -o reader reader.c
gcc -o writer writer.c

# Terminal 1 — run reader FIRST (it will freeze/hang)
./reader

# Terminal 2 — run writer (this unblocks the reader)
./writer
```

### Expected Output

**Terminal 1** (after writer runs):
```
Recieved: I want to become a Product Manager at Microsoft
```

### Blocking Behavior Explained

| Step | What Happens |
|------|-------------|
| Reader opens first | Process **blocks** at `open()` — kernel suspends it |
| Writer opens pipe | Kernel connects both ends simultaneously |
| Both unblock | Data flows, both processes continue and exit |

Running `ps` on the blocked reader shows `STAT = S` (Sleeping), which is hard evidence of the block.

---

## Question 2 — Two-Way Calculator (Client-Server)

### Objective
Build a client-server calculator using **two FIFOs** for bidirectional communication. The client sends an arithmetic expression; the server computes and returns the result.

### Files
| File | Role |
|------|------|
| `server.c` | Creates FIFOs, receives expressions, computes results, sends back |
| `client.c` | Takes user input, sends to server, displays the result |

### FIFO Architecture

```
CLIENT ──── fifo_request ────▶ SERVER
                                  │  (computes result)
CLIENT ◀─── fifo_response ──── SERVER
```

| FIFO | Direction | Purpose |
|------|-----------|---------|
| `fifo_request` | Client → Server | Sends operator + operands |
| `fifo_response` | Server → Client | Sends back the result string |

### Input Format

```
<operator> <operand1> <operand2>
```

Supported operators: `+`  `-`  `*`  `/`

### How to Run

```bash
gcc -o server server.c
gcc -o client client.c
rm -f fifo_request fifo_response

# Terminal 1
./server

# Terminal 2 (run once per calculation)
./client
```

### Sample Runs

| Input | Output |
|-------|--------|
| `+ 4 10` | `Result: 4.00 + 10.00 = 14.00` |
| `* 6 7` | `Result: 6.00 * 7.00 = 42.00` |
| `/ 20 4` | `Result: 20.00 / 4.00 = 5.00` |
| `- 15 6` | `Result: 15.00 - 6.00 = 9.00` |
| `/ 9 0` | `Error: Division by zero is not allowed!` |

### Features
- Server runs in an infinite loop to handle multiple clients
- Division by zero is caught and an error message is returned
- Data is transmitted as a string to avoid binary alignment issues

---

## Question 3 — Student Record Lookup System

### Objective
Build a client-server lookup system where the client sends a roll number and the server searches a predefined in-memory database and returns the student's details.

### Files
| File | Role |
|------|------|
| `server.c` | Maintains student database, searches by roll number, returns result |
| `client.c` | Sends roll number, displays name and marks returned by server |

### FIFO Architecture

```
CLIENT ──── fifo_req ────▶ SERVER
                              │  (searches database)
CLIENT ◀─── fifo_res ──── SERVER
```

### Student Database (hardcoded in server)

| Roll No | Name  | Marks |
|---------|-------|-------|
| 101     | Ali   | 85    |
| 102     | Sara  | 90    |
| 103     | Ahmed | 78    |

### How to Run

```bash
gcc -o server server.c
gcc -o client client.c
rm -f fifo_req fifo_res

# Terminal 1
./server

# Terminal 2
./client      # enter: 101
./client      # enter: 102
./client      # enter: 999  (not found case)
```

### Sample Runs

| Input (Roll No) | Client Output |
|-----------------|---------------|
| `101` | `Found! Roll No: 101 \| Name: Ali \| Marks: 85` |
| `102` | `Found! Roll No: 102 \| Name: Sara \| Marks: 90` |
| `103` | `Found! Roll No: 103 \| Name: Ahmed \| Marks: 78` |
| `999` | `Error: No student found with Roll No 999` |

### Features
- Server runs in a loop to handle multiple lookup requests
- Invalid roll numbers return a clear error message
- Server prints each received roll number before responding

---

## Key Concepts Covered

| Concept | Description |
|---------|-------------|
| **Named Pipes (FIFOs)** | Created with `mkfifo()`, persist on filesystem unlike anonymous pipes |
| **Blocking I/O** | `open()` on a FIFO blocks until both ends are connected |
| **Two-way IPC** | Using two FIFOs to achieve full duplex communication |
| **Client-Server Model** | Server loops indefinitely; client connects, transacts, and exits |
| **String-based protocol** | Data sent as formatted strings and parsed with `sscanf` to avoid alignment issues |

---

## Compilation Reference

```bash
gcc -o <output> <source.c>
```

Always **delete old FIFOs** before re-running to avoid stale pipe errors:
```bash
rm -f myfifo fifo_request fifo_response fifo_req fifo_res
```

---

*Lab completed as part of Operating Systems course — 2024*
