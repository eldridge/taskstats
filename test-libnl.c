#include <stdlib.h>

#include <linux/taskstats.h>

#include <netlink/netlink.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

int callback_message (struct nl_msg *, void *);

int main (int argc, char ** argv)
{
	struct nl_sock *	sock;
	struct nl_msg *		msg;
	int					family;

	sock = nl_socket_alloc();

	// Connect to generic netlink socket on kernel side
	genl_connect(sock);

	// get the id for the TASKSTATS generic family
	family = genl_ctrl_resolve(sock, "TASKSTATS");

	// register for task exit events

	msg = nlmsg_alloc();

	genlmsg_put(msg, NL_AUTO_PID, NL_AUTO_SEQ, family, 0, NLM_F_REQUEST, TASKSTATS_CMD_GET, TASKSTATS_VERSION);
	nla_put_string(msg, TASKSTATS_CMD_ATTR_REGISTER_CPUMASK, "1,2");
	nl_send_auto_complete(sock, msg);
	nlmsg_free(msg);

	// specify a callback for inbound messages
	nl_socket_modify_cb(sock, NL_CB_MSG_IN, NL_CB_CUSTOM, callback_message, NULL);

	for (;;)
		nl_recvmsgs_default(sock);
}

int callback_message (struct nl_msg * nlmsg, void * arg)
{
	struct nlmsghdr *	nlhdr;
	struct nlattr *		nlattrs[TASKSTATS_TYPE_MAX+1];
	struct nlattr *		nlattr;
	struct taskstats *	stats;
	int rem;

	nlhdr = nlmsg_hdr(nlmsg);

	// validate message and parse attributes
	genlmsg_parse(nlhdr, 0, nlattrs, TASKSTATS_TYPE_MAX, 0);

	if (nlattr = nlattrs[TASKSTATS_TYPE_AGGR_PID]) {
		stats = nla_data(nla_next(nla_data(nlattr), &rem));

		printf("---\n");
		printf("pid: %u\n", stats->ac_pid);
		printf("command: %s\n", stats->ac_comm);
		printf("status: %u\n", stats->ac_exitcode);
		printf("time:\n");
		printf("  start: %u\n", stats->ac_btime);
		printf("  elapsed: %lu\n", stats->ac_etime);
		printf("  user: %lu\n", stats->ac_utime);
		printf("  system: %lu\n", stats->ac_stime);
		printf("memory:\n");
		printf("  bytetime:\n");
		printf("    rss: %lu\n", stats->coremem);
		printf("    vsz: %lu\n", stats->virtmem);
		printf("  peak:\n");
		printf("    rss: %lu\n", stats->hiwater_rss);
		printf("    vsz: %lu\n", stats->hiwater_vm);
		printf("io:\n");
		printf("  bytes:\n");
		printf("    read: %lu\n", stats->read_char);
		printf("    write: %lu\n", stats->write_char);
		printf("  syscalls:\n");
		printf("    read: %lu\n", stats->read_syscalls);
		printf("    write: %lu\n", stats->write_syscalls);
	}

	return 0;
}
