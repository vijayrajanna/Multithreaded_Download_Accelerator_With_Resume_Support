# Multithreaded Download Accelerator With Resume Support

##Issue with existing file transfer protocol .
When a computer tries to transfer files over the network with another computer, typically it establishes a single connection with the server and transfers the files sequentially over this connection. This slows down the speed of data transfer and does not utilize the available bandwidth effectively. 

##Solution
Using muti-threading it is possible for several threads to connect to the server independently over different sockets and transfer either different files simultaneously or different portions of a single file simultaneously.

Also when data transfer with a system is terminated abruptly, generally the entire download operation needs to be re-instantiated from scratch again. This can be eliminated if the data transfer package can maintain the status of download for every file downloaded with resume support at the server side, thereby ensuring that the download can resume from the point of disconnection rather than all over from the beginning.

##Main objectives of Multithreaded Downloaded Accelerator with Resume Support
* To develop a server that can support file transfer transactions, with resume support
* To develop a client that can provide an attractive graphical user interface to the user and help the user connect to specific systems and transfer files. Furthermore, the client must be able to maintain the status of all downloads.
* To develop a protocol that ensures that the client can communicate with the server.
* To incorporate multi-threading in order to improve bandwidth utilization, with proper communication amongst threads so that there is no synchronization problems or race conditions.
* To introduce resume support by incorporating CRC checks, so that incomplete downloads can be resumed from the point where it was left off.
