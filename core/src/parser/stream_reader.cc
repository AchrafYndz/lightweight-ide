#include "stream_reader.h"
#include <fstream>
#include <iostream>

StreamReader::StreamReader(std::string sourcePath): sourcePath(sourcePath) {}

char StreamReader::peek(int k) {
  // Let's say we want to access element 6 in file
  // We have a buffer size of 5, with the current buffer start being 2
  // This means that the element at tail is at index 2 in the file, while the element
  // we're looking for is at index tail + 4 = tail + (k - bufferStart)
  int lastInBuffer = bufferStart + std::abs(head - tail);

  // std::cout << lastInBuffer << std::endl;
  // std::cout << relativeIndex << std::endl;

  if (k < bufferStart) {
    empty = true;
    tail = head;
    bufferStart = k - 1;
  }

  if (k >= bufferStart && k <= lastInBuffer) return buffer[tail + k - bufferStart]; 
  else {
    char* read = new char[k - lastInBuffer + 1];

    std::ifstream is(sourcePath);
    is.seekg(lastInBuffer, std::ios::cur);
    is.read(read, k - lastInBuffer + 1);

    is.close();

    for (int i = 0; i < k - lastInBuffer + 1; i++) {
      if (!empty) head = (head + 1) % bufferSize;
      
      buffer[head] = read[i];

      if (tail == head && !empty) {
        tail = (tail + 1) % bufferSize;
        bufferStart++;
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