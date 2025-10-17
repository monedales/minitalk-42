# 🌟 Minitalk Bonus: Working Implementation (LSB-First Approach)

> **UPDATE:** This guide now reflects the WORKING approach that passes all testers!  
> Key changes: LSB-first, no usleep(), proper signal order ✅

## 📍 Project Completion Date: October 15, 2025

---

## 🎯 What is the Bonus?

The bonus adds **reliability** and **acknowledgment** to the basic minitalk communication system. It ensures that no signals are lost, even when transmitting very large files.

### **Mandatory vs Bonus Comparison:**

| Feature | Mandatory | Bonus |
|---------|-----------|-------|
| **Communication** | One-way (client → server) | Two-way (bidirectional) |
| **Reliability** | Fire and forget | Acknowledged delivery |
| **Signal Loss** | Possible with large files | Prevented |
| **Completion Signal** | No confirmation | Server confirms receipt |
| **Large Files** | May fail | Fully supported |
| **Unicode** | Works | Works |

---

## 🔄 How the Bonus Works

### **Communication Flow:**

```
CLIENT                           SERVER
  │                                │
  │──── Send bit (SIGUSR1/2) ────→│
  │                                │ Receive & process bit
  │                                │
  │←─── ACK (SIGUSR2) ────────────│ Send acknowledgment
  │                                │
  │ Wait for ACK                   │
  │                                │
  │──── Send next bit ───────────→│
  │                                │
  │    ... repeat for all bits ... │
  │                                │
  │──── Send '\0' ───────────────→│
  │                                │ Detect end of message
  │                                │
  │←─── SIGUSR1 ──────────────────│ Send completion signal
  │                                │
  │ Print success & exit           │
```

---

## 🛠️ Implementation Details

### **1. Server Bonus Changes**

#### **A) Global Variable for Client PID**
```c
pid_t g_client_pid = 0;
```
- Stores the client's PID to send acknowledgments back
- Must start with `g_` for norminette compliance

#### **B) Signal Handler Changes**
```c
void signal_handler(int sig, siginfo_t *info, void *context)
```

**Key changes:**
1. **New parameters:**
   - `siginfo_t *info` - Contains client information
   - `void *context` - Unused but required

2. **Get client PID:**
   ```c
   g_client_pid = info->si_pid;
   ```

3. **Send ACK after each bit:**
   ```c
   bit_count++;
   kill(g_client_pid, SIGUSR2);  // ACK: "Got it, send next bit"
   ```

4. **Send completion signal:**
   ```c
   if (current_char == '\0')
   {
       kill(g_client_pid, SIGUSR1);  // "Message complete!"
   }
   ```

#### **C) Setup Signals Changes**
```c
sa.sa_sigaction = signal_handler;  // Use sa_sigaction (not sa_handler)
sa.sa_flags = SA_SIGINFO;          // Required flag to get siginfo_t
```

---

### **2. Client Bonus Changes**

#### **A) Global Variable for Acknowledgment**
```c
volatile sig_atomic_t g_received = 0;
```
- **`volatile`** - Tells compiler value can change unexpectedly (by signal)
- **`sig_atomic_t`** - Signal-safe type (atomic operations)
- **`g_` prefix** - Norminette requirement for global variables

#### **B) Signal Handlers**

**Handler 1: Acknowledgment (SIGUSR2)**
```c
void handle_received(int sig)
{
    if (sig == SIGUSR2)
        g_received = 1;  // Server acknowledged, proceed to next bit
}
```

**Handler 2: Completion (SIGUSR1)**
```c
void handle_completion(int sig)
{
    if (sig == SIGUSR1)
    {
        ft_printf("✓ Message received by server!\n");
        exit(0);  // Clean exit
    }
}
```

#### **C) Setup Signal Handlers**
```c
void setup_signal_handlers(void)
{
    struct sigaction sa_received;
    struct sigaction sa_completion;

    // Setup for SIGUSR2 (acknowledgment)
    sa_received.sa_handler = handle_received;
    sigemptyset(&sa_received.sa_mask);
    sa_received.sa_flags = SA_RESTART;  // Restart interrupted syscalls
    sigaction(SIGUSR2, &sa_received, NULL);

    // Setup for SIGUSR1 (completion)
    sa_completion.sa_handler = handle_completion;
    sigemptyset(&sa_completion.sa_mask);
    sa_completion.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_completion, NULL);
}
```

#### **D) Modified send_char()**
```c
void send_char(pid_t server_pid, unsigned char chr)
{
    // ... bit extraction code ...
    
    if (kill(server_pid, signal) == -1)
    {
        ft_error();
        exit(1);
    }
    
    // NEW: Wait for acknowledgment
    while (!g_received)
        pause();  // Sleep until signal arrives
    
    g_received = 0;  // Reset for next bit
    usleep(200);     // Small delay for reliability
    
    // ... continue loop ...
}
```

#### **E) Modified main()**
```c
int main(int argc, char **argv)
{
    // ... argument validation ...
    
    ft_printf("Client PID: %d\n", getpid());  // Show client PID
    setup_signal_handlers();                   // Setup signal handlers
    send_string(server_pid, argv[2]);         // Send message
    
    // Wait for completion signal
    while (1)
        pause();  // Server will send SIGUSR1 when done
    
    return (0);  // Never reached (exits in handler)
}
```

