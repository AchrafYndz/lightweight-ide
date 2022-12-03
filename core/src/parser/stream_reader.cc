#include "stream_reader.h"
#include <fstream>
#include <iostream>

StreamReader::StreamReader(std::string sourcePath): sourcePath(sourcePath) {}

int StreamReader::charCount() const {
  if (head == tail) return 0;
  else if (head > tail) return head - tail + 1;
  else return bufferSize - tail + head + 1;
}

char StreamReader::peek(int k) {
  // Let's say we want to access element 6 in file
  // We have a buffer size of 5, with the current buffer start being 2
  // This means that the element at tail is at index 2 in the file, while the element
  // we're looking for is at index tail + 4 = tail + (k - bufferStart)

  // std::cout << lastInBuffer << std::endl;
  // std::cout << relativeIndex << std::endl;
  int lastInBuffer = bufferStart + charCount();

  if (k < bufferStart) {
    // Clear the buffer, we're backtracking
    empty = true;
    tail = 0;
    head = 0;
    bufferStart = 0;
    lastInBuffer = 0;
  }


  if (k >= bufferStart && k <= lastInBuffer && !empty) return buffer[(tail + k - bufferStart) % bufferSize]; 
  else {
    // We always want to read at least one character
    int length = k - lastInBuffer;
    if (empty) length += 1;
    char* read = new char[length];

    std::ifstream is(sourcePath);

    // If empty, we want to read from the first character in the file
    // otherwise we want to continue after the last read character
    is.seekg(empty ? 0 : lastInBuffer + 1, std::ios::cur);
    is.read(read, length);

    is.close();

    for (int i = 0; i < length; i++) {
      if (!empty) head = (head + 1) % bufferSize;
      buffer[head] = read[i];
      if (tail == head && !empty) {
        tail = (tail + 1) % bufferSize;
        bufferStart += 1;
        }
      empty = false;
    }

    delete[] read;
    return buffer[head];
  }
  

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