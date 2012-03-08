EXTRA_DIST = changelog control copyright rules compat \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.config \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.install \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.postinst \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.shlibs \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.templates \
	libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@-memtrace.lintian-overrides \
	libzorpll@BINARY_BRANCH@-dev.install

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.config: libzorpll.config.in
	cp $< $@

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.install: libzorpll.install.in
	cp $< $@

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.postinst: libzorpll.postinst.in
	$(top_builddir)/config.status --file $@:$<

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.shlibs: libzorpll.shlibs.in
	cp $< $@

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@.templates: libzorpll.templates.in
	cp $< $@

libzorpll@LIBZORPLL_LT_COMPAT_BRANCH@@BINARY_BRANCH@-memtrace.lintian-overrides: libzorpll-memtrace.lintian-overrides.in
	cp $< $@

libzorpll@BINARY_BRANCH@-dev.install: libzorpll-dev.install.in
	cp $< $@

libzorpll.shlibs.in: libzorpll.shlibs.in.in
	$(top_builddir)/config.status --file $@:$<

control: control.in
	$(top_builddir)/config.status --file $@:$<

rules: rules.in
	$(top_builddir)/config.status --file $@:$<
	chmod a+x rules

changelog: changelog.in
	$(top_builddir)/config.status --file $@:$<


MAINTAINERCLEANFILES=control changelog copyright rules libzorpll.shlibs.in
