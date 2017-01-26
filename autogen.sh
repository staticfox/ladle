#!/bin/sh
which autoreconf > /dev/null
if [ $? -eq 0 ] ; then
    autoreconf -fi
    if [ -f configure ] ; then
        echo "Success. Next step:"
        echo "* Run ./configure to configure Ladle for building"
        echo "* Run make to build Ladle"
    else
        echo "There was a problem configuring Ladle. Please verify that you have autoconf installed correctly."
        exit 1
    fi
else
    echo "Ladle requires autoconf which can be downloaded at https://www.gnu.org/software/autoconf/ or through your system's package manager."
    echo "If you have installed autoconf already, ensure it is in your PATH environment variable."
    exit 1
fi
