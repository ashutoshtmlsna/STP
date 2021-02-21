# Spanning Tree Protocol for Extended LANs

## Algorithm for Distributed Spanning Tree Protocol:

Each bridge/node initially claims that it itself is the root and sends the message accordingly over all its adjcent LANs. Depending on the messages received by the bridge over its LAN port, it updates its parameters (root, distance from root and Root Port). When there is no message to be sent, the program terminates.

## Input and Testing
The extended LAN should be given in a text file and nodes/bridges to send message needs to be passed as command line argument in required order with spaces.

Failure to provide lanfilename and initializing nodes will take the input LANs from the lan.txt contained within the program.
Failure to provide the initializing nodes will open the desired lanfile but will generate message from each nodes respectively 
and finally converges to a final Network layout.

An extended LAN with n bridges has 1,2,3,· · ·, n as the IDs of the bridges. If there are m LANs,they are represented by first m capital letters. 
The .txt file should contain n lines, one for each bridge. Each line must begin with the ID of one bridge, followed by the list of the LANs it is connected to. 
The names of LANs should be separated by exactly one space. For example:

1 D E F G H

2 C E

3 A C

4 H I J

5 A B D

6 G I

7 B F K


## Implementation Details
This program simulating distributed Spanning Tree Protocol for extended LANs is tried to be as generalized as possible. 
The program can be tested by providing the program name, filename for LAN and nodes to send message as command line arguments in the format:

output (lanfilename.txt) nodesSeparatedBy1Space
for example: "output lan.txt 1 2 3 4 5"

The output is displayed for each bridge and each LAN port connected to the bridge and a final LAN and bridge network.
> The LAN port status is updated to its best designated bridge to eliminate the redundancy instead of keeping it open for all cases.

### Data Structure:

Three classes are defined in the program as explained below:

#### bridge: 
The bridge class stores its own ID, the root bridge's id, distance from the root, adjacency list of lans (storing the ids), and a pair of root port lan and the bridge connected to that port with minimum root distance.

#### lan: 
The lan class stores its own ID, adjacency list of bridges (storing the ids) and the Designated Bridge.

#### message: 
The message class stores the root ID, current distance from the root, source bridge, destination bridge ID and the lan through which the message is sent.

Two functions 'sendMsg'of type msgSet and 'configUpdate' of type message is defined. sendMsg sends the message from a bridge over the adjacent LAN ports. configUpdate updates the configuration of the bridge receiving the message depending upon the message (root, distance and id) and returns the updated message, which can be used by the bridge to send over its own port, if required.
