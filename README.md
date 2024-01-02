# What is ShrinkLog?

ShrinkLog is a set of tools designed to improve logging on small targets like microcontrollers. Let's consider the following example:
```
printf("This is an example log printing a number %hd\n", 1234);
```
This kind of log requires to put a message in memory:
```
This is an example log printing a number %hd\n
```
Finally, the message must be sent through a serial port or another communication interface:
```
This is an example log printing a number 1234
```
Using such logs brings following issues:
- Storage: The example message string will consume 45 bytes of non-volatile memory. So, if you have 100 similar logs, you'd need over 4kB of space.
- CPU cycles: The printf function needs to transform the number 1234 into a string, involving operations like modulo and division. On simple architectures, this process requires many CPU clock cycles.
- Interface time: Finally, the code has to send 46 bytes through the interface. If you're using a 112500-baud UART, it would take 427 microseconds. This might not be a problem for one log, but if you have 1000 logs, it would take almost half a second to send them all.

When using ShinkLog, the log function looks like in the example below:
```
LOG(SLOG_jI3O7Y4, "This is an example log printing a number %hd\n", 1234);
```
It resembles the printf function and supports almost all printf features, including size modifiers, paddings, and precision. The *SLOG_jI3O7Y4* serves as a tag, and users don't need to modify or manage it.

The message is stored in memory as:
```
0x0102
```
That's only 2 bytes instead of 45.

The log will be sent as:
```
0104d20a
```
This results in 8 bytes instead of 46.
> **_NOTE:_**  This is the so-called text mode (where bytes are sent in text hex format). The Binary mode feature is currently in preparation and will reduce the message sent through the interface by half.

Additionally, the arguments are sent in binary form. They do **not need to be converted**, thus the overhead is minimal.

# How to use ShrinkLog?
# How ShrinkLog works?


