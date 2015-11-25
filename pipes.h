#ifndef __PIPES_H__
#define __PIPES_H__

/* Writes a char array to a named pipe to be read later */
void writeToPipe(char** sendv, FILE* pipeDescriptor);

/* Reads a char array from a named pipe, sent by the function above */
void readFromPipe(char** readv, FILE* pipeDescriptor, int maxBufSize);

#endif /* __PIPES_H__ */