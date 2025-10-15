# 🔄 Process Management & Testing Guide for Minitalk

## 📍 Why You Need Two Processes

Minitalk requires **two separate processes** running at the same time:
1. **Server** - Listens for signals and displays messages
2. **Client** - Sends signals to the server

Since your terminal can only run one foreground process at a time, you need to use either:
- **Background processes** (using `&`)
- **Multiple terminals**

---

## 🎯 Method 1: Using Background Processes (Single Terminal)

### **The `&` Symbol**

When you add `&` at the end of a command, it runs in the **background**:

```bash
./server &
```

**What happens:**
- ✅ Server starts running
- ✅ You get your terminal prompt back immediately
- ✅ You can run more commands while server is running
- ✅ Server output still appears in the terminal

### **Complete Testing Workflow:**

```bash
# 1. Compile the project
make

# 2. Start server in background
./server &
# Output: [1] 2577573
#         Server PID: 2577573

# 3. Run client (server PID from step 2)
./client 2577573 "Hello, World!"

# 4. Server displays: Hello, World!

# 5. When done, kill the background server
kill 2577573
# Or: pkill server
```

---

## 🖥️ Method 2: Using Two Terminals (Recommended for Beginners)

### **Step-by-Step:**

**Terminal 1 (Server):**
```bash
cd /path/to/minitalk
make
./server
# Output: Server PID: 12345
# (Terminal waits here, server is running)
```

**Terminal 2 (Client):**
```bash
cd /path/to/minitalk
./client 12345 "Hello!"
# Server in Terminal 1 displays: Hello!
```

**Advantages:**
- ✅ Clearer separation of server and client
- ✅ Easier to see which output comes from which process
- ✅ No need to manage background jobs

---

## 🛠️ Managing Background Processes

### **Useful Commands:**

| Command | Description |
|---------|-------------|
| `./server &` | Start server in background |
| `jobs` | List all background jobs |
| `fg` | Bring most recent background job to foreground |
| `fg %1` | Bring job #1 to foreground |
| `bg` | Resume a stopped job in background |
| `kill <PID>` | Stop a specific process by PID |
| `pkill server` | Kill all processes named "server" |
| `Ctrl+C` | Stop foreground process |
| `Ctrl+Z` | Pause foreground process (can resume with `bg`) |

### **Example Session:**

```bash
# Start server in background
./server &
[1] 12345
Server PID: 12345

# Check running jobs
jobs
[1]+  Running    ./server &

# Run client
./client 12345 "Test"

# Kill server when done
kill 12345
# Or
pkill server

# Verify it's gone
jobs
# (empty output)
```

---

## 🧪 Testing Scenarios

### **Test 1: Simple Character**
```bash
./server &
./client <PID> "A"
# Expected: Server displays "A"
```

### **Test 2: Full Message**
```bash
./server &
./client <PID> "Hello, World!"
# Expected: Server displays "Hello, World!"
```

### **Test 3: Multiple Messages**
```bash
./server &
./client <PID> "First"
./client <PID> "Second"
./client <PID> "Third"
# Expected: Server displays all three messages
```

### **Test 4: Special Characters**
```bash
./server &
./client <PID> "!@#$%^&*()"
./client <PID> "123 456 789"
./client <PID> "Tab\there"
# Expected: All special chars display correctly
```

### **Test 5: Edge Cases**
```bash
./server &
./client <PID> ""           # Empty string
./client <PID> "A"          # Single character
./client <PID> "Very long message with many words and characters to test buffer handling"
```

### **Test 6: Error Cases**
```bash
./client                    # No arguments - should show usage
./client 123                # Missing message - should show usage
./client abc "test"         # Invalid PID - should error
./client 99999 "test"       # Non-existent PID - should error
```

---

## ⚠️ Common Issues

### **Issue 1: "Error" when sending signals**

**Symptom:**
```bash
./client 12345 "test"
Error
```

**Possible causes:**
- Server is not running
- Wrong PID
- Server crashed

**Solution:**
```bash
# Check if server is running
ps aux | grep server
# Or
jobs

# Restart server if needed
./server &
```

### **Issue 2: Server receives garbled text**

**Symptom:** Server displays weird characters instead of your message

**Possible causes:**
- Bit order is wrong (LSB vs MSB)
- Signal delay is too short
- Bit extraction logic error

**Solution:**
- Check your bit extraction: `bit = (chr >> i) & 1`
- Ensure you start from bit 7 down to bit 0
- Increase `usleep()` delay (try 200-500)

