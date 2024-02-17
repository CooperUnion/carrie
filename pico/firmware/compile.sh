#/bin/bash
elf=build/src/carrie.elf
uf2=build/src/carrie.uf2
tty=/dev/ttyACM0
block_dev=/dev/sda1

if [ ! -d build/ ]; then
	mkdir build/ && cmake -DCMAKE_BUILD_TYPE=Debug -B build/ .
fi

if [ $? -eq 0 ]; then
	make -C build/
fi

if [ $? -eq 0 ]; then
	case $1 in
	-u) # upload via debug lines
		openocd -f interface/raspberrypi-swd.cfg \
			-f target/rp2040.cfg \
			-c "program $elf verify reset exit"
		;;
	-p) # program over serial
		# force a reset using a 1200bps open/close
		stty -F $tty speed 1200
		sleep 1

		# wait for a block device
		while [ ! -e $block_dev ]; do sleep .5; done

		echo "programming..."
		mkdir _pico-storage
		sudo mount $block_dev _pico-storage/
		sudo cp $uf2 _pico-storage/
		while [ ! -e $tty ]; do sleep .5; done
		sudo umount _pico-storage/
		rmdir _pico-storage/
		;;
	esac
fi
