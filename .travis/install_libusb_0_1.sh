#!/bin/bash

# Install libusb 0.1

echo "Entering install_libusb_0_1"
echo "Installing libusb 0.1"

if [[ $TRAVIS_OS_NAME == 'osx' ]]; then

	# Install package dependencies for Mac OS X:
	#brew update
	brew install libusb-compat

else

	# Install package dependencies for Linux:
	sudo apt-get install --force-yes -y libusb-dev

fi

echo "Installed libusb 0.1"