### **Issue 3: Server receives partial message**

**Symptom:** Only part of the message appears

**Possible causes:**
- Signals being lost (too fast)
- Forgot to send null terminator

**Solution:**
- Increase `usleep()` delay
- Make sure to send `'\0'` at the end of the string

### **Issue 4: Can't kill background server**

**Symptom:** Server keeps running even after testing

**Solution:**
```bash
# Find the PID
ps aux | grep server
# Or
jobs

# Kill it
kill <PID>
# Or force kill
kill -9 <PID>
# Or kill by name
pkill -9 server
```

---

## 🎓 Understanding Process IDs (PIDs)

### **What is a PID?**
- Every running process has a unique Process ID (PID)
- PIDs are numbers assigned by the operating system
- You need the server's PID to send it signals

### **How to get a PID:**

**Method 1: Server prints it**
```bash
./server
Server PID: 12345  ← This is the PID
```

**Method 2: Using `ps` command**
```bash
ps aux | grep server
# Shows: maria-ol  12345  0.0  0.0  ./server
#                  ^^^^^
#                  This is the PID
```

**Method 3: Using `$!` (last background process)**
```bash
./server &
echo $!  # Prints the PID of the last background process
```

---

## 🔍 Debugging Tips

### **Enable Verbose Output:**
Add debug prints to see what's happening:

```c
// In client.c
ft_printf("Sending bit %d: %d\n", i, bit);

// In server.c (NOT in signal handler!)
// Use write() instead:
char msg[] = "Received bit\n";
write(1, msg, 13);
```

### **Visual Bit Representation:**
Test with characters whose binary you know:
- `'A'` (65) = `01000001`
- `'B'` (66) = `01000010`
- `'0'` (48) = `00110000`

```bash
# Test with 'A' and check if server receives it correctly
./client <PID> "A"
```

### **Slow Down Signals:**
If you're having issues, temporarily increase the delay:
```c
usleep(1000);  // 1 millisecond instead of 100 microseconds
```

---

## 📊 Process State Diagram

```
┌─────────────────┐
│  Terminal 1     │
│  ./server &     │
│  PID: 12345     │
│  [Listening]    │
└────────┬────────┘
         │
         │ Receives SIGUSR1/SIGUSR2
         │
         ▼
┌─────────────────┐
│  Terminal 2     │
│  ./client 12345 │
│  "Hello"        │
│  [Sending]      │
└─────────────────┘

Process Flow:
1. Client extracts bit from 'H'
2. Client sends SIGUSR1 or SIGUSR2
3. Server receives signal
4. Server reconstructs bit
5. After 8 bits, server prints character
6. Repeat for each character
```

---

## 🎯 Quick Reference: Testing Checklist

Before submitting your project, test:

- [ ] Server starts and displays PID
- [ ] Client validates PID (positive number)
- [ ] Client validates argument count
- [ ] Single character transmission works
- [ ] Multi-character messages work
- [ ] Empty string handled gracefully
- [ ] Special characters work (!@#$%^&*)
- [ ] Numbers work (0-9)
- [ ] Spaces work
- [ ] Long messages work (100+ characters)
- [ ] Multiple sequential messages work
- [ ] Error messages are clear
- [ ] No memory leaks (use `valgrind`)
- [ ] No compiler warnings
- [ ] Makefile works correctly (make, make clean, make fclean, make re)

---

## 💡 Pro Tips

1. **Always compile after changes:**
   ```bash
   make && ./server &
   ```

2. **Use aliases for quick testing:**
   ```bash
   alias mt-server='cd ~/minitalk && make && ./server'
   alias mt-test='cd ~/minitalk && ./client'
   ```

3. **Test incrementally:**
   - First: Single character
   - Then: Short word
   - Then: Long message
   - Finally: Edge cases

4. **Keep server running during multiple tests:**
   ```bash
   ./server &
   PID=$!  # Save the PID
   ./client $PID "Test 1"
   ./client $PID "Test 2"
   ./client $PID "Test 3"
   kill $PID  # Clean up when done
   ```

5. **Use scripts for automated testing:**
   ```bash
   #!/bin/bash
   ./server &
   SERVER_PID=$!
   sleep 1
   ./client $SERVER_PID "Test message 1"
   ./client $SERVER_PID "Test message 2"
   kill $SERVER_PID
   ```

---

**Happy Testing! 🚀**

*Remember: If something doesn't work, check the server is still running first!*

---

*Last updated: October 15, 2025*
