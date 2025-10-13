# 📡 Minitalk: Signal Handling & Bit-by-Bit Communication Guide

---

## 📡 **Part 1: Signal Handling in UNIX**

### **What are Signals?**
Signals are **software interrupts** sent to a process to notify it that something has happened. Think of them like notifications your program can receive.

### **The Two Signals You'll Use:**
- **`SIGUSR1`**: User-defined signal #1 (you can think of it as representing binary `0`)
- **`SIGUSR2`**: User-defined signal #2 (you can think of it as representing binary `1`)

---

### **Key Functions:**

#### 1. **`getpid()`** - Get Process ID
```c
#include <unistd.h>

pid_t getpid(void);
```
- Returns the PID (Process ID) of the current process
- **Server** will print this so the **client** knows where to send signals

---

#### 2. **`kill()`** - Send a Signal
```c
#include <signal.h>

int kill(pid_t pid, int sig);
```
- Sends signal `sig` to process `pid`
- **Client** uses this to send `SIGUSR1` or `SIGUSR2` to the **server**
- Returns 0 on success, -1 on error

**Example:**
```c
kill(server_pid, SIGUSR1);  // Send SIGUSR1 to server
kill(server_pid, SIGUSR2);  // Send SIGUSR2 to server
```

---

#### 3. **`signal()`** - Simple Signal Handler (deprecated, but simpler)
```c
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```
- Registers a function to be called when a signal arrives
- **Not allowed in your project** (use `sigaction` instead)

---

#### 4. **`sigaction()`** - Robust Signal Handler ✅
```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```
- More reliable and configurable than `signal()`
- This is what you MUST use for minitalk

**Structure:**
```c
struct sigaction {
    void (*sa_handler)(int);           // Handler function
    void (*sa_sigaction)(int, siginfo_t *, void *);  // Advanced handler
    sigset_t sa_mask;                  // Signals to block during handler
    int sa_flags;                      // Special flags
    void (*sa_restorer)(void);         // Don't use this
};
```

---

#### 5. **`sigemptyset()` & `sigaddset()`** - Signal Set Management
```c
int sigemptyset(sigset_t *set);      // Initialize empty signal set
int sigaddset(sigset_t *set, int signum);  // Add signal to set
```

---

#### 6. **`pause()`** - Wait for Signal
```c
#include <unistd.h>

int pause(void);
```
- Suspends the process until a signal is received
- Perfect for the server to wait for incoming signals

---

#### 7. **`usleep()`** - Microsecond Sleep
```c
#include <unistd.h>

int usleep(useconds_t usec);
```
- Sleep for `usec` microseconds (1 second = 1,000,000 microseconds)
- **CRITICAL**: You need small delays between signals (100-500 microseconds)
- Why? The subject mentions "Linux doesn't queue signals" — if you send too fast, signals get lost!

---

## 🔢 **Part 2: Bit-by-Bit Communication**

### **The Core Concept:**
You need to send **characters** (8 bits each) from client to server using only two signals.

### **Binary Representation:**
Every character is made of 8 bits. For example:
- `'A'` = `65` = `01000001` in binary
- `'Z'` = `90` = `01011010` in binary
- `'H'` = `72` = `01001000` in binary
- `'\0'` = `0` = `00000000` in binary

---

### **The Strategy:**

1. **Client Side:**
   - Take a character (e.g., `'A'`)
   - Extract each bit (from MSB to LSB, or LSB to MSB — your choice)
   - Send `SIGUSR1` for bit `0`
   - Send `SIGUSR2` for bit `1`
   - Wait a tiny bit between each signal (`usleep(100)`)

2. **Server Side:**
   - Start with an empty character (`char c = 0`)
   - Each time a signal arrives:
     - Shift the current character left by 1 bit
     - If `SIGUSR2` arrived, add `1` to the character
     - If `SIGUSR1` arrived, add `0` (do nothing)
   - After 8 signals, you have a complete character!
   - Print it and reset

---

