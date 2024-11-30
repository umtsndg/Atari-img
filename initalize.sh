#!/bin/bash

# Name of the disk image file
IMAGE_FILE="storage_vgc.img"

# Remove existing image file if it exists
if [ -f "$IMAGE_FILE" ]; then
    sudo rm -f "$IMAGE_FILE"
    echo "Existing $IMAGE_FILE removed."
fi

# Create a new empty disk image file of 100MB
echo "Creating a new disk image file..."
sudo dd if=/dev/zero of="$IMAGE_FILE" bs=1M count=100 status=progress
echo "Disk image file created."

# Format the disk image file as ext4 filesystem
echo "Formatting the disk image file as ext4..."
sudo mkfs.ext4 -F "$IMAGE_FILE"
echo "Disk image formatted as ext4."
