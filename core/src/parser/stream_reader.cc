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

char StreamReader::peek(int k, int& i) {
  // Let's say we want to access element 6 in file
  // We have a buffer size of 5, with the current buffer start being 2
  // This means that the element at tail is at index 2 in the file, while the element
  // we're looking for is at index tail + 4 = tail + (k - bufferStart)
  int cc = charCount();

  int lastInBuffer = bufferStart + cc;

  // Let's say the buffer is [0, 1, 2] with bufferStart 0 and character count 3
  // lastInBuffer would then be 0 + 3 = 3, lastInBuffer should be 2
  if (cc > 0) lastInBuffer -= 1;

  int length;
  int readStart;
  if (empty) {
    // We want to load k+1 items from the file
    length = k + 1;
    // We want to start reading at zero index
    // This can be changed to k - 1 in the future for better performance
    readStart = 0;
    // Update buffer start
    bufferStart = std::max(0, length - bufferSize);
  } else if (k > lastInBuffer) {
    // Read until we reach k
    length = k - lastInBuffer;
    readStart = lastInBuffer + 1;
    if (bufferSize - cc < length) bufferStart = bufferStart + length - (bufferSize - cc);
  } else if (k < lastInBuffer) {
    // Read 
    length = 1;
    readStart = k;
    head = tail;
    bufferStart = k;

    // Mark as empty so the head does not get advanced
    empty = true;
  } else {
    i = (tail + k - bufferStart) % bufferSize;
    return buffer[(tail + k - bufferStart) % bufferSize];
  }

  char* read = new char[length];

  std::ifstream is(sourcePath);
  is.seekg(readStart, std::ios::cur);
  is.read(read, length);
  is.close();

  for (int i = 0; i < length; i++) {
    if (!empty) head = (head + 1) % bufferSize;
    if (head == tail && !empty) tail = (tail + 1) % bufferSize;

    buffer[head] = read[i];
    empty = false;
  }

  i = head;
  return buffer[head];
}

char StreamReader::peek(int k) {
  int i;
  return peek(k, i);
}

char StreamReader::consume(int k) {
  int i = 0;
  char v = peek(k, i);

  tail = i;
  bufferStart = k;
  
  return v;
}