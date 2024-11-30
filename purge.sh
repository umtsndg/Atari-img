#!/bin/bash

# Name of the disk image file
IMAGE_FILE="storage_vgc.img"

# Name for the symbolic link to the device file
SYMLINK_NAME="device-file"

# Mount directory
MOUNT_DIR="mount"

# Check if the disk image file exists
if [ -f "$IMAGE_FILE" ]; then
    # Find the loop device associated with the disk image
    LOOP_DEVICE=$(sudo losetup -j "$IMAGE_FILE" | awk -F: '{print $1}')

    if [ ! -z "$LOOP_DEVICE" ]; then
        echo "Loop device associated with $IMAGE_FILE is $LOOP_DEVICE."

        # Unmount the loop device from the mount directory
        if mount | grep "$LOOP_DEVICE" > /dev/null; then
            sudo umount "$MOUNT_DIR"
            echo "Unmounted $LOOP_DEVICE from $MOUNT_DIR."
        else
            echo "$LOOP_DEVICE is not mounted."
        fi

        # Detach the loop device
        sudo losetup -d "$LOOP_DEVICE"
        echo "Detached loop device $LOOP_DEVICE."
    else
        echo "No loop device is associated with $IMAGE_FILE."
    fi

    # Remove the disk image file
    sudo rm -f "$IMAGE_FILE"
    echo "Removed disk image file $IMAGE_FILE."
else
    echo "Disk image file $IMAGE_FILE does not exist."
fi

# Remove the symbolic link to the device file
if [ -L "$SYMLINK_NAME" ]; then
    rm "$SYMLINK_NAME"
    echo "Removed symbolic link $SYMLINK_NAME."
else
    echo "Symbolic link $SYMLINK_NAME does not exist."
fi

# Optionally, remove the mount directory
if [ -d "$MOUNT_DIR" ]; then
    rm -rf "$MOUNT_DIR"
    echo "Removed mount directory $MOUNT_DIR."
fi
