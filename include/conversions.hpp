#ifndef _CONVERSION_BIN_ 
#define _CONVERSION_BIN_

#include <cmath>
#include <list>
#include <deque>
#include <iostream>
#include <string>
#include <string.h>


/*
https://github.com/multiformats

identity    0x00 	8-bit binary (encoder and decoder keeps data unmodified) 	default
base2           0 	binary (01010101) 	candidate
base8 	        7 	octal 	draft
base10 	        9 	decimal 	draft
base16          f 	hexadecimal 	default
base16upper 	F 	hexadecimal 	default
base32hex       v 	rfc4648 case-insensitive - no padding - highest char 	candidate
base32hexupper  V 	rfc4648 case-insensitive - no padding - highest char 	candidate
base32hexpad 	    t 	rfc4648 case-insensitive - with padding 	candidate
base32hexpadupper 	T 	rfc4648 case-insensitive - with padding 	candidate
base32 	        b 	rfc4648 case-insensitive - no padding 	default
base32upper 	B 	rfc4648 case-insensitive - no padding 	default
base32pad 	    c 	rfc4648 case-insensitive - with padding 	candidate
base32padupper 	C 	rfc4648 case-insensitive - with padding 	candidate
base32z 	    h 	z-base-32 (used by Tahoe-LAFS) 	draft
base36 	        k 	base36 [0-9a-z] case-insensitive - no padding 	draft
base36upper 	K 	base36 [0-9a-z] case-insensitive - no padding 	draft
base58btc 	    z 	base58 bitcoin 	default
base58flickr 	Z 	base58 flicker 	candidate
base64 	        m 	rfc4648 no padding 	default
base64pad   	M 	rfc4648 with padding - MIME encoding 	candidate
base64url 	    u 	rfc4648 no padding 	default
base64urlpad 	U 	rfc4648 with padding 	default
proquint 	    p 	PRO-QUINT https://arxiv.org/html/0901.4016 	draft
*/

const char * g_enc_prefix = "079fFvVtTbBcChkKzZmMuUp";
typedef enum {
    base2,
    base8,
    base10,
    base16,
    base16upper,
    base32hex,
    base32hexupper,
    base32hexpad,
    base32hexpadupper,
    base32z,
    base36,
    base36upper,
    base58btc,
    base58flickr,
    base64,
    base64pad,
    base64url,
    base64urlpad,
    proquint
}  base_encoding_symbol;

inline const char enc_predix(base_encoding_symbol bes) {
    return g_enc_prefix[(uint8_t)bes];
}

// BINARY 
// BINARY STRING REPRESENTATION

// TO
inline void convert_to_binary_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    while ( tmp < end ) {
        unsigned char c = *tmp++;
        unsigned char mask = 0x80;
        while ( mask ) {
            unsigned char b = (mask & c);
            *o_tmp++ = b ? '1' : '0';
            mask >>= 1;
        }
    }
    *o_tmp = 0;
}

// FROM
inline void convert_from_binary_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    while ( tmp < end ) {
        unsigned char mask = 0x80;
        unsigned char add_c = 0;
        while ( mask && *tmp ) {
            unsigned char b = *tmp++;
            if ( b == '1' ) {
                add_c |= mask;
            }
            mask >>= 1;
        }
        *o_tmp++ = add_c;
    }
    *o_tmp = 0;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// OCTAL 
// OCTAL STRING REPRESENTATION

// TO
const char *g_octal_alpha = "01234567";
inline void convert_to_octal_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    unsigned int tmp_buff = 0;
    while ( tmp < end ) {
        unsigned char c = *tmp++;
        tmp_buff = c;
        //
        if ( tmp < end ) c = *tmp++; else c = 0;
        tmp_buff = tmp_buff << 8;
        tmp_buff |= ((0x000000FF) & c);
        //
        if ( tmp < end ) c = *tmp++; else c = 0;
        tmp_buff = tmp_buff << 8;
        tmp_buff |= ((0x000000FF) & c);
        //
        unsigned char *o_tmp_next = o_tmp + 8;
        while ( o_tmp_next > o_tmp ) {
            o_tmp_next--;
            unsigned char bits = tmp_buff & 0x7;
            tmp_buff >>= 3;
            *o_tmp_next = g_octal_alpha[bits];
        }
        o_tmp += 8;
    }
    *o_tmp = 0;
}

