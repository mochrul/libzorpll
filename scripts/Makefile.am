#sysconf_DATA = libzorpll.conf


libexec_SCRIPTS = failure_notify@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@
EXTRA_DIST = failure_notify.in

failure_notify@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@: failure_notify.in
	$(top_builddir)/config.status --file $@:$<
