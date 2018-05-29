# // author: Tomasz Nowak

from scapy.all import *

def packet_callback(packet):
    if packet[UDP].payload:
        payload = str(packet[UDP].payload)
        print("\n{} -> {}:{}: {}".format(packet.src, packet.dst, packet.dport, payload))

sniff(filter="udp", iface="lo", prn=packet_callback, store=0) # src/dest host can be added to filter, iface (interface) can be changed
