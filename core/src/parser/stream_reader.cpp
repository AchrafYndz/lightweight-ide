#include "stream_reader.h"
#include <fstream>

StreamReader::StreamReader(std::string sourcePath): sourcePath(sourcePath) {}

char StreamReader::peek(int k) {
  // Let's say we're trying to read the 10th char (k = 10)
  // and our buffer starts at index 2 (zero-based)
  // then our relative index is 8
  int relativeIndex = k - bufferStart;

  if (relativeIndex < 0) {
    // Reset the buffer the buffer to character k
    // int previousStart = bufferStart;
    bufferStart = k;

    // Read from filew
    // TODO: optimization opportunity -> read from current buffer if possible
    char* buffer = new char[bufferSize];

    std::ifstream is(sourcePath);
    is.seekg(k, std::ios::cur);
    is.read(buffer, bufferSize);

    is.close();

    for (int i = 0; i < bufferSize; i++) this->buffer[i] = *(buffer + i);

  } else if (relativeIndex >= bufferSize) {
    // Read to this character
  } else return buffer[relativeIndex];
}

char StreamReader::consume(int k) {
  return 'a';
}