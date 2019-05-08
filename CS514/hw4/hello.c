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

static const char *hello_str = "Hello World!\n";
static const char *hello_path = "/hello";
static const char *hello_path2 = "/hello_dir/fs.h";
static const char *hello_dir = "/hello_dir";

static int hello_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
	} else if (strcmp(path, hello_path) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 496000;
	}
	else if (strcmp(path, hello_path2) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = 496000;
	}
	else if (strcmp(path, "/fs.h") == 0) {
		stbuf->st_mode = S_IFREG | 0744;
		stbuf->st_nlink = 1;
		stbuf->st_size = 496000;
	}
	else if (strcmp(path, hello_dir) == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 1;
	} else
		res = -ENOENT;


	return res;
}

static int hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0){
		if(strcmp(path, hello_dir) != 0){
			return -ENOENT;
		}
		else{
			filler(buf, ".", NULL, 0);
			filler(buf, "..", NULL, 0);
			filler(buf, "/fs.h" + 1, NULL, 0);
			return 0;
		}
		return -ENOENT;
	}

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	filler(buf, hello_path + 1, NULL, 0);
	filler(buf, "/fs.h" + 1 , NULL, 0);
	filler(buf, hello_dir + 1, NULL, 0);

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

	printf("beginning read\n\n\n\n");
	int fd = open("test", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	void* msg = extractfilefs(path, fd);
	printf("%02x\n\n\n\n", ((char *)msg)[0]);
	//msg = extractfilefs(path)

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

//static int hello_opendir(const char* path, off_t offset, struct fust_file_info* fi){

//}

static struct fuse_operations hello_oper = {
	.getattr	= hello_getattr,
	.readdir	= hello_readdir,
	.open		= hello_open,
	.read		= hello_read,
//	.readdir  = hello_opendir,
};

int main(int argc, char *argv[])
{
	return fuse_main(argc, argv, &hello_oper, NULL);
}
