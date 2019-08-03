/jbibtex\.h/,$d
/^jmp_buf/d
/^typedef/b
/^#define/b
/^[a-zA-Z_].*;/{
	s/^/extern /
	b
}
/^[a-zA-Z].*/ {
	/^[^;][^;]*$/ {
		s/^/extern /
		b
	}
}
/^  */b
t
d
