# 🗡️ Minitalk: Working LSB-First Implementation Guide

## ⚔️ **The Approach That Actually Works!**

After extensive testing and comparing with successful implementations, here's the proven approach that passes all testers including Unicode/emoji handling!

---

## 🎯 **Key Principles**

### **1. LSB-First Bit Transmission** ✅
- Send bits from **0 → 7** (Least Significant Bit first)
- Simpler bit manipulation
- More intuitive loop structure
- **This is what works!**

### **2. NO `usleep()` Delays** ✅
- The acknowledgment system handles synchronization
- Adding `usleep()` makes it **SLOWER** and can cause issues
- Let signals do their job!

### **3. Proper Signal Order** ✅
```c
g_received = 0;           // Reset BEFORE sending
kill(server_pid, signal);  // Send signal
while (g_received == 0)    // Wait for ACK
    pause();
```

### **4. Unified Implementation** ✅
- Same code works for mandatory AND bonus
- One header file (`minitalk.h`)
- Simple, clean, maintainable

---

## 📝 **Working Client Implementation**

### **Signal Handlers**
```c
volatile sig_atomic_t g_received = 0;

static void handle_received(int sig)
{
    if (sig == SIGUSR2)
        g_received = 1;  // ACK received
}

static void handle_completion(int sig)
{
    if (sig == SIGUSR1)
    {
        ft_printf("\nMessage received by server!\n");
        exit(0);  // Mission complete!
    }
}
```

### **Setup Signal Handlers**
```c
void setup_signal_handlers(void)
{
    struct sigaction sa_received;
    struct sigaction sa_completion;

    sa_received.sa_handler = handle_received;
    sigemptyset(&sa_received.sa_mask);
    sa_received.sa_flags = SA_RESTART;
    sigaction(SIGUSR2, &sa_received, NULL);
    
    sa_completion.sa_handler = handle_completion;
    sigemptyset(&sa_completion.sa_mask);
    sa_completion.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_completion, NULL);
}
```

### **Send Character - LSB First** ⭐
```c
void send_char(pid_t server_pid, unsigned char chr)
{
    int             i;
    unsigned int    bit;
    unsigned int    signal;

    i = 0;
    while (i < 8)
    {
        bit = (chr >> i) & 1;  // Extract bit i (LSB first!)
        if (bit == 1)
            signal = SIGUSR2;
        else
            signal = SIGUSR1;
        
        g_received = 0;  // RESET BEFORE SENDING!
        
        if (kill(server_pid, signal) == -1)
        {
            ft_error();
            exit(1);
        }
        
        while (g_received == 0)  // Wait for ACK
            pause();
        
        i++;  // Next bit
    }
}
```

### **Main Function**
```c
int main(int argc, char **argv)
{
    pid_t server_pid;
    int   i;

    if (argc != 3)
    {
        ft_printf("Usage: %s <server_pid> <message>\n", argv[0]);
        return (1);
    }
    
    server_pid = ft_atoi(argv[1]);
    if (server_pid <= 0)
    {
        ft_error();
        return (1);
    }
    
    ft_printf("Client PID: %d\n", getpid());
    setup_signal_handlers();
    
    // Send message character by character
    i = 0;
    while (argv[2][i])
    {
        send_char(server_pid, (unsigned char)argv[2][i]);
        i++;
    }
    send_char(server_pid, '\0');  // Don't forget null terminator!
    
    // Wait for completion signal
    while (1)
        pause();
    
    return (0);
}
```

---

## 🖥️ **Working Server Implementation**

### **Global Variable**
```c
pid_t g_client_pid = 0;  // Store client PID for ACKs
```

### **Signal Handler - LSB First** ⭐
```c
void signal_handler(int sig, siginfo_t *info, void *context)
{
    static char current_char = 0;
    static int  bit_count = 0;

    (void)context;
    g_client_pid = info->si_pid;  // Get client PID from signal info
    
    // Build character LSB-first
    if (sig == SIGUSR2)
        current_char |= (1 << bit_count);  // Set bit at position bit_count
    
    bit_count++;
    
    if (bit_count == 8)  // Complete character received
    {
        if (current_char == '\0')
            kill(g_client_pid, SIGUSR1);  // Send completion signal
        else
        {
            write(1, &current_char, 1);  // Print character
            kill(g_client_pid, SIGUSR2);  // Send ACK
        }
        current_char = 0;
        bit_count = 0;
    }
    else
        kill(g_client_pid, SIGUSR2);  // Send ACK for each bit
}
```

### **Setup Signals**
```c
void setup_signals(void)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = signal_handler;  // Use sa_sigaction for siginfo_t
    sa.sa_flags = SA_SIGINFO;  // MUST use SA_SIGINFO to get client PID
    
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        ft_error();
        exit(1);
    }
    if (sigaction(SIGUSR2, &sa, NULL) == -1)
    {
        ft_error();
        exit(1);
    }
}
```

### **Main Function**
```c
int main(void)
{
    ft_printf("Server PID: %d\n", getpid());
    setup_signals();
    
    while (1)
        pause();  // Wait for signals forever
    
    return (0);
}
```

