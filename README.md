# minitalk 📡 🗡️

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

### Bit-by-Bit Transmission Example (LSB-First):

Sending character `'A'` (ASCII 65 = `01000001`):
```
Bit 0 (1) → SIGUSR2 → Server builds: 00000001 ← Set bit 0
Bit 1 (0) → SIGUSR1 → Server builds: 00000001 ← Keep bit 1 as 0
Bit 2 (0) → SIGUSR1 → Server builds: 00000001 ← Keep bit 2 as 0
Bit 3 (0) → SIGUSR1 → Server builds: 00000001 ← Keep bit 3 as 0
Bit 4 (0) → SIGUSR1 → Server builds: 00000001 ← Keep bit 4 as 0
Bit 5 (0) → SIGUSR1 → Server builds: 00000001 ← Keep bit 5 as 0
Bit 6 (1) → SIGUSR2 → Server builds: 01000001 ← Set bit 6
Bit 7 (0) → SIGUSR1 → Server builds: 01000001 = 'A' ✓
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
│     ├─ Setup signal handlers        │
│     └─ Loop through message:        │
│         ├─ For each character:      │
│         │   └─ send_char()          │
│         └─ send_char('\0')          │
│                                     │
│  2. send_char()                     │
│     └─ For each bit (LSB to MSB):   │
│         ├─ Extract bit value        │
│         ├─ Reset ACK flag           │
│         ├─ Send SIGUSR1 (0) or      │
│         │  SIGUSR2 (1)              │
│         └─ Wait for ACK (bonus)     │
│                                     │
│  3. setup_signal_handlers()         │
│     ├─ Register SIGUSR2 (ACK)       │
│     └─ Register SIGUSR1 (DONE)      │
│                                     │
└─────────────────────────────────────┘
```

### Bonus Architecture:
```
┌──────────────────────────────────────┐
│        ACKNOWLEDGMENT SYSTEM         │
├──────────────────────────────────────┤
│                                      │
│  CLIENT              SERVER          │
│  ───────────────────────────         │
│  Send bit ──────►  Receive bit       │
│                   Store in position  │
│  Wait for ACK     Send SIGUSR2 ◄──── │
│  Receive ACK ◄────                   │
│  Continue...                         │
│                                      │
│  (After '\0')     Send SIGUSR1 ◄──── │
│  "Message done!"                     │
│  Exit ◄───────                       │
│                                      │
└──────────────────────────────────────┘
```

## Source Code Structure 📂

```
minitalk/
├── server.c              # Server: signal_handler(), setup_signals(), main()
├── server_bonus.c        # Server with ACK system
├── client.c              # Client: send_char(), main()
├── client_bonus.c        # Client with ACK handlers
├── includes/
│   ├── minitalk.h        # Main header with function prototypes
│   └── minitalk_bonus.h  # Bonus header (acknowledgment system)
├── libft/                # Custom library (ft_atoi, ft_printf, etc.)
├── guides/
│   ├── working_implementation_guide.md    # Complete LSB-first approach
│   ├── bonus_implementation_guide.md      # Acknowledgment system details
│   ├── signal_and_bit_guide.md            # Signal handling fundamentals
│   └── process_and_testing_guide.md       # Testing strategies
└── Makefile              # Build system (all/bonus targets)
```

## Instructions ⚙️

### Building the Project

**Mandatory:**
```bash
make          # Compile client and server
```

**Bonus (with acknowledgments):**
```bash
make bonus    # Compile client_bonus and server_bonus
```

**Both:**
```bash
make all && make bonus
```

### Running the Server
Start the server first (it will display its PID):
```bash
./server          # Mandatory
# or
./server_bonus    # Bonus with ACK system
```
Output example:
```
Server PID: 12345
```

### Running the Client
Send a message to the server using its PID:
```bash
./client <server_pid> "Your message here"
# or
./client_bonus <server_pid> "Your message here"
```

**Examples:**
```bash
# Basic test
./client 12345 "Hello, World!"

# Long message test
./client 12345 "$(python3 -c 'print("test" * 1000)')"

# Unicode & Emoji test 🗡️
./client_bonus 12345 "世界 🌍 émojis 🎀
```

The server will display the received message in real-time!

