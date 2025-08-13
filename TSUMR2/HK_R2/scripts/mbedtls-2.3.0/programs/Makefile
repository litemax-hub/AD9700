
# To compile on SunOS: add "-lsocket -lnsl" to LDFLAGS
# To compile with PKCS11: add "-lpkcs11-helper" to LDFLAGS

CFLAGS	?= -O2
WARNING_CFLAGS ?= -Wall -W -Wdeclaration-after-statement
LDFLAGS ?=

LOCAL_CFLAGS = $(WARNING_CFLAGS) -I../include -D_FILE_OFFSET_BITS=64
LOCAL_LDFLAGS = -L../library 			\
		-lmbedcrypto$(SHARED_SUFFIX)
#		-lmbedtls$(SHARED_SUFFIX)	\
#		-lmbedx509$(SHARED_SUFFIX)	\

ifndef SHARED
DEP=../library/libmbedcrypto.a #../library/libmbedx509.a ../library/libmbedtls.a
else
DEP=../library/libmbedcrypto.$(DLEXT) ../library/libmbedx509.$(DLEXT) ../library/libmbedtls.$(DLEXT)
endif

ifdef DEBUG
LOCAL_CFLAGS += -g3
endif

# if we're running on Windows, build for Windows
ifdef WINDOWS
WINDOWS_BUILD=1
endif

ifdef WINDOWS_BUILD
DLEXT=dll
EXEXT=.exe
LOCAL_LDFLAGS += -lws2_32
ifdef SHARED
SHARED_SUFFIX=.$(DLEXT)
endif
else
DLEXT=so
EXEXT=
SHARED_SUFFIX=
endif

# Zlib shared library extensions:
ifdef ZLIB
LOCAL_LDFLAGS += -lz
endif

APPS =	pkey/rsa_genkey$(EXEXT) pkey/rsa_sign$(EXEXT)

ifdef PTHREAD
APPS +=	ssl/ssl_pthread_server$(EXEXT)
endif

.SILENT:

.PHONY: all clean list

all: $(APPS)

$(DEP):
	$(MAKE) -C ../library

pkey/rsa_genkey$(EXEXT): pkey/rsa_genkey.c $(DEP)
	echo "  CC    pkey/rsa_genkey.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) pkey/rsa_genkey.c  $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

pkey/rsa_sign$(EXEXT): pkey/rsa_sign.c $(DEP)
	echo "  CC    pkey/rsa_sign.c"
	$(CC) $(LOCAL_CFLAGS) $(CFLAGS) pkey/rsa_sign.c    $(LOCAL_LDFLAGS) $(LDFLAGS) -o $@

clean:
ifndef WINDOWS
	rm -f $(APPS)
else
	del /S /Q /F *.o *.exe
endif

list:
	echo $(APPS)
