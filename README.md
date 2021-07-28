# CODINGS
base *<num>* encoding including binary, b58, b64, hex, octal, in C++


## Current State

All methods are in src/conversions.hpp.

These are inlined methods that assume memory management is handle by callers. 

These provide basic base encodings for use in any number of application requiring the representation of binary buffers as text.

The Bitcoin b58 implementation has been included. The implementation here is geared towards understanding how it operates in memory. 
But, it is basically the same as the one in the Bitcoin repository.

## List of calls

* **Binary**
```
inline void convert_to_binary_str(const unsigned char *input,size_t in_len,unsigned char *output)
inline void convert_from_binary_str(const unsigned char *input,size_t in_len,unsigned char *output)
```
* **Octal**
```
inline void convert_to_octal_str(const unsigned char *input,size_t in_len,unsigned char *output)
inline void convert_from_octal_str(const unsigned char *input,size_t in_len,unsigned char *output)
```

* **Hex**
```
inline void convert_to_hex_str(const unsigned char *input,size_t in_len,unsigned char *output,bool lower = true)
inline void convert_from_hex_str(const unsigned char *input,size_t in_len,unsigned char *output,bool lower = true)
```

* **base 58 - bitcoin**
```
inline void convert_to_58_str(const unsigned char *input,size_t in_len,unsigned char *output)
inline bool convert_from_58_str(const unsigned char *input,size_t in_len,unsigned char *output,size_t out_size)
```
> Note: the second call is a wrapper around the *b58tobin* function. *convert_from_58_str* shifts the buffer left eliminating zeros. That allows the display of strings.
> For actualy binary buffers, use *b58tobin* .


* **base 64 - bitcoin**
```
inline void convert_to_64_str(const unsigned char *input,size_t in_len,unsigned char *output,bool url_safe = false)
inline void convert_from_64_str(const unsigned char *input,size_t in_len,unsigned char *output)
```

## Coming soon:

As time permits, the methods for base32 will be added. Other flavors of bases can be added as well.

Soon, a wrapper for a node.js module will be added. Other development wrappers will be considered also as time permits.


