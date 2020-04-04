// fileIOmodule.h

#ifndef FILEIOMODULE_H
#define FILEIOMODULE_H

void writeFile(char *buffer, char *path, int *sectors, char parentIndex);
void readFile(char *buffer, char *path, int *result, char parentIndex);
// delete file

#endif