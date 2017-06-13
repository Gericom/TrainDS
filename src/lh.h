#pragma once

#define CX_ERR_SUCCESS              0
#define CX_ERR_UNSUPPORTED          -1
#define CX_ERR_SRC_SHORTAGE         -2
#define CX_ERR_SRC_REMAINDER        -3
#define CX_ERR_DEST_OVERRUN         -4
#define CX_ERR_ILLEGAL_TABLE        -5

//---- Compression type
typedef enum
{
	CX_COMPRESSION_LZ = 0x10,     // LZ77
	CX_COMPRESSION_HUFFMAN = 0x20,     // Huffman
	CX_COMPRESSION_RL = 0x30,     // Run Length
	CX_COMPRESSION_LH = 0x40,     // LH(LZ77+Huffman)
	CX_COMPRESSION_LRC = 0x50,     // LRC(LZ77+RangeCoder)
	CX_COMPRESSION_DIFF = 0x80,     // Differential filter

	CX_COMPRESSION_TYPE_MASK = 0xF0,
	CX_COMPRESSION_TYPE_EX_MASK = 0xFF
}
CXCompressionType;

//----------------------------------------------------------------
// Compressed data header
//
typedef struct
{
	u8  compType;   // Compression type
	u8  compParam;  // Compression parameter
	u8  padding_[2];
	u32 destSize;   // Expanded size
}
CXCompressionHeader;

typedef struct
{
	u8   *destp;                         // Write-destination pointer:                     4B
	s32  destCount;                      // Remaining size to write:                     4B
	s32  forceDestCount;                 // Forcibly set the decompression size             4B
	u16  huffTable9[1 << (9 + 1)];    // Huffman encoding table: 2048B
	u16  huffTable12[1 << (5 + 1)];    // Huffman encoding table: 128B
	u16  *nodep;                         // Node during a Huffman table search: 4B
	s32  tableSize9;                     // Table size during a load: 4B
	s32  tableSize12;                    // Table size during a load: 4B
	u32  tableIdx;                       // Index for the table load position: 4B
	u32  stream;                         // Bit stream for loading: 4B
	u32  stream_len;                     // Number of valid stream bits for loading: 4B
	u16  length;                         // Read length for LZ compression: 2B
	s8   offset_bits;                    // Bit length for offset information: 1B
	u8   headerSize;                     // Size of header being read:             1B
}                                        //                             Total is 2216B
CXUncompContextLH;

/*---------------------------------------------------------------------------*
Name:         CXGetUncompressedSize

Description:  Gets the data size after decompression.
This function can be used for data in all compression formats handled by CX.

Arguments:    srcp :  Pointer to the first four bytes of data in the compressed data

Returns:      Data size after decompression
*---------------------------------------------------------------------------*/
u32 CXGetUncompressedSize(const void *srcp);

/*---------------------------------------------------------------------------*
Name:         CXInitUncompContextLH

Description:  Initializes the streaming decompression context for LZ-Huffman (combined) compressed data.

Arguments:    context: Pointer to the decompressed context
dest        Destination address for uncompressed data

Returns:      None.
*---------------------------------------------------------------------------*/
void CXInitUncompContextLH(CXUncompContextLH * context, void* dest);

/*---------------------------------------------------------------------------*
Name:         CXReadUncompLH

Description:  This function performs streaming decompression of LZ-Huffman (combined) compressed data.

Arguments:    *context: Pointer to the decompressed context
data:     Data pointer
len     Data size

Returns:      Size of remaining uncompressed data
*---------------------------------------------------------------------------*/
s32 CXReadUncompLH(CXUncompContextLH *context, const void* data, u32 len);