// FROM
inline void convert_from_octal_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    unsigned char digit_base = (unsigned char)('0');
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    unsigned int tmp_buff = 0;
    while ( tmp < end ) {
        for ( unsigned char i = 0; i < 8; i++ ) {
            unsigned char alph = *tmp++;
            alph = (alph - digit_base);
            tmp_buff <<= 3;
            tmp_buff |= (0x7) & alph;
        }
        *(unsigned int *)(o_tmp) = tmp_buff;
        unsigned char c = o_tmp[0];
        o_tmp[0] = o_tmp[2];
        o_tmp[2] = c;
        o_tmp += 3;
    }
    *o_tmp = 0;
}

// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// HEX 
// HEX STRING REPRESENTATION

// TO
const char *g_hex_alpha = "0123456789abcdef";
const char *g_hex_alpha_UPPER = "0123456789ABCDEF";
inline void convert_to_hex_str(const unsigned char *input,size_t in_len,unsigned char *output,bool lower = true) {
    static const unsigned char mask_upper = 0xF0;
    static const unsigned char mask_lower = 0x0F;
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    const char *alpha = lower ? g_hex_alpha : g_hex_alpha_UPPER;
    while ( tmp < end ) {
        unsigned char c = *tmp++;
        unsigned char b_u = (mask_upper & c) >> 4;
        unsigned char b_l = (mask_lower & c);
        *o_tmp++ = g_hex_alpha[b_u];
        *o_tmp++ = g_hex_alpha[b_l];
    }
    *o_tmp = 0;
}

// FROM
inline void convert_from_hex_str(const unsigned char *input,size_t in_len,unsigned char *output,bool lower = true) {
    static const unsigned char digit_base = (unsigned char)('0');
    unsigned char alph_base = lower ? (unsigned char)('a') : (unsigned char)('A');
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    while ( tmp < end ) {
        unsigned char alph_u = *tmp++;
        unsigned char alph_l = *tmp++;
        alph_u = isdigit((int)(alph_u)) ? (alph_u - digit_base) : ((alph_u - alph_base) + 10);
        alph_l = isdigit((int)(alph_l)) ? (alph_l - digit_base) : ((alph_l - alph_base) + 10);
        unsigned char add_c = (alph_u << 4) | alph_l;
         *o_tmp++ = add_c;
    }
    *o_tmp = 0;
}



// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// BASE32 
// 

// TO

const char pad = '=';

const char *g_base32_alpha = "abcdefghijklmnopqrstuvwxyz234567";
const char *g_base32_alpha_UPPER = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567";
const char *g_hex32_alpha = "0123456789abcdefghijklmnopqrstuv";
const char *g_hex32_alpha_UPPER = "0123456789ABCDEFGHIJKLMNOPQRSTUV";

// Takes string to be encoded as input
// and its length and returns encoded string
inline void convert_to_32_str(const unsigned char *input,size_t in_len,unsigned char *output,bool lower = true,bool hex = false) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    //
    const char *alpha = g_base32_alpha;
    if ( hex ) {
        alpha = lower ? g_hex32_alpha : g_hex32_alpha_UPPER;
    } else {
        alpha = lower ? g_base32_alpha : g_base32_alpha_UPPER;
    }
    unsigned char zero_char = alpha[0];
    //
    bool pad = false ;
    while ( tmp < end ) {
        unsigned char c0 = *tmp++;
        unsigned char c1 = ( tmp < end ) ? *tmp  : 0;
        if ( tmp < end ) tmp++; else pad = true;
        unsigned char c2 = ( tmp < end ) ? *tmp  : 0;
        if ( tmp < end ) tmp++; else pad = true;
        unsigned char c3 = ( tmp < end ) ? *tmp  : 0;
        if ( tmp < end ) tmp++; else pad = true;
        unsigned char c4 = ( tmp < end ) ? *tmp  : 0;
        if ( tmp < end ) tmp++; else pad = true;
        //
        uint64_t buffer = ((uint64_t)c0 << 32) | ((uint64_t)c1 << 24) | ((uint64_t)c2 << 16) | ((uint64_t)c3 << 8) | c4;
        //
        unsigned char o7 = buffer & 0x01F; buffer >>= 5;
        unsigned char o6 = buffer & 0x01F; buffer >>= 5;
        unsigned char o5 = buffer & 0x01F; buffer >>= 5;
        unsigned char o4 = buffer & 0x01F; buffer >>= 5;
        unsigned char o3 = buffer & 0x01F; buffer >>= 5;
        unsigned char o2 = buffer & 0x01F; buffer >>= 5;
        unsigned char o1 = buffer & 0x01F; buffer >>= 5;
        unsigned char o0 = buffer & 0x01F;
        *o_tmp++ = alpha[o0];
        *o_tmp++ = alpha[o1];
        *o_tmp++ = alpha[o2];
        *o_tmp++ = alpha[o3];
        *o_tmp++ = alpha[o4];
        *o_tmp++ = alpha[o5];
        *o_tmp++ = alpha[o6];
        *o_tmp++ = alpha[o7];
    }
    *o_tmp = 0;
    if ( pad ) {
        o_tmp--;
        unsigned char c = *o_tmp;
        while ( ( c == zero_char ) && ( o_tmp > output ) ) {
            *o_tmp = ( c == zero_char ) ? pad : c;
            o_tmp--; c = *o_tmp;
        }
    }
}


