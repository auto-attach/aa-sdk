This File Describes the AA Server Linux Reference Platform Implementation

Background
 Auto-Attach IETF Draft: http://tools.ietf.org/html/draft-unbehagen-lldp-spb-00
 Also See Readme.MD.


Purpose of the AA Server (LRP)
 Those implementing their own Auto-Attach clients using the Auto-Attach SDK 
 can use the AA-Server binary to test the auto-attach behaviour of their client.
 The Server can be used to verify client Avaya Element Types found during the 
 AA discovery phase, and the server can be used to validate ISID-VLAN 
 assignments sent by the client code.

 This Server does not do standard SBPM datapath encapsulation of any traffic.


AA Server Operation
 In general, the server is based on the Vincent-Bernat LLDP implementation 
 and as such will behave as a standard LLDP daemon, transmitting LLDP packets
 every 30 seconds by default across known interfaces and processing incoming 
 packets from those interfaces. This daemon also supports CLI via the lldpcli 
 binary which is built along with the server.  

 The LLDP implementation is enhanced to accept Auto-Attach TLV's and reply with 
 its' own TLVs. For every ISID-VLAN Assignments request received, the server 
 will reply with the same ISID-VLAN Assignment status set to "Active" indicating 
 an automatic acceptance of any ISID-VLAN Assignments. There are no checks 
 performed on the server for duplicates or otherwise.


How to Build The AA Server (Ubuntu linux)
 sudo apt-get update
 sudo apt-get install git libtool build-essential autoconf pkg-config
 git clone https://github.com/auto-attach/aa-sdk
 cd aa-sdk
 git submodule update –init –recursive
 cd lldp
 ./autogen.sh
 ./configure --enable-privsep=NO --enable-aaserver
 make -j 2
 sudo make install


How to bring up and run the AA Server
 1. Once your AA server is built, then do the following as root on your platform:
  #lldpd -d
 2. Then run your AA-Client on the same platform in a Virtual Machine or run it
  on a machine that is directly attached to your ethernet interface. There should
  not be an intervening bridge or switch or router between your client and your 
  server since the device may drop any LLDP packets.
 3. You can use tcpdump or some other tool to sniff incoming/outgoing lldp 
  packets and confirm lldp packets are being transmitted by both client and 
  server. E.g. if using tcpdump on eth0 run it like this:
   "tcpdump -i eth0 -XX not ip6 and not ip and not tcp and not udp and not stp"
 4. On your client, configure it to send an ISID-VLAN Assignment such as 
   10000 20. Check that after the server receives this packet, on the next LLDP 
   packet it sends, this ISID-VLAN Assignment is accepted.

Example VM creation on VirtualBox:
 Download VirtualBox from www.virtualbox.org for your system (Windows, OSX,
 Linux, Solaris). Install it and make sure it's running.

 Download a guest image such as Ubuntu 14.04 64-bit (any linux-like guest image 
 should do, e.g. CentOS).

 In the "Oracle VM VirtualBox Manager" window click "New", or click Machine->New
 from the main menu. 
   Give your new machine a Name, e.g. "myUbuntu14". 
   For Type choose Linux. 
   For Version choose the appropriate version (if you downloaded an Ubuntu 14 
    64-bit image, then choose "Ubuntu (64-bit"). 
   Click Next Then choose minimum 1024MB for memory (more if possible). 
   Click Next then choose "Create a virtual hard drive now" of size 8GB
   Click Create
   Click VDI (VirtualBox Disk Image) for the Hard drive file type, Click Next
   Choose "Dynamically allocated" on the Storage on phyiscal hard drive window
   Click Next, click Create on the FIle location and size window
   Back on the Oracle VM VirtualBox Manager window click on your machine name
   (should be in "powered off" state) and Start it.
   Choose your guest image file to boot from and then proceed with the 
   installation of the distribution.

   Once the installation is complete you may need to reboot the virtual machine.

 After your VM is created, run it and obtain a terminal prompt as the local
 user, and check for network connectivity. Once you can access the internet 
 proceed with the "How to Build The AA Server ..." Section and "How to bring up
 and use the AA Server" Section above.


