
LIBNETDIR=libnet-1.1.2.1
PCAPDIR=libpcap-1.0.0

all: celp flooder 

clean:
	rm -f celp flooder 

${LIBNETDIR}/src/libnet.a:
	cd ${LIBNETDIR}; ./configure; make; make install

${PCAPDIR}/libpcap.a:
	cd ${PCAPDIR}; ./configure; make; make install

celp: celp.c 
	gcc celp.c -o celp

flooder: ${PCAPDIR}/libpcap.a ${LIBNETDIR}/src/libnet.a flooder.c 
	gcc  flooder.c -lnet -lm -lpcap -o flooder

