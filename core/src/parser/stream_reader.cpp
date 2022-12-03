#include "stream_reader.h"
#include <fstream>

StreamReader::StreamReader(std::string sourcePath): sourcePath(sourcePath) {}

char StreamReader::peek(int k) {
  // Let's say we want to ac
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
    int length = relativeIndex - bufferSize;
    char* buffer = new char[length];

    std::ifstream is(sourcePath);
    is.seekg(k - bufferStart + bufferSize, std::ios::cur);
    is.read(buffer, length);

    for (int i = 0; i < bufferSize; i++) {
      if (i < length) continue;
      if (i == )
    }

  } else return buffer[relativeIndex];
}

char StreamReader::consume(int k) {
  return 'a';
}