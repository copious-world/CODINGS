
/*
-lcrypto

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

int main(int arc, char *argv[])
{ 
  // Load the human readable error strings for libcrypto
  ERR_load_crypto_strings();

  // Load all digest and cipher algorithms 
  OpenSSL_add_all_algorithms();

  // Load config file, and other important initialisation 
  OPENSSL_config(NULL);

  //... Do some crypto stuff here ... 

  // Clean up 

  // Removes all digests and ciphers 
  EVP_cleanup();

  // if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations 
  CRYPTO_cleanup_all_ex_data();

  // Remove error strings
  ERR_free_strings();

  return 0;
}

*/

inline uint32_t log2(uint32_t count) {
    // count bits...
    uint32_t b1 = count;
    uint8_t b = -1;
    while ( b1 ) {
        b1 >>= 1;
        b++;
    }
    return b;
}

#define HASH_SIZE 256
typedef uint8_t uint256[HASH_SIZE];
typedef uint8_t uint512[HASH_SIZE*2];

typedef struct {
    node_256   *_parent;
    node_256   *_sibling;
    uint256     _hash;
}  node_256;


bool (*sha256_process)(const char *data,size_t size,const char *output) = NULL;

void set_sha256((*)(const char *data,size_t size,const char *output) impl) {
    sha256_process = impl
}


void sha256_double_buff(uint512 data,uint256 hash) {
    // call up sha256
    if ( sha256_process ) {
        (*sha256_process)((const char *)data,512,(const char *)hash);
    }
}

static inline node_256 *SHA256D64_homebrew(node_256 *i_buff,uint32_t count) {
    node_256 *get1 = (node_256 *)(&i_buff[0]);
    node_256 *get2 = get1 + 1;
    node_256 *next = get1 + count;
    node_256 *put = next;
    while ( get1 < next ) {
        uint512 combined;
        uint256 *cpy_here = &combined[0];
        memcpy(cpy_here,get1->_hash,HASH_SIZE);
        cpy_here++;
        memcpy(cpy_here,get2->_hash,HASH_SIZE);
        sha256_double_buff(combined,put->_hash);
        get1->_parent = put;
        get2->_parent = put;
        get1->_sibling = get2;
        get2->_sibling = get1;
        put++;
        get1 += 2;
        get2 += 2;
    }
    return next;
}


static inline node_256 *ComputeMerkleRoot(node_256 level_input[], uint32_t count, node_256 bot_up_filler[], bool use_filler, bool* mutated) {
    // assumes level_input is not replicated -- assumed sorted... (so that found replications)
    // assume storage for a complete tree with having up to the root
    uint32_t height = log2(count) + 1;
    node_256 *next = &level_input[0];
    uint32_t level = 0;
    while ( height ) {
        if ( count & 1 ) {
            if ( use_filler ) {
                memcpy(next[count-1]._hash,bot_up_filler[level],HASH_SIZE);
            } else {
                memcpy(next[count-1]._hash,next[count-2]._hash,HASH_SIZE);
            }
        }
        //
        next = SHA256D64_homebrew(next,count);
        count >>= 1; level++; height--;
    }
    if (mutated) *mutated = mutation;
    return next;
}


static inline bool trace_root(node_256 level_input[],uint32_t count,uint256 check) {
    node_256 *check_node = bin_search_node_255(level_input,check_node,count);  // only bottom is sorted
    node_256 *p = check_node->_parent;
    while ( p ) {
        node_256 *s1 = check_node->_sibling;
        node_256 *s2 = check_node;
        //
        if ( s2 < s1 ) { // swap
            node_256 *tmp = s2;
            s2 = s1;
            s1 = tmp;
        }
        //
        uint512 combined;
        uint256 result;
        uint256 *cpy_here = (uint256 *)(&combined[0]);
        memcpy(cpy_here,s1->_hash,HASH_SIZE);
        cpy_here++;
        memcpy(cpy_here,s2->_hash,HASH_SIZE);
        sha256_double_buff(combined,result);
        if ( memcmp(result,p->_hash) != 0 ) {
            return false
        }
        check_node = p;
    }
    return true;
}