#!/bin/sh
#IFS='.' read -a VERS <<< "2.6.18"
IFS='.' read -a VERS <<< "$(uname -r)"

if [ ${VERS[0]} -lt 2 -o ${VERS[0]} -gt 4 ] \
|| [ ${VERS[0]} -eq 2 -a ${VERS[1]} -lt 6 ] \
|| [ ${VERS[0]} -eq 2 -a ${VERS[1]} -eq 6 -a ${VERS[2]} -le 18 ] \
|| [ ${VERS[0]} -eq 4 -a ${VERS[1]} -gt 14 ]; then
    echo "Script requires Linux Kernel version 2.6.19..4.15.x"
else
    if [ -d "/opt/EDTpdv/" ]; then
        if [ -x "/opt/EDTpdv/uninstall.sh" ]; then
            echo "Uninstalling existing EDTpdv files."
            /opt/EDTpdv/uninstall.sh
        else
            echo "/opt/EDTpdv exists; failed to run EDT uninstall script."
        fi
    else
        echo "Downloading EDTpdv drivers..."
        wget https://edt.com/downloads/pdv_5-5-5-8_run/ -O EDTpdv.run
        echo "Installing EDTpdv drivers..."
        chmod +x ./EDTpdv.run
        ./EDTpdv.run && cp /opt/EDTpdv/*h ./EDT_include
    fi
fi
