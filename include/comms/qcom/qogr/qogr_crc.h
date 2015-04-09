// CRC.H

#ifndef CRC_H
#define        CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef        UCHAR
#define        UCHAR        unsigned char
#endif
#ifndef        USHORT
#define        USHORT        unsigned short int
#endif
#ifndef        UINT
#define        UINT        unsigned short int
#endif
#ifndef        ULONG
#define        ULONG        unsigned long int
#endif

#define        BYTE        UCHAR
#define        WORD        USHORT
#define        DWORD        ULONG
#define        BOOLEAN        UCHAR
#define        BOOL        UCHAR

extern WORD crctbl[256];

#define CRCccittMACRO(b, crc) (crctbl[(crc ^ b) & 0xff] ^ (crc >> 8))
// Macro Arguements
//        b is an unsigned char or byte
//        crc is an unsigned short
// Return Value
//        16 bit CCITT CRC (unsigned short)

WORD        CalcCRC(WORD seed, BYTE *p, WORD size);        // General use

BOOLEAN        QSIM_CheckCRC(BYTE *p, WORD size);        // QCOM Protocol
BOOLEAN        CheckCRC(BYTE *p, WORD size);                // IGT Protocol

//         CheckCRC returns true (non-zero) for success else FALSE (0)

void        PutCRC_LSBfirst(BYTE *p, WORD size);        // QCOM Protocol
void        PutCRC_MSBfirst(BYTE *p, WORD size);        // IGT Protocol

//         PutCRC calculates the CRC over size bytes starting at p
//        and appends the result to p[size] and p[size+1]

#ifdef __cplusplus
}
#endif

#endif
