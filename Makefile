hellomake: main.c
	clang main.c -Ilib/mlx_linux -Llib/mlx_linux -lX11 -lXext -lmlx
