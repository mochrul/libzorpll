EXTRA_DIST = makefile.msc libzorpll.rc libzorpll.def

SUBDIRS = zorp

lib_LTLIBRARIES = libzorpll.la

libzorpll_la_SOURCES = \
	cap.cc \
	connect.cc \
	crypt.cc \
	error.cc \
	io.cc \
	listen.cc \
	log.cc \
	memtrace.cc \
	misc.cc \
	packetbuf.cc \
	poll.cc \
	process.cc \
	random.cc \
	registry.cc \
	sockaddr.cc \
	socket.cc \
	socketsource.cc \
	source.cc \
	ssl.cc \
	stackdump.cc \
	stream.cc \
	streambuf.cc \
	streamfd.cc \
	streamgzip.cc \
	streamline.cc \
	streamssl.cc \
	thread.cc \
	zobject.cc \
	blob.cc \
	streamblob.cc \
	streamtee.cc \
	code_base64.cc \
	code.cc \
	code_cipher.cc \
	code_gzip.cc \
	headerset.cc \
	zcp.cc \
	zurlparse.cc
	

libzorpll_la_LDFLAGS = \
	-version-info $(LIBZORPLL_LT_CURRENT):0:$(LIBZORPLL_LT_AGE) \
	-release $(LIBZORPLL_LT_RELEASE) \
	-export-dynamic

libzorpll_la_CPPFLAGS = -DCOMPILING_LIBZORPLL

install-exec-local:
	mkdir -p ${DESTDIR}/${ZORPLIB_TEMP_DIR}
	mkdir -p ${DESTDIR}/${ZORPLIB_PIDFILE_DIR}