---

## 🔍 **LSB-First vs MSB-First: The Comparison**

### **LSB-First (What Works!)** ✅
```c
// Client: Send bits 0→7
i = 0;
while (i < 8)
{
    bit = (chr >> i) & 1;  // Shift right by i, extract bit 0
    // send bit
    i++;
}

// Server: Build character
if (sig == SIGUSR2)
    current_char |= (1 << bit_count);  // Set bit at position
```

**Why it's better:**
- ✅ Simple increment loop (`i = 0; i < 8; i++`)
- ✅ Direct bit setting with `|= (1 << bit_count)`
- ✅ No need for left-shifting entire character
- ✅ More intuitive (bit 0, bit 1, bit 2...)

### **MSB-First (Old Approach)** ❌
```c
// Client: Send bits 7→0
i = 7;
while (i >= 0)
{
    bit = (chr >> i) & 1;  // Shift right by i, extract bit 0
    // send bit
    i--;
}

// Server: Build character
current_char = current_char << 1;  // Shift entire character left
if (sig == SIGUSR2)
    current_char = current_char | 1;  // Set lowest bit
```

**Why it's worse:**
- ❌ Awkward decrement loop (`i = 7; i >= 0; i--`)
- ❌ Must shift entire character each time
- ❌ Two operations per bit instead of one
- ❌ Less intuitive

---

## 🚀 **Performance Tips**

### **DO:**
- ✅ Use `pause()` for waiting (efficient)
- ✅ Reset `g_received = 0` BEFORE `kill()`
- ✅ Use LSB-first approach
- ✅ Send ACK after EVERY bit
- ✅ Use `SA_SIGINFO` flag to get client PID

### **DON'T:**
- ❌ Use `usleep()` with acknowledgments (unnecessary delay!)
- ❌ Reset `g_received` AFTER `pause()` (race condition!)
- ❌ Forget to send null terminator
- ❌ Forget to send ACK after each bit
- ❌ Mix up signal types (SIGUSR1 vs SIGUSR2)

---

## 🧪 **Testing Strategies**

### **1. Basic ASCII**
```bash
./client <PID> "Hello World"
./client <PID> "42"
./client <PID> "!@#$%^&*()"
```

### **2. Long Messages**
```bash
./client <PID> "$(python3 -c 'print("test" * 1000)')"
```

### **3. Unicode & Emoji** 🗡️
```bash
./client <PID> "世界 🌍 émojis"
./client <PID> "⠀⢀⡀⣌⡹⢻⢯⡽⣛⢏⡞⢾⢳⡩⢮⣅"  # Braille characters
./client <PID> "🗡🥋🎀"  # Kenshin style!
```

### **4. Terminal Reset**
If you see garbage (`�ZG�ZK�Z`), reset terminal:
```bash
reset && ./server
```

---

## 📊 **Signal Flow Diagram**

```
CLIENT                          SERVER
------                          ------
Extract bit 0 ─────SIGUSR1/2───→ Receive bit
                                 Store in position 0
                                 bit_count++
               ←─────SIGUSR2───── Send ACK
Receive ACK
g_received = 1

Extract bit 1 ─────SIGUSR1/2───→ Receive bit
                                 Store in position 1
                                 bit_count++
               ←─────SIGUSR2───── Send ACK
Receive ACK
g_received = 1

... (repeat for bits 2-7)

Extract bit 7 ─────SIGUSR1/2───→ Receive bit
                                 Store in position 7
                                 bit_count = 8
                                 Print character!
               ←─────SIGUSR2───── Send ACK (or SIGUSR1 if '\0')
```

---

## 🎓 **Key Learnings**

1. **LSB-first is simpler** - less mental gymnastics
2. **Acknowledgments replace sleep** - dynamic synchronization
3. **Race conditions are real** - reset flags in the right order
4. **Terminal encoding matters** - `reset` fixes display issues
5. **Test with Unicode** - real-world stress test
6. **Keep it simple** - remove unnecessary complexity

---

## 🏆 **Success Criteria**

Your implementation works when:
- ✅ Passes basic string tests
- ✅ Handles long messages (10,000+ characters)
- ✅ Transmits Unicode/emoji correctly
- ✅ No hanging or timeouts
- ✅ Fast (no artificial delays)
- ✅ Passes norminette
- ✅ Works for both mandatory and bonus

---

## 🗡️ **Oro! Final Words**

> "The sword is a weapon. The art of swordsmanship is learning how to kill. But I prefer the reverse blade - a weapon that protects life."  
> — Himura Kenshin

Your minitalk should be like Kenshin's reverse blade:
- **Precise** - every bit counts
- **Reliable** - no signal loss
- **Clean** - simple code structure
- **Effective** - gets the job done!

**Remember:** If it can transmit Rurouni Kenshin ASCII art perfectly, it can handle anything! ⚔️✨

---

*Guide created with 💜 after many hours of debugging, comparing implementations, and testing with Kenshin ASCII art!*