static inline unsigned char convert_single_char_32(unsigned char in,bool hex = false) {
    const unsigned char digit_base = hex ? (unsigned char)('0') : (unsigned char)('2');
    const unsigned char alph_base_lower = (unsigned char)('a');
    const unsigned char alph_base_upper = (unsigned char)('A');
    //
    const unsigned char alpha_offset = hex ? 9 : 0;
    const unsigned char digit_offset = hex ? 0 : 26;
    //
    unsigned char o_i = 0;
    if ( in == '=' ) o_i = 0;
    else if ( isdigit(in) ) { o_i = (in - digit_base) + digit_offset; }
    else if ( islower(in) ) { o_i =  (in - alph_base_lower) + alpha_offset; }
    else { o_i = (in - alph_base_upper) + alpha_offset; }
    //
    return o_i;
}

// FROM
inline void convert_from_32_str(const unsigned char *input,size_t in_len,unsigned char *output,bool hex = false) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    while ( tmp < end ) {
        unsigned char o0 = *tmp++;
        unsigned char o1 = *tmp++;
        unsigned char o2 = *tmp++;
        unsigned char o3 = *tmp++;
        unsigned char o4 = *tmp++;
        unsigned char o5 = *tmp++;
        unsigned char o6 = *tmp++;
        unsigned char o7 = *tmp++;
        //
        uint64_t buffer = 0;
        o0 = convert_single_char_32(o0);
        o1 = convert_single_char_32(o1);
        o2 = convert_single_char_32(o2);
        o3 = convert_single_char_32(o3);
        o4 = convert_single_char_32(o4);
        o5 = convert_single_char_32(o5);
        o6 = convert_single_char_32(o6);
        o7 = convert_single_char_32(o7);
        //
        buffer = (o0 << 25) | (o1 << 20) | (o2 << 25) | (o3 << 20) | (o4 << 15) | (o5 << 10) | (o6 << 5) | o7;
        // 
        unsigned char c4 = buffer & 0xFF; buffer >>= 8;
        unsigned char c3 = buffer & 0xFF; buffer >>= 8;
        unsigned char c2 = buffer & 0xFF; buffer >>= 8;
        unsigned char c1 = buffer & 0xFF; buffer >>= 8;
        unsigned char c0 = buffer & 0xFF;
        *o_tmp++ = c0;
        *o_tmp++ = c1;
        *o_tmp++ = c2;
        *o_tmp++ = c3;
        *o_tmp++ = c4;
    }
    *o_tmp = 0;
}




// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// BASE36
//   https://www.geeksforgeeks.org/convert-base-decimal-vice-versa/ 

// TO
const char *g_base36_alpha = "0123456789abcdefghijklmnopqrstuvwxyz";
const char *g_base36_alpha_UPPER = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

typedef unsigned long long int base36_t;

const uint8_t BASE36_LENGTH = 13;
static base36_t base36_powers[BASE36_LENGTH] =
  {
    1ULL,
    36ULL,
    1296ULL,
    46656ULL,
    1679616ULL,
    60466176ULL,
    2176782336ULL,
	78364164096ULL,
	2821109907456ULL,
	101559956668416ULL,
	3656158440062976ULL,
	131621703842267136ULL,
	4738381338321616896ULL
  };


