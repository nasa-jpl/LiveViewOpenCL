#!/bin/bash
IFS='-' read -a PREVERS <<< "$(uname -r)"
IFS='.' read -a VERS <<< "${PREVERS[0]}"
EDTDIR="/opt/EDTpdv"

if [ ${VERS[0]} -lt 2 -o ${VERS[0]} -gt 4 ] \
|| [ ${VERS[0]} -eq 2 -a ${VERS[1]} -lt 6 ] \
|| [ ${VERS[0]} -eq 2 -a ${VERS[1]} -eq 6 -a ${VERS[2]} -le 18 ] \
|| [ ${VERS[0]} -eq 4 -a ${VERS[1]} -gt 15 ]; then
    echo "Script requires Linux Kernel version 2.6.19..4.15.x"
else
    if [ -d "$EDTDIR" ]; then
        if [ -x "$EDTDIR/uninstall.sh" ]; then
            echo "Uninstalling existing EDTpdv files."
            pushd "$EDTDIR" && ./uninstall.sh && rmdir "$EDTDIR"
            popd
        else
            echo "$EDTDIR exists; failed to run EDT uninstall script."
            exit
        fi
    fi

    echo "Downloading EDTpdv drivers..."
    wget https://edt.com/downloads/pdv_5-5-5-8_run/ -O EDTpdv.run
    echo "Installing EDTpdv drivers..."
    chmod +x ./EDTpdv.run && ./EDTpdv.run "$EDTDIR"
    if [ ! -d ./EDT_include ]; then
        mkdir ./EDT_include
    fi
    cp "$EDTDIR/*.h" ./EDT_include/
fi
