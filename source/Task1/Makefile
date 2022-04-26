########################
#makefile of task a
########################
src = task_a.asm
src2 = task_a.com

.PHONY : everything clean

everything : $(src2)
	sudo mount -o loop pmtest.img /mnt/floppyB/
	sudo cp $(src2) /mnt/floppyB/ -v
	sudo umount /mnt/floppyB/

$(src2) : $(src)
	nasm $(src) -o $(src2)

bochs:
	bochs -f bochsrc.txt

clean:
	rm -rf $(src2)
