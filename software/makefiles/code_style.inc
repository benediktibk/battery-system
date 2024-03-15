CLANGSOURCESBATTERYMANAGEMENTSYSTEM := $(shell find battery_management_system -type f -name '*.[ch]')
CLANGSOURCESCOMPLETE := $(CLANGSOURCESBATTERYMANAGEMENTSYSTEM)

format:
	$(CLANGFORMAT) -style=file -Werror -i $(CLANGSOURCESCOMPLETE)

build/clang-format: build/guard $(ALLMAKEFILES) .clang-format $(CLANGSOURCESCOMPLETE)
	rm -f $@
	$(CLANGFORMAT) -style=file -Werror --dry-run $(CLANGSOURCESCOMPLETE) || (echo "clang-format failed $$?"; exit 1)
	touch $@