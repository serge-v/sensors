/*
*  node7: Control program for sensors network.
*  Copyright (C) 2014 Serge Voilokov
*
*  Based on examples and driver from:
*  https://github.com/gkaindl/rfm12b-linux
*  rfm12b-linux: linux kernel driver for the rfm12(b) RF module by HopeRF
*  Copyright (C) 2013 Georg Kaindl
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/ioctl.h>

#include <rfm12b_config.h>
#include <rfm12b_ioctl.h>
#include <rfm12b_jeenode.h>

#define JEENODE_ID      7

#define RF12_MAX_RLEN   128
#define RF12_MAX_SLEN   66

#define SEND_COLOR      "\033[1;32m"
#define RECV_COLOR      "\033[1;31m"
#define STOP_COLOR      "\033[0m"

static volatile int running;

static void
sig_handler(int signum)
{
	signal(signum, SIG_IGN);
	running = 0;
}

static int
set_nonblock_fd(int fd)
{
	int opts;

	opts = fcntl(fd, F_GETFL);
	if (opts >= 0)
	{
		opts = (opts | O_NONBLOCK);
		if (fcntl(fd, F_SETFL, opts) >= 0)
			return 0;
	}

	return -1;
}

struct rfm12_device {
	char* devname;
	int jee_id;   // jeenode id
	int band_id;  // band (1,2,3 == 434,868,915). 
	int group_id; // see ../../doc/rfm12b.txt:"Synchron pattern" 
	int bit_rate; // see ../../doc/rfm12b.txt:"Data Rate Command" 
	int send_ack; // enable sending jeenode acknoledge
	int fd;
};

static struct rfm12_device rfm12 = {
	.devname = RF12_TESTS_DEV,
//	.devname = "test_fifo",
	.jee_id = JEENODE_ID,
	.send_ack = 0,
};

static int
init(struct rfm12_device* d)
{
	if (set_nonblock_fd(STDIN_FILENO))
	{
		fprintf(stderr, "failed to set non-blocking I/O on stdin: %s.\n",
		       strerror(errno));
		return -1;
	}

	d->fd = open(d->devname, O_RDWR);
	if (d->fd < 0)
	{
		fprintf(stderr, "failed to open %s: %s.\n", d->devname, strerror(errno));
		return -1;
	}

	if (set_nonblock_fd(d->fd))
	{
		fprintf(stderr, "failed to set non-blocking I/O on %s: %s.\n",
		       d->devname, strerror(errno));
		close(d->fd);
		return -1;
	}
	
	printf("successfully opened %s as fd %i.\n", d->devname, d->fd);

	int ioctl_err = 0;

	ioctl_err |= ioctl(d->fd, RFM12B_IOCTL_GET_GROUP_ID, &d->group_id);
	ioctl_err |= ioctl(d->fd, RFM12B_IOCTL_GET_BAND_ID, &d->band_id);
	ioctl_err |= ioctl(d->fd, RFM12B_IOCTL_GET_BIT_RATE, &d->bit_rate);
	
	ioctl_err |= ioctl(d->fd, RFM12B_IOCTL_SET_JEEMODE_AUTOACK, &d->send_ack);

	if (ioctl_err)
	{
		close(d->fd);
		fprintf(stderr, "ioctl() error while setting autoack: %s.\n", strerror(errno));
		return -1;
	}

	if (ioctl(d->fd, RFM12B_IOCTL_SET_JEE_ID, &d->jee_id))
	{
		close(d->fd);
		fprintf(stderr, "ioctl() error while setting jeenode id: %s.\n", strerror(errno));
		return -1;
	}

	printf("group_id: %d, band_id: %d, bit_rate: 0x%0X, jee_id: %d\n",
		d->group_id, d->band_id, d->bit_rate, d->jee_id);


	return 0;
}

static int
send_buffer(struct rfm12_device* d, const char* buf, int len)
{
	len = write(d->fd, buf, len);

	if (len < 0)
	{
		fprintf(stderr, "error while sending: %s.\n",
		       strerror(errno));
		return -1;
	}

	printf(SEND_COLOR "<SENT CTL:0 ACK:1 DST:0 ADDR:%d LEN:%d>" STOP_COLOR " %s\n",
	       d->jee_id, len - 2, buf + 2);
	return len;
}

static void
print_buffer(char* obuf, int len)
{
	int i, has_ack, has_ctl, is_dst, jee_addr, jee_len;

	// replace non-printable ASCII characters with a dot. the
	// first two bytes are preserved, though, since they are the
	// jeenode hdr and len bytes.
	for (i = 2; i < len; i++)
		if (' ' > obuf[i] || '~' < obuf[i])
			obuf[i] = '.';

	obuf[len] = '\0';

	has_ack = (RFM12B_JEE_HDR_ACK_BIT & obuf[0]) ? 1 : 0;
	has_ctl = (RFM12B_JEE_HDR_CTL_BIT & obuf[0]) ? 1 : 0;
	is_dst = (RFM12B_JEE_HDR_DST_BIT & obuf[0]) ? 1 : 0;
	jee_addr = RFM12B_JEE_ID_FROM_HDR(obuf[0]);
	jee_len = obuf[1];

	printf(RECV_COLOR "<RECV CTL:%d ACK:%d DST:%d ADDR:%d LEN:%d>" STOP_COLOR " %s\n",
	       has_ctl, has_ack, is_dst, jee_addr, jee_len, &obuf[2]);

	// in jeenode-compatible mode, the driver sends ACK packets automatically by default,
	// so we're just writing out here what the driver does internally. you do
	// not need to manually send an ACK if one is requested, though!
	if ((obuf[0] & RFM12B_JEE_HDR_ACK_BIT) && !(obuf[0] & RFM12B_JEE_HDR_CTL_BIT))
	{
		if (obuf[0] & RFM12B_JEE_HDR_DST_BIT)
		{
			// if this was only sent to us, an ACK is sent as broadcast
			printf(SEND_COLOR "<SENT CTL:1 ACK:0 DST:0 ADDR:%d LEN:0>\n" STOP_COLOR,
			       rfm12.jee_id);
		}
		else
		{
			// if this was a broadcast, the ACK is sent directly to the source node.
			printf(SEND_COLOR "<SENT CTL:1 ACK:0 DST:1 ADDR:%d LEN:0>\n" STOP_COLOR,
			       RFM12B_JEE_ID_FROM_HDR(obuf[0]));
		}
	}
}

static void
process(struct rfm12_device* d, char* obuf, int len)
{
	int ipos = 2, jee_addr;
	jee_addr = RFM12B_JEE_ID_FROM_HDR(obuf[0]);

	if (jee_addr == 11)
	{
		sleep(1);
		char buf[RF12_MAX_SLEN + 1];
		
		buf[0] = rfm12.jee_id | RFM12B_JEE_HDR_ACK_BIT;
		buf[ipos++] = 'c';
		buf[ipos++] = '1';
		buf[ipos++] = '2';

		buf[1] = ipos - 2; // len of payload
		buf[ipos] = 0;

		send_buffer(d, buf, ipos);
	}
}

int
main(int argc, char** argv)
{
	int len, nfds, ipos;

	char ibuf[RF12_MAX_SLEN + 1], obuf[RF12_MAX_RLEN + 1], c;
	fd_set fds;

	if (init(&rfm12) < 0)
		return 1;

	fflush(stdout);
	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);

	printf("ready, type something to send it as broadcast + ACK request.\n\n");

	running = 1;
	ipos = 2;
	while (running)
	{
		FD_ZERO(&fds);
		FD_SET(STDIN_FILENO, &fds);
		FD_SET(rfm12.fd, &fds);

		nfds = select(rfm12.fd + 1, &fds, NULL, NULL, NULL);

		if (nfds < 0 && running)
		{
			printf("\nan error happened during select: %s.\n\n", strerror(errno));
			return -1;
		}

		if (nfds == 0)
			continue;

		// we ignore when select() returns 0, e.g. nothing is readable.
		if (FD_ISSET(STDIN_FILENO, &fds))
		{
			// we can read from stdin, so read each available character until
			// we run out of space or get a \n, which means we should send

			len = read(STDIN_FILENO, &c, 1);
			
			if (len < 0 && len != EWOULDBLOCK)
			{
				fprintf(stderr, "error while reading from stdin: %s.\n", strerror(errno));
				return 1;
			}

			if (len == 0)
				continue;

			if ('\n' == c)
			{
				// in jeenode-compatible mode, we fill in the hdr and len
				// fields manually and write them as part of the send buffer.
				// what we do here is send a broadcast (DST is 0), but
				// request an ACK from receivers. If they send an ACK, we
				// will receive it, and you'll see the output on the console.
				//
				// note that the "len" byte will be "fixed" by the driver if
				// you supply a length different from the amount of bytes you
				// actually write().

				ibuf[0] = rfm12.jee_id | RFM12B_JEE_HDR_ACK_BIT;   // hdr
				ibuf[1] = ipos - 2;                             // len of payload
				ibuf[ipos] = 0;
				len = send_buffer(&rfm12, ibuf, ipos);
				if (len < 0)
					return 1;
				ipos = 2;
			}
			else if (ipos < RF12_MAX_SLEN)
				ibuf[ipos++] = c;
		}
		else if (FD_ISSET(rfm12.fd, &fds))
		{
			len = read(rfm12.fd, obuf, RF12_MAX_RLEN);

			if (len < 0)
			{
				printf("\nerror while receiving: %s.\n\n", strerror(errno));
				return 1;
			}
			
			if (len > 0)
			{
				print_buffer(obuf, len);
				process(&rfm12, obuf, len);
			}
		}
	}

	printf("\n");
	close(rfm12.fd);

	return 0;
}
