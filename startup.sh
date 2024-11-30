#!/bin/bash

# Name of the disk image file
IMAGE_FILE="storage_vgc.img"

# Name for the symbolic link to the device file
SYMLINK_NAME="device-file"

# Mount directory
MOUNT_DIR="mount"

# Check if the disk image file exists
if [ ! -f "$IMAGE_FILE" ]; then
    echo "Disk image file $IMAGE_FILE does not exist. Please run initialize.sh first."
    exit 1
fi

# Set up a loop device and attach the image
LOOP_DEVICE=$(sudo losetup -f --show "$IMAGE_FILE")

# Check if losetup succeeded
if [ -z "$LOOP_DEVICE" ]; then
    echo "Failed to set up loop device."
    exit 1
fi

echo "Disk image attached to loop device $LOOP_DEVICE."

# Create a symbolic link to the loop device in the current directory
ln -sf "$LOOP_DEVICE" "$SYMLINK_NAME"
echo "Symbolic link $SYMLINK_NAME created for $LOOP_DEVICE."

# Create the mount directory if it doesn't exist
if [ ! -d "$MOUNT_DIR" ]; then
    mkdir "$MOUNT_DIR"
    echo "Mount directory $MOUNT_DIR created."
fi

# Mount the loop device to the mount directory
sudo mount "$LOOP_DEVICE" "$MOUNT_DIR"
echo "Loop device $LOOP_DEVICE mounted to $MOUNT_DIR."
