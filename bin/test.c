#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>


int  main(void){
int i;
struct hostent *he;
struct in_addr **addr_list;
struct in_addr addr;
char us[80] = {0};
int ul = 0;

while(1){
// get the addresses of www.yahoo.com:
    printf("ip/url/hostname : ");
     fgets(us,879, stdin);
     ul = strlen(us);
    us[ ul-1 ] = 0;
    printf("looking up : {%s}\n",us);
    
    he = gethostbyname(us);
    if (he == NULL) { // do some error checking
        herror("gethostbyname"); // herror(), NOT perror()
        continue; //return(-1);
    }
    
    // print information about this host:
    printf("Official name is: %s\n", he->h_name);
    printf("IP address: %s\n", inet_ntoa(*(struct in_addr*)he->h_addr));
    printf("All addresses: ");
    addr_list = (struct in_addr **)he->h_addr_list;
    for(i = 0; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");
    
    // get the host name of 66.94.230.32:
    
    //inet_aton("66.94.230.32", &addr);
    //he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    
    printf("Host name: %s\n", he->h_name);
}
return 0;
}
