all: laba defined undefined

laba: main.c
	@clang main.c -o laba

defined: f_def g_def

undefined: f_undef g_undef

test_defined: laba defined
	./laba f_def g_def

test_undefined: laba undefined
	./laba f_undef g_undef

f_def: f_def.c
	@clang f_def.c -o f_def

g_def: g_def.c
	@clang g_def.c -o g_def

f_undef: f_undef.c
	@clang f_undef.c -o f_undef

g_undef: g_undef.c
	@clang g_undef.c -o g_undef

clean:
	@rm laba f_def f_undef g_def g_undef

re: clean all

.PHONY: all clean re defined undefined