## 🧮 **Part 3: Bit Manipulation Deep Dive**

### **What is a Bit?**
A **bit** (binary digit) is the smallest unit of data — it's either `0` or `1`.

### **Common Bit Operations:**

#### **1. Left Shift (`<<`)**
Shifts all bits to the left by a specified number of positions.
- Left bits fall off
- Right bits are filled with `0`

```c
int x = 5;        // Binary: 00000101
int y = x << 1;   // Binary: 00001010 (= 10)
int z = x << 2;   // Binary: 00010100 (= 20)
```

**Visual:**
```
Original:  0 0 0 0 0 1 0 1  (5)
<< 1:      0 0 0 0 1 0 1 0  (10)
<< 2:      0 0 0 1 0 1 0 0  (20)
```

---

#### **2. Right Shift (`>>`)**
Shifts all bits to the right by a specified number of positions.

```c
int x = 20;       // Binary: 00010100
int y = x >> 1;   // Binary: 00001010 (= 10)
int z = x >> 2;   // Binary: 00000101 (= 5)
```

**Visual:**
```
Original:  0 0 0 1 0 1 0 0  (20)
>> 1:      0 0 0 0 1 0 1 0  (10)
>> 2:      0 0 0 0 0 1 0 1  (5)
```

---

#### **3. Bitwise AND (`&`)**
Compares each bit; result is `1` only if BOTH bits are `1`.

```c
int a = 12;       // Binary: 00001100
int b = 10;       // Binary: 00001010
int c = a & b;    // Binary: 00001000 (= 8)
```

**Truth Table:**
```
0 & 0 = 0
0 & 1 = 0
1 & 0 = 0
1 & 1 = 1
```

---

#### **4. Bitwise OR (`|`)**
Compares each bit; result is `1` if EITHER bit is `1`.

```c
int a = 12;       // Binary: 00001100
int b = 10;       // Binary: 00001010
int c = a | b;    // Binary: 00001110 (= 14)
```

**Truth Table:**
```
0 | 0 = 0
0 | 1 = 1
1 | 0 = 1
1 | 1 = 1
```

---

#### **5. Bitwise XOR (`^`)**
Compares each bit; result is `1` if bits are DIFFERENT.

```c
int a = 12;       // Binary: 00001100
int b = 10;       // Binary: 00001010
int c = a ^ b;    // Binary: 00000110 (= 6)
```

---

#### **6. Bitwise NOT (`~`)**
Inverts all bits (`0` becomes `1`, `1` becomes `0`).

```c
int a = 5;        // Binary: 00000101
int b = ~a;       // Binary: 11111010 (= -6 in two's complement)
```

---

### **💡 Practical Examples for Minitalk:**

#### **Example 1: Extract a Specific Bit**
To check if bit `n` is `0` or `1`:

```c
// Check if bit 3 of character 'A' (65 = 01000001) is set
char c = 'A';
int bit_position = 3;

// Method: Right shift and mask with 1
int bit_value = (c >> bit_position) & 1;
// Result: (01000001 >> 3) & 1 = 00001000 & 1 = 0
```

**Step-by-step:**
```
c = 01000001 (65, 'A')
c >> 3 = 00001000 (shift right 3 positions)
00001000 & 00000001 = 00000000 (mask with 1)
Result: 0
```

---

#### **Example 2: Send Character Bit-by-Bit (Client)**
```c
void send_char(pid_t server_pid, char c)
{
    int bit;
    int i;

    i = 7;  // Start from MSB (most significant bit)
    while (i >= 0)
    {
        bit = (c >> i) & 1;  // Extract bit at position i
        
        if (bit == 0)
            kill(server_pid, SIGUSR1);  // Send 0
        else
            kill(server_pid, SIGUSR2);  // Send 1
        
        usleep(100);  // Small delay to avoid signal loss
        i--;
    }
}
```

