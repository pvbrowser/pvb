#ifdef unix
int setPort(char *name, char *baud, char parity);
#else
HANDLE WINAPI setPort(char *name, char *baud, char parity);
#endif
