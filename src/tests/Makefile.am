TESTS = test01.vtc test02.vtc test03.vtc test04.vtc test05.vtc test06.vtc test07.vtc test08.vtc test09.vtc
.PHONY: test $(TESTS)

test: $(TESTS)

$(TESTS):
	@VARNISHTEST@ -Dvmod_topbuild=$(abs_top_builddir) $@