**Example for 'A' (01000001):**
```
i=7: (01000001 >> 7) & 1 = 0 → SIGUSR1
i=6: (01000001 >> 6) & 1 = 1 → SIGUSR2
i=5: (01000001 >> 5) & 1 = 0 → SIGUSR1
i=4: (01000001 >> 4) & 1 = 0 → SIGUSR1
i=3: (01000001 >> 3) & 1 = 0 → SIGUSR1
i=2: (01000001 >> 2) & 1 = 0 → SIGUSR1
i=1: (01000001 >> 1) & 1 = 0 → SIGUSR1
i=0: (01000001 >> 0) & 1 = 1 → SIGUSR2
```

---

#### **Example 3: Reconstruct Character (Server)**
```c
void signal_handler(int sig)
{
    static char current_char = 0;
    static int bit_count = 0;

    // Shift left to make room for new bit
    current_char = current_char << 1;
    
    // If SIGUSR2, add 1; if SIGUSR1, add 0 (do nothing)
    if (sig == SIGUSR2)
        current_char = current_char | 1;
    
    bit_count++;
    
    // After 8 bits, we have a complete character
    if (bit_count == 8)
    {
        write(1, &current_char, 1);  // Print the character
        current_char = 0;             // Reset
        bit_count = 0;                // Reset
    }
}
```

**Reconstruction example for 'A':**
```
Start: current_char = 00000000, bit_count = 0

Signal 1 (SIGUSR1): current_char = (00000000 << 1) | 0 = 00000000, bit_count = 1
Signal 2 (SIGUSR2): current_char = (00000000 << 1) | 1 = 00000001, bit_count = 2
Signal 3 (SIGUSR1): current_char = (00000001 << 1) | 0 = 00000010, bit_count = 3
Signal 4 (SIGUSR1): current_char = (00000010 << 1) | 0 = 00000100, bit_count = 4
Signal 5 (SIGUSR1): current_char = (00000100 << 1) | 0 = 00001000, bit_count = 5
Signal 6 (SIGUSR1): current_char = (00001000 << 1) | 0 = 00010000, bit_count = 6
Signal 7 (SIGUSR1): current_char = (00010000 << 1) | 0 = 00100000, bit_count = 7
Signal 8 (SIGUSR2): current_char = (00100000 << 1) | 1 = 01000001 = 'A', bit_count = 8 ✅

Print 'A', reset current_char and bit_count.
```

---

## 💡 **Visual Example: Sending "Hi"**

### **Character 'H' = 72 = 01001000**
```
Client sends:
Bit 7 (0) → SIGUSR1 → Server: 00000000
Bit 6 (1) → SIGUSR2 → Server: 00000001
Bit 5 (0) → SIGUSR1 → Server: 00000010
Bit 4 (0) → SIGUSR1 → Server: 00000100
Bit 3 (1) → SIGUSR2 → Server: 00001001
Bit 2 (0) → SIGUSR1 → Server: 00010010
Bit 1 (0) → SIGUSR1 → Server: 00100100
Bit 0 (0) → SIGUSR1 → Server: 01001000 = 'H' ✅
```

### **Character 'i' = 105 = 01101001**
```
Client sends:
Bit 7 (0) → SIGUSR1 → Server: 00000000
Bit 6 (1) → SIGUSR2 → Server: 00000001
Bit 5 (1) → SIGUSR2 → Server: 00000011
Bit 4 (0) → SIGUSR1 → Server: 00000110
Bit 3 (1) → SIGUSR2 → Server: 00001101
Bit 2 (0) → SIGUSR1 → Server: 00011010
Bit 1 (0) → SIGUSR1 → Server: 00110100
Bit 0 (1) → SIGUSR2 → Server: 01101001 = 'i' ✅
```

---

## 🧠 **Pseudocode Summary:**

### **Server Pseudocode:**
```
1. Print server PID
2. Set up signal handlers for SIGUSR1 and SIGUSR2
3. Loop forever (pause and wait for signals)

Signal Handler:
- Static variable: bit_count (tracks how many bits received)
- Static variable: current_char (builds the character)

- Shift current_char left by 1 bit
- If signal is SIGUSR2, add 1 to current_char
- Increment bit_count

- If bit_count == 8:
    * Print current_char
    * Reset bit_count to 0
    * Reset current_char to 0
```

