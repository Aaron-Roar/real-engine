#ifndef CONSOLE_H
#define CONSOLE_H
typedef enum SourceType {ENGINE, APP, ERROR} SourceType;
void console_write(SourceType source, const char *fmt, ...);
extern const char console_legend[];
#endif

