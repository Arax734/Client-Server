# Client-Server
Client-Server application in C. 
The application creates an NxN array, where N is the number of servers. We provide the values of the elements to the array. Each server in a child process calculates the sum of the values of the elements of a particular row and then passes them to the main process, the client. The client, after receiving all the sums, calculates the total sum.

Program uses lncurses library which you have to install before using:
sudo apt-get install libncurses5-dev libncursesw5-dev

To compile program you have to enter the command:
gcc -o client-server client-server.c -lncurses
