import pyshark

pcap_file = 'try.pcap'

# Open the PCAP file for reading
cap = pyshark.FileCapture(pcap_file)

# Create a list to store packets
packets = []

t = 0

# Iterate through each packet in the capture
for packet in cap:
    if 'IP' in packet and 'UDP' in packet:
        ip_info = packet.ip
        udp_info = packet.udp
        udp_payload = packet.udp.payload
        packets.append((ip_info, udp_info, udp_payload))

cap.close()

# Sort packets by UDP payload
sorted_packets = sorted(packets, key=lambda x: x[2])

flag = False

key = []

packet_no = 0
lost = 0

# Iterate through sorted packets and print IP and UDP information
for ip_info, udp_info, udp_payload in sorted_packets:
    udp_payload = udp_info.payload
    hex_values  = udp_payload.split(':')
    ascii_characters = ''.join([chr(int(hex_value, 16)) for hex_value in hex_values])

    if flag:
        tmp = int(ip_info.hdr_len) - 20

    if "END FLAG" in ascii_characters:
        flag = False
        packet_no = int(ascii_characters[4:9])

    if flag == True:
        while int(ascii_characters[4:9]) > packet_no + 1:
            # print("lost packet :",packet_no + 1)
            # print()
            packet_no = packet_no + 1
            lost = lost + 1
        # print(ascii_characters)
        tmp = tmp + len(ascii_characters)
        key.append(tmp)
        packet_no = int(ascii_characters[4:9])
            
    if "BEGIN FLAG" in ascii_characters:
        flag = True
        packet_no = int(ascii_characters[4:9])
    
    # print(ascii_characters)


print("< Lost flag packets :", lost, ' >\n')

flag = "verfy "

print("verfy ", end = "")
for temp in key:
    print(chr(temp), end = "")
    flag += chr(temp)
print()
print()

# print(flag)

path = 'flag.txt'
f = open(path, 'w')
f.write(flag)
f.close()