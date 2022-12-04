#include "stream_reader.h"
#include <fstream>
#include <iostream>

StreamReader::StreamReader(std::string sourcePath): sourcePath(sourcePath) {}

int StreamReader::charCount() const {
  if (empty) return 0;
  else if (head == tail) return 1;
  else if (head >= tail) return head - tail + 1;
  else return bufferSize - tail + head + 1;
}

char StreamReader::peek(int k) {
  // Let's say we want to access element 6 in file
  // We have a buffer size of 5, with the current buffer start being 2
  // This means that the element at tail is at index 2 in the file, while the element
  // we're looking for is at index tail + 4 = tail + (k - bufferStart)

  int cc = charCount();

  int lastInBuffer = bufferStart + cc;

  // Let's say the buffer is [0, 1, 2] with bufferStart 0 and character count 3
  // lastInBuffer would then be 0 + 3 = 3, lastInBuffer should be 2
  if (cc > 0) lastInBuffer -= 1;

  // TODO: @Flor refactor to merge edge case handling
  int length;
  if (empty) {
    int length = k + 1;
    char* read = new char[length];

    std::ifstream is(sourcePath);
    is.seekg(0, std::ios::cur);
    is.read(read, length);
    is.close();

    for (int i = 0; i < length; i++) {
      if (!empty) head = (head + 1) % bufferSize;
      if (head == tail && !empty) tail = (tail + 1) % bufferSize;

      buffer[head] = read[i];
      empty = false;
    }

    bufferStart = std::max(0, length - bufferSize);

    return buffer[head];
  }

  if (k > lastInBuffer) {
    // Read until we reach k
    int length = k - lastInBuffer;
    char* read = new char[length];

    std::ifstream is(sourcePath);
    is.seekg(lastInBuffer + 1, std::ios::cur);
    is.read(read, length);
    is.close();

    for (int i = 0; i < length; i++) {
      head = (head + 1) % bufferSize;
      if (head == tail) tail = (tail + 1) % bufferSize;

      buffer[head] = read[i];
      empty = false;
    }
    
    if (bufferSize - cc < length) bufferStart = bufferStart + length - (bufferSize - cc);

    return buffer[head];
  } else if (k < lastInBuffer) {
    head = tail;
    bufferStart = k;

    char* read = new char[1];

    std::ifstream is(sourcePath);
    is.seekg(k, std::ios::cur);
    is.read(read, 1);
    is.close();
    
    buffer[head] = read[0];

    return buffer[head];
  } else return buffer[(tail + k - bufferStart) % bufferSize];

  // if (relativeIndex < 0) {
  //   // Reset the buffer the buffer to character k
  //   // int previousStart = bufferStart;
  //   bufferStart = k;

  //   // Read from filew
  //   // TODO: optimization opportunity -> read from current buffer if possible
  //   char* buffer = new char[bufferSize];

  //   std::ifstream is(sourcePath);
  //   is.seekg(k, std::ios::cur);
  //   is.read(buffer, bufferSize);

  //   is.close();

  //   for (int i = 0; i < bufferSize; i++) this->buffer[i] = *(buffer + i);

  // } else if (relativeIndex >= bufferSize) {
  //   // Read to this character
  //   int length = relativeIndex - bufferSize;
  //   char* buffer = new char[length];

  //   std::ifstream is(sourcePath);
  //   is.seekg(k - bufferStart + bufferSize, std::ios::cur);
  //   is.read(buffer, length);

  //   for (int i = 0; i < bufferSize; i++) {
  //     if (i < length) continue;
  //     if (i == )
  //   }

  // } else return buffer[relativeIndex];
}

char StreamReader::consume(int k) {
  return 'a';
}