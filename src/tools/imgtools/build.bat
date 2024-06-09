del *.o
del *.exe
gcc -c ff.c -o ff.o
gcc -c ffsystem.c -o ffsystem.o
gcc -c diskio.c -o diskio.o
gcc -c img-common.c -o img-common.o
gcc -c img-get.c -o img-get.o
gcc -c img-put.c -o img-put.o
gcc -c img-ls.c -o img-ls.o
gcc -c img-rm.c -o img-rm.o
gcc -c img-mkdir.c -o img-mkdir.o
gcc -c img-mkdisc.c -o img-mkdisc.o
gcc -c img-fdisc.c -o img-fdisc.o
gcc -c img-mkfs.c -o img-mkfs.o
gcc img-get.o img-common.o diskio.o ffsystem.o ff.o -o img-get.exe
gcc img-put.o img-common.o diskio.o ffsystem.o ff.o -o img-put.exe
gcc img-ls.o img-common.o diskio.o ffsystem.o ff.o -o img-ls.exe
gcc img-rm.o img-common.o diskio.o ffsystem.o ff.o -o img-rm.exe
gcc img-mkdir.o img-common.o diskio.o ffsystem.o ff.o -o img-mkdir.exe
gcc img-mkdisc.o img-common.o diskio.o ffsystem.o ff.o -o img-mkdisc.exe
gcc img-fdisc.o img-common.o diskio.o ffsystem.o ff.o -o img-fdisc.exe
gcc img-mkfs.o img-common.o diskio.o ffsystem.o ff.o -o img-mkfs.exe

