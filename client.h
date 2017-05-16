//###########################
//# Projet réseau 
//#RWIBUTSO JEAN BOSCO et HARDI ALEXANDRE
//#serie 2
//#############################
#include "socket.h"
#include    <netdb.h>
#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>

#include    <sys/socket.h>
#include    <sys/types.h>

#include    <netinet/in.h>
#include	<signal.h>
#include	<errno.h>
#define SYS(call) ((call) == -1) ?(perror(#call ":ERROR"),exit(1)) : 0