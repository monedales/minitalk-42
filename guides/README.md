# 📚 Minitalk Guides

Welcome to the minitalk implementation guides! These documents were created during the development process to understand and implement the project successfully.

## 🗡️ **START HERE**
**→ [`working_implementation_guide.md`](working_implementation_guide.md)** ⭐  
The complete, tested, working implementation with LSB-first approach. This is what passed all testers including Unicode/emoji tests!

## 📖 Other Guides

### [`signal_and_bit_guide.md`](signal_and_bit_guide.md)
Comprehensive reference for:
- UNIX signal handling (`sigaction`, `kill`, `pause`)
- Bit manipulation techniques
- Signal safety and best practices
- Fundamental concepts

### [`bonus_implementation_guide.md`](bonus_implementation_guide.md)
Detailed explanation of the bonus acknowledgment system:
- Bidirectional communication
- Client-server synchronization
- Signal flow diagrams
- Bonus requirements

### [`process_and_testing_guide.md`](process_and_testing_guide.md)
Testing strategies and process management:
- How to test your implementation
- Terminal reset techniques
- Unicode handling tips
- Common pitfalls

## 🎯 Quick Implementation Checklist

- ✅ Use LSB-first (bits 0→7)
- ✅ NO `usleep()` with acknowledgments
- ✅ Reset `g_received = 0` BEFORE `kill()`
- ✅ Use `SA_SIGINFO` flag to get client PID
- ✅ Send ACK after EVERY bit
- ✅ Test with Unicode/emoji (🗡️⚔️)

## 🚀 Testing Your Implementation

```bash
# Basic test
./server &
./client <PID> "Hello World"

# Long message test
./client <PID> "$(python3 -c 'print("test" * 1000)')"

# Unicode test (Kenshin style!)
./client <PID> "⠀⢀⡀⣌⡹⢻⢯⡽⣛⢏⡞ 🗡️🥋🎀"

# If you see garbage, reset terminal
reset && ./server
```

## 📝 Evolution Notes

This project went through several iterations:
1. Initial MSB-first approach (bit 7→0)
2. Added `usleep()` delays → made it SLOW ❌
3. Tried different synchronization methods
4. **Final:** LSB-first (bit 0→7) with pure acknowledgment system ✅

The working approach was discovered by:
- Testing extensively with Unicode/emoji
- Comparing with successful peer implementations
- Understanding race conditions in signal handling
- Removing unnecessary complexity (`usleep()`, `send_string()`, separate main files)

## 🏆 Success Story

**Final test:** Transmitted complete Rurouni Kenshin ASCII art (2000+ bytes of braille Unicode characters + emojis) perfectly! ⚔️

If it can handle that, it can handle anything the evaluator throws at it! 💪

---

*"The sword that protects, not the sword that kills."* — Himura Kenshin 🗡️
