/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags --libs` hello.c -o hello
*/

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "fs.h"



static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
	}
	else if(strcmp(path, "/hello_dir") == 0){
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
	}
	else
		setattr(path, stbuf);

	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0){
			filler(buf, ".", NULL, 0);
			filler(buf, "..", NULL, 0);

			//in the future, instead of fs.h magically being the only thing in each non-root
			//directory we should go through and get a list of all items in that dir

			return -EACCES;
		}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, "/hello_dir" + 1, NULL, 0);
	return 0;
}

static int hello_open(const char *path, struct fuse_file_info *fi)
{
	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int hello_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;

	int fd = open("fuse_fs", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	void* msg = extractfilefs(path, fd);

	len = strlen(msg);
	size = len;
	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		memcpy(buf, msg + offset, len);
	} else
		size = 0;

	return size;
}

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
