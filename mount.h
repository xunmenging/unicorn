#ifndef MOUNT_H
#define MOUNT_H

int mount_init(char * mount_base);

//install roofs_base to dst_dir for later chroot
int prepare_mount(char * mount_base, char * unicorn_id, char * rootfs_base);

int pivot_move(char * mount_base, char * unicorn_id);

#endif
