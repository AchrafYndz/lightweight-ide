#ifndef __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_
#define __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_

#include <string>

class StreamReader {
private:
  std::string sourcePath;

  const int bufferSize = 5;
  char buffer[5];

  int head = 0;
  int tail = 0;
  int bufferStart = 0;
  int empty = true;

public:
  StreamReader(std::string sourcePath);
  char peek(int k, int& i);
  char peek(int k);
  char consume(int k);
  int charCount() const;
};

#endif // __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_