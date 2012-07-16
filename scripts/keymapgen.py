#!/usr/bin/python

SHIFT=0x100

keymap = {
    ' ':3,
    '0':51,
    ')':51 | SHIFT,
    '1':39,
    '2':11,
    '"':11 | SHIFT,
    '3':10,
    '#':10 | SHIFT,
    '4':9,
    '$':9 | SHIFT,
    '5':8,
    '%':8 | SHIFT,
    '6':43,
    '_':43 | SHIFT,
    '7':45,
    '&':45 | SHIFT,
    '8':47,
    "'":47 | SHIFT,
    '9':49,
    '(':49 | SHIFT,

    'a':23,
    'b':30,
    'c':26,
    'd':27,
    'e':4,
    'f':29,
    'g':31,
    'h':33,
    'i':46,
    'j':35,
    'k':37,
    'l':39,
    'm':34,
    'n':32,
    'o':52,
    'p':50,
    'q':6,
    'r':5,
    's':25,
    't':44,
    'u':48,
    'v':28,
    'w':7,
    'x':24,
    'y':42,
    'z':22,

    ',':36,
    '?':36 | SHIFT,
    '.':38,
    '/':40,
    '|':40 | SHIFT,
    ';':41,
    ':':41 | SHIFT,
    '-':53,
    '*':53 | SHIFT

}

table=[0]*128
for key in keymap.keys():
    table[ord(key)] = keymap[key]
for i in range(26):
    table[i+ord('A')]=(table[i+ord('a')]|SHIFT)

print "unsigned int keyTable[128] = {"
for i in range(8):
    print ",".join(map(str,table[i*16:(i*16)+16]))
print "};"