### **Client Pseudocode:**
```
1. Get server PID from arguments
2. Get string to send from arguments
3. For each character in the string:
    - For each bit in the character (8 bits, starting from MSB):
        * Extract the bit using (c >> bit_position) & 1
        * If bit is 0, send SIGUSR1 to server
        * If bit is 1, send SIGUSR2 to server
        * Sleep for 100-500 microseconds (usleep)
4. Send null terminator '\0' the same way
```

---

## ⚠️ **Critical Gotchas:**

### 1. **Signal Loss**
> "The Linux system doesn't queue signals when you already have pending signals of that type!"

**Solution:** Add `usleep(100)` or `usleep(500)` between each `kill()` call to give the server time to process.

### 2. **Global Variables**
You're allowed **ONE global variable per program**. Use it wisely!
- Server: Could store the current character being built (or use static variables in handler)
- Client: Might not need one at all

### 3. **Don't Forget the Null Terminator**
Send `'\0'` at the end of your string so the server knows when to print a newline or handle end of message!

### 4. **Static vs Global**
Static variables inside functions persist between function calls but are scoped to that function. This is perfect for the signal handler!

---

## 📚 **Recommended Learning Resources:**

### **Signal Handling:**
1. **Beej's Guide to Unix IPC - Signals Section**
   - URL: https://beej.us/guide/bgipc/html/split/signals.html
   - Why: Clear, beginner-friendly explanation with code examples

2. **GNU C Library Manual - Signal Handling**
   - URL: https://www.gnu.org/software/libc/manual/html_node/Signal-Handling.html
   - Why: Official documentation, very detailed

3. **Video: Unix Signals (CodeVault)**
   - URL: https://www.youtube.com/watch?v=5We_HtLlAbs
   - Why: Visual explanation of how signals work

4. **Article: Signal Handling in Linux**
   - URL: https://www.geeksforgeeks.org/signals-c-language/
   - Why: Good examples with sigaction

### **Bit Manipulation:**
1. **Video: Bitwise Operators in C (freeCodeCamp)**
   - URL: https://www.youtube.com/watch?v=jlQmeyce65Q
   - Why: Visual demonstrations of each operator

2. **Interactive: Bit Manipulation Visualizer**
   - URL: https://bitwisecmd.com/
   - Why: See bit operations in real-time

3. **Article: Bitwise Operators in C**
   - URL: https://www.programiz.com/c-programming/bitwise-operators
   - Why: Simple examples with visual diagrams

4. **Practice: LeetCode Bit Manipulation Problems**
   - URL: https://leetcode.com/tag/bit-manipulation/
   - Why: Hands-on practice with bit problems

### **Man Pages (in your terminal):**
```bash
man 2 signal
man 2 sigaction
man 2 kill
man 2 getpid
man 3 usleep
man 2 pause
```

---

## 🎯 **Quick Reference:**

### **Extract bit at position `i`:**
```c
int bit = (character >> i) & 1;
```

### **Set bit at position `i` to 1:**
```c
character = character | (1 << i);
```

### **Clear bit at position `i` to 0:**
```c
character = character & ~(1 << i);
```

### **Toggle bit at position `i`:**
```c
character = character ^ (1 << i);
```

### **Shift left and add bit:**
```c
character = (character << 1) | bit;
```

---

## 🚀 **Next Steps:**

1. **Start with the server** - It's simpler and needs to run first
2. **Test with simple signals** - Make sure signal handling works before adding bit logic
3. **Add bit reconstruction** - Implement the 8-bit character building
4. **Build the client** - Send one character first, then full strings
5. **Test thoroughly** - Try different strings, special characters, long messages
6. **Optimize delays** - Find the sweet spot for `usleep()` timing

---

Good luck! 🎉