//
// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// 58 - bitcoin
// 58 STRING REPRESENTATION
// This code is derived from the bitcoin C++ implementation: https://github.com/bitcoin/libbase58/blob/master/base58.c

static const int8_t b58digits_map[] = {
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1,
	-1, 0, 1, 2, 3, 4, 5, 6,  7, 8,-1,-1,-1,-1,-1,-1,
	-1, 9,10,11,12,13,14,15, 16,-1,17,18,19,20,21,-1,
	22,23,24,25,26,27,28,29, 30,31,32,-1,-1,-1,-1,-1,
	-1,33,34,35,36,37,38,39, 40,41,42,43,-1,44,45,46,
	47,48,49,50,51,52,53,54, 55,56,57,-1,-1,-1,-1,-1,
};

#define b32_bits (sizeof(uint32_t) * 8)   // 32
static const uint32_t b32_mask = ((((uint64_t)1) << b32_bits) - 1);


static const char g_b58_alpha[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

inline void convert_to_58_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    //
    const unsigned char *tmp = (const unsigned char *)input;
    const unsigned char *end = tmp + in_len;
	unsigned char *o_tmp = output;   // length checks done by parent caller

    // data_sz size of binary input
    size_t z_count = 0;  // count all zero bytes -- skip all zeroes up front
	while ( (tmp < end) && (*tmp == 0) ) { 
        ++z_count; tmp++; *o_tmp++ = '1';
    } 
	//

	size_t size = ((in_len - z_count) * 138 / 100) + 1;  // why 138 ?
	uint8_t buf[size];  // buffer on stack is 1.38*(the # of remaining octets)
	memset(buf, 0, size);
	//
    size_t j = 0;
    size_t top = (size - 1);
    size_t high = top;
	while ( tmp < end )  { // long division
        //
	    unsigned int carry = *tmp++;  // next byte in binary input
        j = top;  // always start at the top of the buffer
        //
		while ( (j > high) || (carry != 0) ) {
            unsigned int b = buf[j];
			carry += (b << 8);
			buf[j] = carry % 58;
			carry /= 58;
			if (!j) {
				break;  // Otherwise j wraps to maxint which is > high 
			}
            j--;
		}
        //
        high = j;   // set a new lower bound on j with carry == 0
	}
    j++;
    //
	for ( ; j < size; ++j ) {
		*o_tmp++ = g_b58_alpha[buf[j]];
    }
	*o_tmp= 0;
	//
}



bool b58tobin(const unsigned char *input, size_t in_len,unsigned char *output, size_t *binszp)
{
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    //
	size_t binsz = *binszp;
	size_t outisz = (binsz + sizeof(uint32_t) - 1) / sizeof(uint32_t);
    //
	uint32_t outi[outisz];
	uint8_t bytesleft = binsz % sizeof(uint32_t);
	uint32_t zeromask = bytesleft ? (b32_mask << (bytesleft * 8)) : 0;
	//
    memset(outi,0,sizeof(uint32_t)*outisz);
	// Leading zeros, just count
    while ( ( tmp < end ) && (*tmp == '1') ) tmp++;
    while ( tmp < end ) {
        unsigned char cc = *tmp++;
		if ( cc & 0x80 ) return false; // High-bit set on invalid digit
        //
        int8_t ii = b58digits_map[cc];
		if ( ii == -1) return false; // Invalid base58 digit
        //
		uint32_t c = (unsigned)ii;
		for ( size_t j = outisz; j--; ) {
			uint64_t t = ((uint64_t)outi[j]) * 58 + c;
			c = t >> b32_bits;
			outi[j] = t & b32_mask;
		}
		if (c) return false;                    // Output number too big (carry to the next int32)
		if (outi[0] & zeromask) return false;   // Output number too big (last int32 filled too far)
	}
	
    {
        size_t j = 0;
        if ( bytesleft ) {
            for ( size_t i = bytesleft; i > 0; --i ) {
                *(o_tmp++) = (outi[0] >> (8 * (i - 1))) & 0xff;
            }
            ++j;
        }
        
        for (; j < outisz; ++j) {
            for ( size_t i = sizeof(uint32_t); i > 0; --i ) {
                *(o_tmp++) = (outi[j] >> (8 * (i - 1))) & 0xff;
            }
        }
    }

	return true;
}

