[![arduino-library-badge](https://www.ardu-badge.com/badge/EEPROM_Hex_Dump.svg?)](https://www.ardu-badge.com/Arduino-EEPROM-Hex-Dump) ![version](https://img.shields.io/github/v/tag/Reefwing-Software/Arduino-EEPROM-Hex-Dump) ![license](https://img.shields.io/badge/license-MIT-green) ![release](https://img.shields.io/github/release-date/Reefwing-Software/Arduino-EEPROM-Hex-Dump?color="red") ![open source](https://badgen.net/badge/open/source/blue?icon=github)

# Arduino EEPROM Hex Dump
An Arduino sketch which outputs the contents of the EEPROM to the Serial Monitor, in both hexadecimal and ASCII format. The following is an extract from my Medium article regarding Memory Usage on the Arduino and EEPROM...

There will come a point where you will want to store data which isn't lost when your microprocessor loses power. This is particularly handy for storing calibration or configuration values, to save you having to re-calibrate some piece of hardware (e.g., IMUs, and servos) every time the power cycles. One solution for Arduino boards is using EEPROM (Electrically Erasable Programmable Read-Only Memory).

EEPROM is a type of non-volatile ROM that enables individual bytes of data to be erased and reprogrammed. It is used to store small amounts of data which are written occasionally and then read multiple times.

EEPROM is often contained within the microprocessor, and different boards will have different amounts. For example, the ATMega328P used in the UNO R3 and Nano, has 1024 bytes (1KB) of EEPROM. It is organized as a separate data space, and the EEPROM data bytes are addressed linearly between 0 and 1023.

```
Volatile memory is temporary and loses data when power is lost, while non-volatile memory retains data even when the power is off. Both types of memory serve critical roles in computing systems, with volatile memory providing the working space for active processes and non-volatile memory providing long-term data storage and program storage.
```

The following Arduino boards all have some form of EEPROM:

- UNO R4 Minima
- UNO R4 WiFi
- UNO Rev.3
- UNO WiFi Rev.2
- Mega 2560 Rev.3
- Nano Every
- Micro
- Leonardo
- Nano

## Static RAM vs Dynamic RAM

Static RAM (SRAM) and Dynamic RAM (DRAM) are two common types of 
volatile computer memory with key differences:

**Storage Technology:**

- SRAM uses flip-flops to store each bit of data. These flip-flops are 
stable and do not need constant refreshing to retain data.
- DRAM uses a capacitor to store each bit of data. Capacitors discharge 
over time, so DRAM requires periodic refreshing to maintain data 
integrity.

**Speed and Access Time:**

- SRAM is faster and has a shorter access time compared to DRAM. It 
provides rapid access to stored data, making it suitable for cache memory 
and high-speed applications.
- DRAM is slower than SRAM due to the need for refresh cycles. Access times 
are typically longer, making it less suitable for high-speed applications 
but more cost-effective for storing large amounts of data.

**Density and Size:**

- SRAM is less dense and requires more transistors per bit, making it 
physically larger and more expensive. It is used for smaller, 
high-performance caches in CPUs.
- DRAM is more compact and cost-effective for storing larger amounts of 
data. It is commonly used for main memory (RAM) in computers.

**Power Consumption:**

- SRAM consumes more power compared to DRAM because it does not need 
refresh cycles, and its flip-flops draw power constantly.
- DRAM consumes less power but requires occasional refreshing, which 
consumes some energy.

**Data Persistence:**

- SRAM retains data as long as power is supplied. It is volatile and loses 
data when power is cut.
- DRAM is also volatile but can retain data for a short time after power 
loss, provided it is refreshed quickly.

*In summary, SRAM is faster, more power-hungry, and used for high-speed, 
low-density applications like CPU caches. DRAM is slower, more 
power-efficient, and used for higher-density memory in computers, 
but it requires periodic refreshing to maintain data.*

## ATMega328P Memory

Looking at the [ATMega328P data sheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf), you will see that there are three types of memory available for us to use:

- Flash (32KB) - Write/Erase cycles: 10,000
- EEPROM (1KB) - Write/Erase cycles: 100,000
- SRAM (2KB)

On the Arduino UNO/Nano, Flash memory contains the boot code and is used to store your uploaded sketches, EEPROM is available for use from your sketches, and SRAM is used for run-time data (including the stack and the heap).

```
Stack:

The stack is a region of memory used for managing function calls and 
local variables in your Arduino sketch. When a function is called, 
a new stack frame is created to store its local variables, return 
address, and other execution context information. The stack operates 
on a last-in, first-out (LIFO) basis, meaning that the most recently 
called function is at the top of the stack, and when it returns, 
it is removed from the stack, allowing the previous function to resume.

Heap:

The heap is a region of memory used for dynamic memory allocation, 
such as creating objects or variables whose size cannot be determined at 
compile-time. You can allocate memory on the heap using functions like 
malloc() and deallocate it using free(). You are responsible for 
allocating and freeing memory, which can lead to memory leaks or 
fragmentation if not handled properly. The heap size is limited by the 
available RAM on your Arduino board.

It's important to manage both the stack and heap carefully in your Arduino 
sketches to avoid issues like stack overflow or running out of available 
memory. Arduino's limited resources make memory management a crucial 
consideration when developing projects, especially on boards with small 
amounts of RAM. Careful programming practices, like using local variables 
whenever possible, minimizing dynamic memory allocation, and avoiding 
recursive functions, can help you make the most of the available stack 
and heap space.
```

If you want more detail on how memory is handled in the Arduino, have a read of the [Arduino Memory Guide](https://docs.arduino.cc/learn/programming/memory-guide). It is very detailed!

## Memory Usage & Measuring Free SRAM

To see how UNO/Nano sketches are using memory, we can use the `avr-size` utility from the AVR toolchain. To do this, select `Sketch > Export Compiled Binary` in the Arduino IDE to compile the sketch, the `.elf` file will be saved to the `build/<board identifier>` subfolder of the sketch, where `<board identifier>` is the FQBN (fully qualified board name) of the board you are compiling for, with the `:` separators replaced by `.`.

The hardest part of using `avr-size` is finding the path to the executable file. On our Mac using the Arduino IDE 2.2.1, it can be found at: `~/Library/Arduino15/packages/arduino/tools/avr-gcc/7.3.0-atmel3.6.1-arduino7/bin`. To run the command, first `cd` to the directory containing the `.elf` file of your sketch. Then run `avr-size -C -x fileName.ino.elf`. You will probably need to include the full path to the `avr-size` program.

This information is also summarised every time you compile a sketch using the Arduino IDE. The program storage space is Flash memory, and the dynamic memory is referring to SRAM. When we start using the EEPROM, this will also be displayed.

The `avr-size` options are described using `--help` or `-h`:

```html
Usage: avr-size [option(s)] [file(s)]
 Displays the sizes of sections inside binary files
 If no input file(s) are specified, a.out is assumed
 The options are:
  -A|-B|-C  --format={sysv|berkeley|avr}  Output style (default berkeley)
            --mcu=<avrmcu>            MCU name for AVR format only
            --mlist-devices           List all supported MCUs
  -o|-d|-x  --radix={8|10|16}         Display numbers in oct, dec or hex
  -t        --totals                  Display the total sizes (Berkeley)
            --common                  Display total size for *COM* syms
            --target=<bfdname>        Set the binary file format
            @<file>                   Read options from <file>
  -h        --help                    Display this information
  -v        --version                 Display the program's version
```

Another handy utility from the AVR toolchain is `avr-nm`, it will list data symbols in the executable, by decreasing size.

```bash
avr-nm -Crtd - size-sort platformCalibrate.ino.elf | grep -i ' [dbv] '
```

In the output of this command, the first column is the symbol size, the second column is the symbol location (e.g., b = .bss, and d = .data), and the third column is the symbol name.

The SRAM usage reported by `avr-size` is only what is known at compile time. If you are worried about running out of RAM during execution, you can use the following method to get the amount of free memory.

```c++
int freeSRAM ()  {
  extern int __heap_start, *__brkval; 
  int v; 

  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
```

The last memory address pointer used by the heap is stored in `__brkval` . It can be used to calculate how many bytes exist between the end of the heap and the last allocated object on the stack, effectively how much the stack and heap can grow before they collide.

## PROGMEM and the F() Macro

Flash memory normally contains just your program, however if you are short of space (SRAM), it is possible to store constant data there. The cost is some added complexity and speed, flash memory is slower than RAM (and EEPROM is the slowest of the three).

To store a constant in Flash, you use the `PROGMEM` variable modifier. This can be used with any of the datatypes defined in pgmspace.h, as long as they are globally defined, or also defined with the static keyword.

```c++
const PROGMEM uint16_t charSet[] = { 65000, 32796, 16843, 10, 11234};
unsigned int displayInt;

void setup() {
  Serial.begin(9600);
  for (byte k = 0; k < 5; k++) {
    displayInt = pgm_read_word_near(charSet + k);
    Serial.println(displayInt);
  }
}
```

String literals are a special case of read-only data. They are treated like initialized read-only variables (`.data`) and on program execution they are copied from Flash to SRAM. If you have a lot of string literals (e.g., Serial, LCD, Ethernet, and WiFi), you can run out of RAM. Each character in a string literal uses a byte, so they can add up quick. In these circumstances, use the `F()` macro to keep the strings in Flash.

```c++
Serial.print("Display String Literal - stored in Flash and SRAM");
Serial.print(F("Display String Literal - stored in Flash only"));
```

The `F()` macro uses `PROGMEM` but hides the complexity. It tells the compiler to keep the string in Flash and then changes the value type from `char *` (a character pointer) to the `__FlashStringHelper *` type.

In the Print class there are `.print()` and `.println()` methods that accept a `__FlashStringHelper *` instead of the more usual `char *`. The compiler chooses those methods when you pass the `__FlashStringHelper *` created by the `F()` macro. The methods then cast the FlashStringHelper pointer back to a character pointer and fetch each character of the string from Flash. Serial, LCD, Ethernet, and WiFi, are all Arduino objects that inherit behavior from the Print class so their `.print()` and `.println()` methods can also accept the `__FlashStringHelper *` type and fetch the corresponding string from the Flash address space.

To test how effective this technique is, we moved all the calibration instruction string literals in our sketch to Flash using `F()`. Flash usage went up from 4684 to 5030 bytes (an increase of 346 bytes or 1%), and SRAM usage (at compile time) went down from 1333 to 551 (782 bytes, a 39% improvement). Using the `freeSRAM()` method described above, the memory saving at the end of `Setup()` was an even better 846 bytes. This change was definitely worth doing, particularly in an application like ours where speed isn't important.

## Using the EEPROM Library

Our example application will save calibration data for six servo motors. This is well suited to saving in EEPROM as it doesn't often change and reading the values are not time critical. 

For the ATMega328P, the quoted number of Write/Erase cycles is 100,000. You can read from the EEPROM as many times as you like but we want to minimise the number of times we write to it. The 100,000 is not a hard limit. At 100,000 cycles, Atmel guarantee 20 years data retention, beyond this, the data retention duration reduces.

Using the Arduino EEPROM Library is fairly simple and well documented. We will try to provide a few value adds, and explain a specific application where we used the EEPROM in a project.

*Note that the EEPROM will be erased if you upload a sketch via the ICSP header or update the bootloader. EEPROM is not erased when you upload a sketch via Serial.*




