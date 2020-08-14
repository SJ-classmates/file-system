#ifndef _FILE_OPERATOR_H
#define _FILE_OPERATOR_H

int file_open(char *filename, int mode);
int file_close(int fd);
int file_read(int fd, char *buffer, int size);
int file_write(int fd, char *buffer, int size);

#endif // _FILE_OPERATOR_H
