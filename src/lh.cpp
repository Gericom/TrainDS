#include <nitro.h>
#include "lh.h"

/*---------------------------------------------------------------------------*
Name:         CXGetUncompressedSize

Description:  Gets the data size after decompression.
This function can be used for data in all compression formats handled by CX.

Arguments:    srcp :  Starting address of the compressed data

Returns:      Data size after decompression
*---------------------------------------------------------------------------*/
u32 CXGetUncompressedSize(const void *srcp)
{
	u32 size = *(u32*)srcp >> 8;
	if (size == 0)
	{
		size = *((u32*)srcp + 1);
	}
	return size;
}

typedef struct
{
	const u8* srcp;
	u32   cnt;
	u32   stream;
	u32   stream_len;
}
BitReader;


static inline s32
BitReader_Read(BitReader* context, u8 bits)
{
	s32 value;

	while (context->stream_len < bits)
	{
		if (context->cnt == 0)
		{
			return -1;
		}
		context->stream <<= 8;
		context->stream += *context->srcp;
		context->srcp++;
		context->cnt--;
		context->stream_len += 8;
	}

	value = (s32)((context->stream >> (context->stream_len - bits)) & ((1 << bits) - 1));
	context->stream_len -= bits;
	return value;
}

static inline s64
BitReader_ReadEx(BitReader* context, u8 bits)
{
	s64 value;
	u8  stock = 0;

	while (context->stream_len < bits)
	{
		if (context->cnt == 0)
		{
			return -1;
		}
		if (context->stream_len > 24)
		{
			stock = (u8)(context->stream >> 24);
		}
		context->stream <<= 8;
		context->stream += *context->srcp;
		context->srcp++;
		context->cnt--;
		context->stream_len += 8;
	}
	value = context->stream;
	value |= (s64)stock << 32;
	value = (s64)((value >> (context->stream_len - bits)) & ((1 << bits) - 1));
	context->stream_len -= bits;

	return value;
}

#define ENC_OFFSET_WIDTH

/*---------------------------------------------------------------------------*
Name:         CXInitUncompContextLH

Description:


Arguments:    context
dest

Returns:      None.
*---------------------------------------------------------------------------*/
void
CXInitUncompContextLH(CXUncompContextLH * context, void* dest)
{
	context->destp = (u8*)dest;
	context->destCount = -1;
	context->nodep = context->huffTable9 + 1;
	context->tableSize9 = -1;
	context->tableSize12 = -1;
	context->headerSize = 8;
	context->length = 0;
	context->stream = 0;
	context->stream_len = 0;
	context->offset_bits = -1;
	context->forceDestCount = 0;
}

//extern BOOL CXiLHVerifyTable(const void* pTable, u8 bit);

