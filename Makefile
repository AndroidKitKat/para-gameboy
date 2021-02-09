CC=lcc

game: game.c big_gun.c
	$(CC) -o para.gb game.c
