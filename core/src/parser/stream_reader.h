#ifndef __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_
#define __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_

class StreamReader {
private:
  int bufferStart = 0;
  int bufferSize = 10000;

public:
  StreamReader(int bufferSize);
  char peek(int k);
  char consume(int k);
};

#endif // __PROJECTS_LIGHTWEIGHT_IDE_V2_CORE_SRC_PARSER_STREAM_READER_H_