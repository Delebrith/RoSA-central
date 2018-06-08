# // author: Tomasz Nowak

from scapy.all import *
import datetime

def packet_callback(packet):
    if packet[UDP].payload:
        payload = str(packet[UDP].payload)
        time = datetime.datetime.fromtimestamp(int(packet.time)).strftime('%d-%m-%Y %H:%M:%S');
        print("\n[{}] {}:{} -> {}:{}: {}".format(time, packet[IP].src, packet[UDP].sport, packet[IP].dst, packet[UDP].dport, str(payload)))

sniff(filter="udp and portrange 7000-9000", prn=packet_callback, store=0) # filter can be changed