### Development Commands
- Clean object files: `make clean`
- Remove all generated files: `make fclean`
- Rebuild from scratch: `make re`
- Check norminette: `norminette *.c *.h`

### Terminal Reset (if needed)
If you see garbage characters after Unicode tests:
```bash
reset          # Reset terminal state
clear          # Clear screen
```

## Technical Challenges & Solutions 🧩

### Signal Loss Prevention ✅
**Challenge:** Linux doesn't queue multiple signals of the same type.  
**Solution (Bonus):** Implement acknowledgment system - client waits for server's ACK before sending next bit!
```c
// Client waits for ACK
g_received = 0;           // Reset flag
kill(server_pid, signal); // Send bit
while (g_received == 0)   // Wait for ACK
    pause();
```

### Bit Manipulation Operations (LSB-First Approach) ✅
```c
// CLIENT: Extract bit at position i (LSB first - bits 0→7)
bit = (character >> i) & 1;

// SERVER: Reconstruct character (direct bit setting)
if (signal == SIGUSR2)
    current_char |= (1 << bit_count);  // Set bit at position
```

### Static Variables in Signal Handlers
Use static variables to maintain state between signal handler calls:
```c
static char current_char = 0;  // Builds character
static int  bit_count = 0;     // Tracks bits received (0-7)
```

### Race Conditions Prevention ✅
**Critical:** Reset acknowledgment flag BEFORE sending signal:
```c
g_received = 0;           // Reset FIRST
kill(server_pid, signal); // Then send
while (g_received == 0)   // Then wait
    pause();
```

## Bonus Features ⭐

### Implemented:
- ✅ **Server acknowledgment**: Server sends `SIGUSR2` after each bit received
- ✅ **Completion signal**: Server sends `SIGUSR1` when full message received (`'\0'`)
- ✅ **Client feedback**: Client prints "Message received by server!" on completion
- ✅ **Unicode support**: Handles multi-byte UTF-8 characters (emojis, braille, etc.)
- ✅ **Fast & Reliable**: No artificial delays, pure signal synchronization

### Signal Flow (Bonus):
```
CLIENT                          SERVER
------                          ------
Send bit 0 (SIGUSR1/2) ────►   Receive & store bit 0
                               bit_count++
Wait for ACK...        ◄────   Send SIGUSR2 (ACK)
Receive ACK ✓

Send bit 1 (SIGUSR1/2) ────►   Receive & store bit 1
                               bit_count++
Wait for ACK...        ◄────   Send SIGUSR2 (ACK)
Receive ACK ✓

... (repeat for bits 2-7)

Send bit 7 (SIGUSR1/2) ────►   Receive & store bit 7
                               bit_count = 8
                               Print character!
Wait for ACK...        ◄────   Send SIGUSR2 (ACK)
Receive ACK ✓

... (after last '\0')
                       ◄────   Send SIGUSR1 (DONE)
"Message received!" ✓
Exit
```

## Useful Links 🔗

### Project Guides (in `/guides/`):
- **`working_implementation_guide.md`** ⭐ - Complete working solution
- `signal_and_bit_guide.md` - Signal handling fundamentals
- `bonus_implementation_guide.md` - Acknowledgment system
- `process_and_testing_guide.md` - Testing strategies

### External Resources:

#### Signal Handling:
- [Signals in C – GeeksforGeeks](https://www.geeksforgeeks.org/signals-c-language/)
- [sigaction man page](https://man7.org/linux/man-pages/man2/sigaction.2.html)

#### Bit Manipulation:
- [Bitwise Operators in C – Programiz](https://www.programiz.com/c-programming/bitwise-operators)
- [Bit Manipulation – GeeksforGeeks](https://www.geeksforgeeks.org/bits-manipulation-important-tactics/)
- [Interactive Bitwise Visualizer](https://bitwisecmd.com/)

#### Video Tutorials:
- [Unix Signals (CodeVault)](https://www.youtube.com/watch?v=5We_HtLlAbs)
- [Bitwise Operators in C (Neso Academy)](https://www.youtube.com/watch?v=jlQmeyce65Q)

## License 📜

This project was developed for academic purposes at 42 School, but feel free to use it as reference or inspiration for your own learning journey.