MAKE=$1
if [ -z ${MAKE} ]; then
    MAKE=make
fi

cd tools/W806/config
${MAKE} mconf
cd ..
../../bin/build/config/mconf wconfig
