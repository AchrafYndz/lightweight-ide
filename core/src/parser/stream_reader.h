#ifndef __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_
#define __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_

#include <string>

class StreamReader {
private:
  int bufferStart = 0;
  std::string sourcePath;

  const int bufferSize = 10000;
  char buffer[10000];

public:
  StreamReader(std::string sourcePath);
  char peek(int k);
  char consume(int k);
};

#endif // __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_