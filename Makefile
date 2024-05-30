hellomake: main.c
	clang -Ilib/mlx_linux main.c -Llib/mlx_linux -lX11 -lXext -lmlx