/*---------------------------------------------------------------------------*
Name:         CXReadUncompLH

Description:  This function performs streaming decompression of LZ-Huffman (combined) compressed data.

Arguments:    *context: Pointer to the decompressed context
data:     Data pointer
len:      Data size

Returns:      Size of remaining uncompressed data
Returns a negative error code if failed.
*---------------------------------------------------------------------------*/
s32
CXReadUncompLH(CXUncompContextLH *context, const void* data, u32 len)
{
#define LENGTH_BITS         9
#if defined(ENC_OFFSET_WIDTH)
#define OFFSET_BITS     5
#define OFFSET_MASK     0x07
#define LEAF_FLAG       0x10
#else
#define OFFSET_BITS     12
#define OFFSET_MASK  0x3FF
#define LEAF_FLAG    0x800
#endif

	const u8* srcp = (const u8*)data;
	BitReader stream;
	s32  val;
	u16* nodep;
	u16  length;

	stream.srcp = srcp;
	stream.cnt = len;
	stream.stream = context->stream;
	stream.stream_len = context->stream_len;

	// Header parsing
	while (context->headerSize > 0)
	{
		s64 val32 = BitReader_ReadEx(&stream, 32);
		if (val32 < 0)
		{
			goto out;
		}
		context->headerSize -= 4;
		if (context->headerSize == 4)
		{
			u32 headerVal = MI_SwapEndian32((u32)val32);
			if ((headerVal & CX_COMPRESSION_TYPE_MASK) != CX_COMPRESSION_LH)
			{
				return CX_ERR_UNSUPPORTED;
			}
			context->destCount = (s32)(headerVal >> 8);

			if (context->destCount == 0)
			{
				context->headerSize = 4;
				context->destCount = -1;
			}
			else
			{
				context->headerSize = 0;
			}
		}
		else // if ( context->headerSize == 0 )
		{
			context->destCount = (s32)MI_SwapEndian32((u32)val32);
		}
		if (context->headerSize == 0)
		{
			if ((context->forceDestCount > 0) && (context->forceDestCount < context->destCount))
			{
				context->destCount = context->forceDestCount;
			}
		}
	}

	// load the Huffman table
	{
		if (context->tableSize9 < 0)
		{
			if ((val = BitReader_Read(&stream, 16)) < 0)
			{
				goto out;
			}
			context->tableSize9 = (MI_SwapEndian16((u16)val) + 1) * 4 * 8; // shown with the bit count
			context->tableIdx = 1;
			context->tableSize9 -= 16;
		}
		while (context->tableSize9 >= LENGTH_BITS)
		{
			if ((val = BitReader_Read(&stream, LENGTH_BITS)) < 0)
			{
				goto out;
			}
			context->huffTable9[context->tableIdx++] = (u16)val;
			context->tableSize9 -= LENGTH_BITS;
		}
		while (context->tableSize9 > 0)
		{
			if ((val = BitReader_Read(&stream, (u8)context->tableSize9)) < 0)
			{
				goto out;
			}
			context->tableSize9 = 0;
		}
		// verify the table
		//if (!CXiLHVerifyTable(context->huffTable9, LENGTH_BITS))
		//{
		//	return CX_ERR_ILLEGAL_TABLE;
		//}
	}
	{
		if (context->tableSize12 < 0)
		{
			if ((val = BitReader_Read(&stream, (OFFSET_BITS > 8) ? 16 : 8)) < 0)
			{
				goto out;
			}
#if ( OFFSET_BITS > 8 )
			context->tableSize12 = (CXiConvertEndian16_((u16)val) + 1) * 4 * 8;
#else // ( OFFSET_BITS <= 8 )
			context->tableSize12 = ((u16)val + 1) * 4 * 8;
#endif
			context->tableIdx = 1;
			context->tableSize12 -= (OFFSET_BITS > 8) ? 16 : 8;
		}

		while (context->tableSize12 >= OFFSET_BITS)
		{
			if ((val = BitReader_Read(&stream, OFFSET_BITS)) < 0)
			{
				goto out;
			}
			context->huffTable12[context->tableIdx++] = (u16)val;
			context->tableSize12 -= OFFSET_BITS;
		}
		while (context->tableSize12 > 0)
		{
			if ((val = BitReader_Read(&stream, (u8)context->tableSize12)) < 0)
			{
				goto out;
			}
			context->tableSize12 = 0;
		}
		// verify the table
		//if (!CXiLHVerifyTable(context->huffTable12, OFFSET_BITS))
		//{
		//	return CX_ERR_ILLEGAL_TABLE;
		//}
	}

	nodep = context->nodep;
	length = context->length;

	// Data conversion
	while (context->destCount > 0)
	{
		// get length data
		if (length == 0)
		{
			do
			{
				u8  bit;
				u32 offset;
				if ((val = BitReader_Read(&stream, 1)) < 0)
				{
					context->nodep = nodep;
					context->length = length;
					goto out;
				}
				bit = (u8)(val & 1);
				offset = (((*nodep & 0x7F) + 1U) << 1) + bit;

				if (*nodep & (0x100 >> bit))
				{
					nodep = (u16*)((u32)nodep & ~0x3);
					length = *(nodep + offset);
					nodep = context->huffTable12 + 1;
					break;
				}
				else
				{
					nodep = (u16*)((u32)nodep & ~0x3);
					nodep += offset;
				}
			} while (1);
		}

		if (length < 0x100)
			// uncompressed data
		{
			*context->destp++ = (u8)length;
			context->destCount--;
			nodep = context->huffTable9 + 1;
			length = 0;
		}
		else
			// compressed data
		{
			u16 lzOffset;
			u16 lzLength = (u16)((length & 0xFF) + 3);

#if defined(ENC_OFFSET_WIDTH)
			if (context->offset_bits < 0)
			{
#endif
				do
				{
					u8 bit;
					u32 offset;

					if ((val = BitReader_Read(&stream, 1)) < 0)
					{
						context->nodep = nodep;
						context->length = length;
						goto out;
					}
					bit = (u8)(val & 1);
					offset = (((*nodep & OFFSET_MASK) + 1U) << 1) + bit;

					if (*nodep & (LEAF_FLAG >> bit))
					{
						nodep = (u16*)((u32)nodep & ~0x3);
#if defined(ENC_OFFSET_WIDTH)
						context->offset_bits = (s8)(*(nodep + offset));
#else
						lzOffset = (u16)(*(nodep + offset) + 1);
#endif
						break;
					}
					else
					{
						nodep = (u16*)((u32)nodep & ~0x3);
						nodep += offset;
					}
				} while (1);
#if defined(ENC_OFFSET_WIDTH)
			}
#endif

#if defined(ENC_OFFSET_WIDTH)
			if (context->offset_bits <= 1)
			{
				val = context->offset_bits;
			}
			else if ((val = BitReader_Read(&stream, (u8)(context->offset_bits - 1))) < 0)
			{
				context->nodep = nodep;
				context->length = length;
				goto out;
			}
			if (context->offset_bits >= 2)
			{
				val |= (1 << (context->offset_bits - 1));
			}

			context->offset_bits = -1;
			lzOffset = (u16)(val + 1);
#endif

			if (context->destCount < lzLength)
				// A buffer overrun handler for when invalid data is decompressed.
			{
				if (context->forceDestCount == 0)
				{
					return CX_ERR_DEST_OVERRUN;
				}
				lzLength = (u16)context->destCount;
			}

			context->destCount -= lzLength;
			while (lzLength--)
			{
				*context->destp = *(context->destp - lzOffset);
				++context->destp;
			}
			length = 0;
			nodep = context->huffTable9 + 1;
		}
	}

out:
	context->stream = stream.stream;
	context->stream_len = stream.stream_len;


	// After decompression, remaining source data will be treated as an error
	if ((context->destCount == 0) &&
		(context->forceDestCount == 0) &&
		(len > 32))
	{
		return 0;// CX_ERR_SRC_REMAINDER;
	}

	return context->destCount;

#undef OFFSET_BITS
#undef OFFSET_MASK
#undef LEAF_FLAG
}