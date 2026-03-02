int ruserok(const char *rhost, int superuser, const char *ruser, const char *luser);
void rcmd(char **ahost, unsigned short inport, const char *locuser, const char *remuser, const char *cmd, int *fd2p);
