Usage
============

To compress the file *input1* to file *output1*, invoke the program as
`huffman -c input1 output1`.  To decompress the file *input1* to the file
*output1*, invoke as `huffman -d input1 output1`.

Overview
====
This project is a implemenation of the Huffman codes compression algorithm.
When compressing a file, it reads the file and counts the frequencies of each
character.  Then, it uses the Huffman codes algorithm to come up with an
encoding (a mapping of characters to bitstrings) for that file based on those
frequencies.  It writes a table of that encoding to the output file.  Then,
it goes back to the beginning of the input file, reads it byte by byte, writing
the bitstring corresponding to each byte to the output file.

When decompressing a file, the program reads and parses the encoding table
from the input file.  Then, it reads the rest of the input file, bit-by-bit,
until it finds a bit-sequence that was in the encoding table, writing the
corresponding character to the output file and repeating the process until the
end of the input file is reached.

Details
====

The Encoding Table
--------

In the encoding table, each character is followed by its encoding, which is
a sequence of bits padded by zeros so that they are all the same width.  A 1
is used to seperate the bitstring from the additional zeros. Since the number
of bytes needed to store the longest bitstring depends on the file being
compressed, that number is written at the beginning of the table.  The end
of the table is marked by a repetition of the first character, without its
bitstring.

Bitstream
-----

I called the modules that read and write bitstrings to file `ibitstream` and
`obitstream` respectively.  Both of them use a bitset as a buffers.  `obitstream` copies bitstrings to the buffer, writing the buffer to the file when it is
full.  The `ibitstream` constructor takes a set of valid bitstrings.
The `extract` function reads bits from the buffer until they form a valid
sequence and returns that sequence.  When the end of the buffer is reached, it
is more data it is refilled with more bits from the file.



Marking the end of the File
---------------

After the `obitstream` writes  the last bitstring to the buffer, the buffer
will be written to the file, even if it is not full, causing the file to be
padded with zeros. When decompressing a file, the program cannot just read
until the end of the file because it will interpret the padding as extra
characters, causing the output file to end with gibberish.  Therefore, the
least commonly occuring character is used to mark the end of the file.  It
is written to the file right after the encoding table (so that the program
will know which character to look for when decompressing the file), and any
subsequent occurrance of the character is repeated until the end of the file.
When the encoding of the character is encountered once, that marks the end
of the file.

Error Detection
-----

If the `ibitstream::extract` function comes across an unrecognized sequence
that is as long as the longest bitstring in the encoding, the stream will
lock and the program will report that file is corrupt.  Similarly, if the
end of the file is reached without encountering an EOF character then the file
must be corrupt.





