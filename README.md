# minitalk 📡

A client-server communication program using UNIX signals to transmit strings bit by bit, developed as part of the 42 School curriculum to explore inter-process communication and bit manipulation.

## Description 📜

The minitalk project implements a communication system between two programs: a **server** and a **client**. The server displays its PID and waits to receive strings from clients. The client takes the server's PID and a string as parameters, then sends the string to the server character by character using only UNIX signals (`SIGUSR1` and `SIGUSR2`). The server reconstructs and displays the received message.

**Key Challenge:** Transmit data using only two signals, requiring bit-by-bit communication and careful timing to avoid signal loss.

## Technologies & Concepts 🛠️

- **UNIX Signals**: Inter-process communication using `SIGUSR1` and `SIGUSR2`
- **Signal Handling**: Using `sigaction()` for robust signal management
- **Bit Manipulation**: Encoding/decoding characters using bitwise operations
- **Process Communication**: Client-server architecture using PIDs
- **Async-Signal-Safe Functions**: Understanding signal handler constraints
- **Timing & Synchronization**: Managing signal transmission delays to prevent loss
- **Memory Management**: Static variables and proper resource handling

## How It Works 🔄

### Communication Flow:
```
┌─────────┐                           ┌─────────┐
│ CLIENT  │                           │ SERVER  │
└────┬────┘                           └────┬────┘
     │                                     │
     │  1. Server starts & prints PID      │
     │  ◄────────────────────────────────  │
     │                                     │
     │  2. Client sends string as bits     │
     │  ──────────────────────────────►    │
     │     (SIGUSR1 = 0, SIGUSR2 = 1)      │
     │                                     │
     │  3. Server reconstructs chars       │
     │                  ◄───────────────   │
     │                                     │
     │  4. Server displays message         │
     │                  ◄───────────────   │
     └                                     ┘
```

### Bit-by-Bit Transmission Example:

Sending character `'A'` (ASCII 65 = `01000001`):
```
Bit 7 (0) → SIGUSR1 → Server builds: 00000000
Bit 6 (1) → SIGUSR2 → Server builds: 00000001
Bit 5 (0) → SIGUSR1 → Server builds: 00000010
Bit 4 (0) → SIGUSR1 → Server builds: 00000100
Bit 3 (0) → SIGUSR1 → Server builds: 00001000
Bit 2 (0) → SIGUSR1 → Server builds: 00010000
Bit 1 (0) → SIGUSR1 → Server builds: 00100000
Bit 0 (1) → SIGUSR2 → Server builds: 01000001 = 'A' ✓
```

## Architecture 🏗️

### Server Architecture:
```
┌─────────────────────────────────────┐
│           SERVER PROGRAM            │
├─────────────────────────────────────┤
│                                     │
│  1. main()                          │
│     ├─ Print PID                    │
│     ├─ Setup signal handlers        │
│     └─ Loop forever (pause)         │
│                                     │
│  2. setup_signals()                 │
│     ├─ Configure sigaction          │
│     ├─ Register SIGUSR1 handler     │
│     └─ Register SIGUSR2 handler     │
│                                     │
│  3. signal_handler()                │
│     ├─ Receive signal (SIGUSR1/2)   │
│     ├─ Build character bit-by-bit   │
│     └─ Print when 8 bits complete   │
│                                     │
└─────────────────────────────────────┘
```

### Client Architecture:
```
┌─────────────────────────────────────┐
│           CLIENT PROGRAM            │
├─────────────────────────────────────┤
│                                     │
│  1. main()                          │
│     ├─ Validate arguments           │
│     ├─ Parse server PID             │
│     └─ Send string to server        │
│                                     │
│  2. send_string()                   │
│     └─ For each character:          │
│         └─ send_char()              │
│                                     │
│  3. send_char()                     │
│     └─ For each bit (MSB to LSB):   │
│         ├─ Extract bit value        │
│         ├─ Send SIGUSR1 (0) or      │
│         │  SIGUSR2 (1)              │
│         └─ Small delay (usleep)     │
│                                     │
└─────────────────────────────────────┘
```

## Source Code Structure 📂

### Server Implementation
- `server.c` - Server program with signal handling and character reconstruction
  - `main()` - Entry point, prints PID and enters waiting loop
  - `setup_signals()` - Configures `sigaction` structures for both signals
  - `signal_handler()` - Receives signals and builds characters bit by bit

### Client Implementation  
- `client.c` - Client program for sending messages
  - `main()` - Entry point, validates input and initiates transmission
  - `send_string()` - Iterates through string and sends each character
  - `send_char()` - Extracts and transmits individual bits as signals

### Headers & Libraries
- `minitalk.h` - Main header with function prototypes and includes
- `libft/` - Custom library with utility functions (`ft_atoi`, `ft_printf`, etc.)

### Additional Files
- `Makefile` - Build system for compiling both executables
- `guides/signal_and_bit_guide.md` - Comprehensive guide on signals and bit manipulation

## Instructions ⚙️

### Building the Project
Run `make` to compile both `client` and `server` executables:
```bash
make
```

### Running the Server
Start the server first (it will display its PID):
```bash
./server
```
Output example:
```
Server PID: 12345
```

### Running the Client
Send a message to the server using its PID:
```bash
./client <server_pid> "Your message here"
```
Example:
```bash
./client 12345 "Hello, World!"
```

The server will display:
```
Hello, World!
```

### Development Commands
- Clean object files: `make clean`
- Remove all generated files: `make fclean`
- Rebuild from scratch: `make re`
- Check norminette: `make normi`

## Technical Challenges 🧩

### Signal Loss Prevention
Linux doesn't queue multiple signals of the same type. Solution: Add small delays (`usleep(100-500)`) between signal transmissions.

### Bit Manipulation Operations
```c
// Extract bit at position i (MSB first)
bit = (character >> (7 - i)) & 1;

// Reconstruct character (shift left + OR)
character = (character << 1) | bit;
```

### Static Variables in Signal Handlers
Use static variables to maintain state between signal handler calls:
```c
static char current_char = 0;  // Builds character
static int bit_count = 0;       // Tracks bits received
```

## Bonus Features ⭐

- **Server acknowledgment**: Server sends signal back to client after receiving each message
- **Unicode support**: Extend to handle multi-byte characters (UTF-8)


## Useful Links 🔗

### Signal Handling:

- [Signals in C – GeeksforGeeks](https://www.geeksforgeeks.org/signals-c-language/)

### Bit Manipulation:
- [Bitwise Operators in C – Programiz](https://www.programiz.com/c-programming/bitwise-operators)
- [Bit Manipulation – GeeksforGeeks](https://www.geeksforgeeks.org/bits-manipulation-important-tactics/)
- [Interactive Bitwise Visualizer](https://bitwisecmd.com/)

### Video Tutorials:
- [Unix Signals (CodeVault)](https://www.youtube.com/watch?v=5We_HtLlAbs)
- [Bitwise Operators in C (Neso Academy)](https://www.youtube.com/watch?v=jlQmeyce65Q)

## License 📜

This project was developed for academic purposes at 42 School, but feel free to use it as reference or inspiration for your own learning journey.