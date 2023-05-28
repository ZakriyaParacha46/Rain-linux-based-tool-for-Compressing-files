////////////////////////////////////////////////////////////////////////////////
// COMP1521 23T1 --- Assignment 2: `rain', a simple file archiver          //
// <https://www.cse.unsw.edu.au/~cs1521/23T1/assignments/ass2/index.html>     //
//                                                                            //
// 2023-04-06   v1.0    Team COMP1521 <cs1521 at cse.unsw.edu.au>             //
//                                                                            //
///////////////////// YOU DO NOT NEED TO MODIFY THIS FILE. /////////////////////

////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
// THIS FILE IS NOT SUBMITTED AND WILL BE PROVIDED AS IS DURING TESTING AND MARKING //
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////


#ifndef _RAIN_H
#define _RAIN_H

#include <stdint.h>


// list_drop, check_drop, extract_drop, and create_drop are defined in rain.c
// Implement these functions in rain.c

void list_drop(char *drop_pathname, int long_listing);
void check_drop(char *drop_pathname);
void extract_drop(char *drop_pathname);
void create_drop(char *drop_pathname, int append, int droplet_format, int n_pathnames, char *pathnames[n_pathnames]);


// droplet_hash is defined in rain_hash.c
// This function is provided for you to use in rain.c

uint8_t droplet_hash(uint8_t current_hash_value, uint8_t byte_value);


// droplet_to_6_bit, and droplet_from_6_bit are defined in rain_6_bit.c
// These functions are provided for you to use in rain.c

int droplet_to_6_bit(uint8_t eight_bit_value);
int droplet_from_6_bit(uint8_t six_bit_value);


// Useful constants for you to use in rain.c

/** The file `.drop` format magic number. */
static const uint8_t DROPLET_MAGIC  = 0x63;

/** The three types of droplet format. */
enum droplet_fmt {
    DROPLET_FMT_6 = 0x36,
    DROPLET_FMT_7 = 0x37,
    DROPLET_FMT_8 = 0x38,
};

/** Droplet Offsets. */
static const int DROP_OFFSET_MAGIC    = 0;
static const int DROP_OFFSET_FORMAT   = 1;
static const int DROP_OFFSET_MODE     = 2;
static const int DROP_OFFSET_PATHNLEN = 12;

/** Droplet lengths. */
static const int DROP_LENGTH_MAGIC    = 1;
static const int DROP_LENGTH_FORMAT   = 1;
static const int DROP_LENGTH_MODE     = 10;
static const int DROP_LENGTH_PATHNLEN = 2;
static const int DROP_LENGTH_CONTLEN  = 6;
static const int DROP_LENGTH_HASH     = 1;

/**
 * An drop consists of 1 or more droplets.
 * Each droplet contains the information about one file or directory.
 *
 * The droplet format:
 * ------------------------------------
 *
 *  - 'magic_number':    byte 0 in every droplet must be 0x63 (ASCII 'c')
 *
 *  - 'droplet_format':   byte 1 in every droplet must be one of
 *                       0x36, 0x37, 0x38 (ASCII '6', '7' '8')
 *
 *  - 'mode':            bytes 2-11 are the type and permissions as
 *                       a ls(1)-like character array; e.g., "-rwxr-xr-x"
 *
 *  - 'pathname_length': bytes 12-13 are an unsigned 2-byte (16-bit)
 *                       little-endian integer, giving the length of the
 *                       file path that was give
 *
 *  - 'pathname':        the filename of the object in this droplet.
 *                       this is 'pathname_length' bytes long.
 *
 *  - 'content_length':  the next bytes are an unsigned 6-byte (48-bit)
 *                       little-endian integer, giving the length of the
 *                       file that was encoded
 *
 *  - 'contents':        the data of the object in this droplet.
 *                       this is 'content_length' bytes long.
 *
 *  - 'hash':            the last byte of an droplet is a `droplet_hash'
 *                       of all bytes of this droplet except this byte.
 *
 *
 * droplet content encodings:
 * ------------------------------------
 *
 *  - droplet format 0x38 ('8'):
 *    `contents' is a plain array of bytes.
 *
 *  - droplet format 0x37 ('7'):
 *    `contents' is an array of bytes of packed seven-bit values where
 *    the trailing bits in the last byte are zero.
 *
 *    This format can store any seven bit values --- so, for example,
 *    any valid ASCII can be stored.
 *
 *    This format needs to store ceil((7.0/8) * content_length) bytes.
 *
 *  - droplet format 0x36 ('6'):
 *    `contents' is an array of bytes of packed six-bit values where the
 *    trailing bits in the last byte are zero, and which are translated
 *    by functions `rain_6_bit.c'.
 *
 *    This format cannot store all of ASCII!
 *
 *    This format needs to store ceil((6.0/8) * content_length) bytes.
 *
 *
 * Packed n-bit encoding:
 * ------------------------------------
 *
 * We often store smaller values inside larger types.  For example, the
 * integer 42 only needs six bits; but we often will store it in a full
 * thirty-two-bit integer, wasting many bits of zeroes.  Assuming we
 * know how many bits the value needs, we could only store the relevant
 * bits.
 *
 * For example, let's say we have three seven-bit values `a', `b', `c',
 * made up of arbitrary bitstrings, and stored in eight-bit variables
 *
 *     a: 0b0AAA_AAAA,  b: 0b0BBB_BBBB,  c: 0b0CCC_CCCC,
 *
 * then a packed seven-bit encoding of these values in order would be:
 *
 *        0bAAAA_AAAB_BBBB_BBCC_CCCC_C???
 *
 * However, we have a problem: what happens to the trailing bits, which
 * don't have a value?  Note that we've defined all trailing bits to be
 * zero above, which would here give:
 *
 *        0bAAAA_AAAB_BBBB_BBCC_CCCC_C000
 *
 */

#endif // _RAIN_H


////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
// THIS FILE IS NOT SUBMITTED AND WILL BE PROVIDED AS IS DURING TESTING AND MARKING //
////////////////////////////// DO NOT CHANGE THIS FILE. //////////////////////////////
