## Example

This is an esample for how you could get a reply back from the server using the bash script to receive the data and then call the slightly modefied send.c script which
now allows to pass parameters from the command line. Like `sudo ./send hi <server_ip>`.

# Attention

The script is not finished. Buffer overflows are possible iam working with null bites and the script does not reply the actual result of the execution but only the send text.

## for now

## Usage

Just open up to command lines on your pc. On one open the server.sh file. In the other the imp.c file. On the server place the send.c file and the server.sh file from the server
 folder. Now the server should send you an echo.

 ## Do not run just now I will improve it!

If you won't just use sudo ./server.sh (from the server folder). with th send.c program in the samen folder. I also wont to add that it realy echos the output of the pogramm.