---

## 📂 File Structure for Bonus

### **Required Files:**
```
minitalk/
├── server_bonus.c         # Server with acknowledgment
├── client_bonus.c         # Client signal handlers & send functions
├── main_client_bonus.c    # Client main (if split for norminette)
├── minitalk_bonus.h       # Bonus header
└── Makefile               # Updated with bonus targets
```
---

## 🔧 Makefile Bonus Configuration

```makefile
# Bonus programs
CLIENT_BONUS = client_bonus
SERVER_BONUS = server_bonus

# Bonus source files
SRC_CLIENT_BONUS = client_bonus.c main_client_bonus.c  # If split
SRC_SERVER_BONUS = server_bonus.c

# Bonus rule
bonus: $(CLIENT_BONUS) $(SERVER_BONUS)

$(CLIENT_BONUS): $(OBJS_CLIENT_BONUS) $(LIBFT)
	@$(CC) $(CFLAGS) $(OBJS_CLIENT_BONUS) $(LIBFT) -o $(CLIENT_BONUS)

$(SERVER_BONUS): $(OBJS_SERVER_BONUS) $(LIBFT)
	@$(CC) $(CFLAGS) $(OBJS_SERVER_BONUS) $(LIBFT) -o $(SERVER_BONUS)
```

---

## 🧪 Testing the Bonus

### **Test 1: Basic Message**
```bash
# Terminal 1
./server_bonus
# Output: Server PID: 12345

# Terminal 2
./client_bonus 12345 "Hello Bonus!"
# Output: 
# Client PID: 12346
# Hello Bonus!✓ Message received by server!
```

### **Test 2: Large Message**
```bash
./client_bonus 12345 "This is a very long message with many characters to test the acknowledgment system reliability!"
```

### **Test 3: Unicode**
```bash
./client_bonus 12345 "Hello 世界 🌍 Café!"
# Should display perfectly with emojis and special characters
```

### **Test 4: Large File (Tester)**
```bash
python3 tester.py
# Should pass the 18KB file test with bonus!
```
---

## 📊 Signal Flow Diagram

```
BIT TRANSMISSION (8 times per character):
═══════════════════════════════════════════

CLIENT                                    SERVER
┌────────┐                              ┌────────┐
│ Bit=1? │                              │        │
│  Yes   │                              │        │
└───┬────┘                              │        │
    │                                   │        │
    │ kill(pid, SIGUSR2)                │        │
    ├──────────────────────────────────→│ Receive│
    │                                   │ SIGUSR2│
    │                                   │        │
    │                                   │ OR bit │
    │                                   │ with 1 │
    │                                   │        │
    │              kill(client, SIGUSR2)│        │
    │←──────────────────────────────────┤ Send   │
    │                                   │ ACK    │
┌───┴────┐                              │        │
│ Receive│                              │        │
│ SIGUSR2│                              │        │
│        │                              │        │
│ Set    │                              │        │
│ flag=1 │                              │        │
└───┬────┘                              │        │
    │                                   │        │
    │ Continue to next bit              │        │
    ▼                                   ▼


CHARACTER COMPLETION:
═══════════════════════

After 8 bits received:
                                        ┌────────┐
                                        │ Char   │
                                        │ = '\0'?│
                                        └───┬────┘
                                            │ Yes
                                    ┌───────┴───────┐
                                    │ kill(client,  │
                                    │ SIGUSR1)      │
                                    └───────┬───────┘
                                            │
CLIENT                                      │
┌────────┐                                  │
│ Receive│←─────────────────────────────────┘
│ SIGUSR1│
│        │
│ Print  │
│ Success│
│        │
│ exit(0)│
└────────┘
```

---

## 🎓 Key Concepts Learned

### **1. Bidirectional Signal Communication**
- Not just sending signals, but receiving them too
- Both processes act as sender AND receiver

### **2. Volatile & Atomic Variables**
- `volatile` prevents compiler optimizations
- `sig_atomic_t` ensures atomic read/write operations
- Critical for signal handlers

### **3. Signal Safety**
- Only async-signal-safe functions in handlers
- `write()` is safe, `printf()` is not
- `exit()` is safe for termination

### **4. Synchronization**
- Using `pause()` to wait for signals
- Using global flags for communication between handler and main code
- Prevents race conditions

### **5. Error Handling**
- Always check `kill()` return value
- Validate PIDs before use
- Handle edge cases (null strings, invalid PIDs)
---

## 📚 Additional Resources

### **Man Pages:**
```bash
man sigaction    # Signal handling
man kill         # Sending signals
man pause        # Waiting for signals
man usleep       # Microsecond delays
```

### **Concepts:**
- **Race Conditions**: When signal arrives during processing
- **Signal Queuing**: Linux doesn't queue duplicate signals
- **Async-Signal-Safe**: Functions safe to use in signal handlers
- **Atomic Operations**: Indivisible read/modify/write
---

*Guide created: October 15, 2025*  
*Project: Minitalk (42 School)*  
*Status: Mandatory ✅ | Bonus ✅*
