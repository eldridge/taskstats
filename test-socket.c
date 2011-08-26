#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <linux/netlink.h>


#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

int main (int argc, char ** argv)
{
	int						s;
	struct sockaddr_nl *	addr;

	addr = malloc(sizeof(struct sockaddr_nl));

	memset(addr, 0, sizeof(struct sockaddr_nl));

	s = socket(AF_NETLINK, SOCK_RAW, NETLINK_GENERIC);

	if (bind(s, (struct sockaddr *) addr, sizeof(struct sockaddr_nl)))
		printf("Wee!\n");
	else
		printf("poo!\n");
}
