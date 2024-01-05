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

The message is stored in the memory as:
```
0x010200
```
That's only 3 bytes instead of 45.

The log will be sent as:
```
0104d20a
```
This results in 8 bytes instead of 46.
> **_NOTE:_**  This is the so-called text mode (where bytes are sent in text hex format). The Binary mode feature is currently in preparation and will reduce the message sent through the interface by half.

Additionally, the arguments are always sent in their binary form. They do **not need to be converted to string**, thus the overhead is minimal.

# How to use ShrinkLog?
1. **Install Docker**

   To make ShrinkLog as portable and simple to install as possible both build and execution happens inside a docker container. However you do not need to know much about Docker. All steps are covered by pre-prepared scripts. Your local filesystem will mounted inside the container so no files need to be copied.

   Currently ShrinkLog was tested on Windows (WSL2) and Linux (Ubuntu).

   Install Docker on your system using official instruction https://docs.docker.com/engine/install/.
   > **NOTE**
   >* Sometimes the docker deamon need to be run manually or configured to start automatically.
   >   * On Windows always start Docker Desktop before using ShrinkLog.
   >   * On Linux make sure the demon starts with the OS https://docs.docker.com/config/daemon/start/.

1. **(Windows only) Execute all of the next steps inside WSL2 Linux terminal**

   All required steps to install a WSL2 Linux are explained here: https://learn.microsoft.com/en-us/windows/wsl/install \
   Recomended distribution is Ubuntu or Debian. \
   Your windows filesystem will be mounted to ```/mnt/<your drive letter>``` by default. For example ```/mnt/c```.
1. **Clone ShrinkLog to selected directory**
```
git clone https://github.com/RafalBanaszak/ShrinkLog.git
cd ShrinkLog
```
1. **Run the installation script**

   Make sure you are in ShrinkLog directory (!important) and execute the installation script:
   ```
   ./Scripts/Install.sh
   ```
   The script will:
   * build a docker image
   * build the ShrinkLog using docker image
   * build and execute the ExampleProject using docker image

   You can check results of the ExampleProject.
   1. Open and review the ExampleProject/main.c file. The first section is an internal test - ignore it. The ShrinkLog demo is located in the second part.
   1. Open the logRaw.txt file and examine its contents. This file contains encoded logs.
   1. Open the logDecoded.txt file and examine its contents. The demo logs can be found at the bottom of this file.

# How ShrinkLog works?