// FROM
inline bool convert_from_58_str(const unsigned char *input,size_t in_len,unsigned char *output,size_t out_size) {
    //const unsigned char *tmp = input;
    //const unsigned char *end = tmp + in_len;
    //unsigned char *o_tmp = output;   // length checks done by parent caller
    size_t binszp = out_size;
    if ( b58tobin(input, in_len,output,&binszp) ) {
        unsigned char *out = output;
        unsigned char *end = out + out_size;
        while ( (*out == 0) && ( out < end ) ) {
            out++;
        }
        unsigned char *tmp = output;
        while ( out < end ) *tmp++ = *out++;
        return true;
    }
    return false;
}


// ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ---- ----
// base64 
// 64 STRING REPRESENTATION

// TO
const char *g_64_alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
const char *g_64_alphabet_url = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

// Takes string to be encoded as input
// and its length and returns encoded string
inline void convert_to_64_str(const unsigned char *input,size_t in_len,unsigned char *output,bool url_safe = false)
{
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    const char *alpha = url_safe ? g_64_alphabet : g_64_alphabet_url;
    unsigned char zero_char = alpha[0];
    //
    bool pad = false ;
    while ( tmp < end ) {
        unsigned char c0 = *tmp++;
        unsigned char c1 = ( tmp < end ) ? *tmp : 0;
        if ( tmp < end ) tmp++; else pad = true;
        unsigned char c2 = ( tmp < end ) ? *tmp : 0;
        if ( tmp < end ) tmp++; else pad = true;
        //
        unsigned int buffer = (c0 << 16) | (c1 << 8) | c2;
        //
        unsigned char o3 = buffer & 0x03F; buffer >>= 6;
        unsigned char o2 = buffer & 0x03F; buffer >>= 6;
        unsigned char o1 = buffer & 0x03F; buffer >>= 6;
        unsigned char o0 = buffer & 0x03F;
        *o_tmp++ = alpha[o0];
        *o_tmp++ = alpha[o1];
        *o_tmp++ = alpha[o2];
        *o_tmp++ = alpha[o3];
    }
    *o_tmp = 0;
    if ( pad ) {
        o_tmp--;
        unsigned char c = *o_tmp;
        while ( ( c == zero_char ) && ( o_tmp > output ) ) {
            *o_tmp = ( c == zero_char ) ? '=' : c;
            o_tmp--; c = *o_tmp;
        }
    }
}


static inline unsigned char convert_single_char_64(unsigned char in) {
    static unsigned char digit_base = (unsigned char)('0');
    static unsigned char alph_base_lower = (unsigned char)('a');
    static unsigned char alph_base_upper = (unsigned char)('A');
    const unsigned char plus = 62;
    const unsigned char slash = 63;
    const unsigned char lower_offset = 26;
    const unsigned char digit_offset = 52;
    //
    unsigned char o_i = 0;
    if ( in == '=' ) o_i = 0;
    else if ( in == '+' || in == '-' ) o_i = plus;
    else if ( in == '/' || in == '_') o_i = slash;
    else if ( isdigit(in) ) {   o_i = (in - digit_base) + digit_offset; }
    else if ( islower(in) ) { o_i =  (in - alph_base_lower) + lower_offset; }
    else { o_i = (in - alph_base_upper); }
    //
    return o_i;
}

// FROM
inline void convert_from_64_str(const unsigned char *input,size_t in_len,unsigned char *output) {
    const unsigned char *tmp = input;
    const unsigned char *end = tmp + in_len;
    unsigned char *o_tmp = output;   // length checks done by parent caller
    while ( tmp < end ) {
        unsigned char o0 = *tmp++;
        unsigned char o1 = *tmp++;
        unsigned char o2 = *tmp++;
        unsigned char o3 = *tmp++;
        //
        unsigned int buffer = 0;
        o0 = convert_single_char_64(o0);
        o1 = convert_single_char_64(o1);
        o2 = convert_single_char_64(o2);
        o3 = convert_single_char_64(o3);
        //
        buffer = (o0 << 18) | (o1 << 12) | (o2 << 6) | o3;
        //
        unsigned char c2 = buffer & 0xFF; buffer >>= 8;
        unsigned char c1 = buffer & 0xFF; buffer >>= 8;
        unsigned char c0 = buffer & 0xFF;
        *o_tmp++ = c0;
        *o_tmp++ = c1;
        *o_tmp++ = c2;
    }
    *o_tmp = 0;
}

#endif
