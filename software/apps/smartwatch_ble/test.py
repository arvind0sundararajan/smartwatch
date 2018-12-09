# f = [0x20, 0x73, 0x03, 0xc7, 0x3d, 0x90, 0xfa, 0x9c, 0x40, 0x45, 0xf6, 0xea, 0x8e, 0x2e, 0x85, 0xc4]
# a = list(map(lambda x: '{:02X}'.format(x), f))

# a = a[::-1]

# first = a[0:4]
# first = ''.join(first)
# second = a[4:6]
# second = ''.join(second)
# third = a[7:9]
# third = ''.join(third)
# fourth = a[9:11]
# fourth = ''.join(fourth)
# fifth = a[11:]
# fifth = ''.join(fifth)

# total = '-'.join([first, second, third, fourth, fifth])
# print(total)

def get_uuid_formated(s):
    s = ''.join(filter(lambda x: x != '-', s))
    f = []
    for i in range(16):
        f += ['0x'+s[2*i:2*i+2]]
    f = sorted(f, reverse=True)

    uuid = '{ ' + ', '.join(f) + ' }'
    return uuid

s = "00B05AC1-C404-42CA-A435-730FCFE28635"
print(get_uuid_formated(s))
s = "FD873405-FEE4-4DD3-9629-5905E748B69E"
print(get_uuid_formated(s))
s = "DF431879-1A75-4628-8DAE-33A05EB8B8EB"
print(get_uuid_formated(s))