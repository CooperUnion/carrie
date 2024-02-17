include_directories(.)

add_executable(carrie
	carrie.c++
#	console.c++
)

# enable USB, disable UART
pico_enable_stdio_usb(carrie 1)
pico_enable_stdio_uart(carrie 0)

pico_add_extra_outputs(carrie)

add_subdirectory(comms)
add_subdirectory(control)
add_subdirectory(rcc)

target_link_libraries(carrie
	control
	comms

	pico_stdlib
	rcc

#	Argumentum:argumentum
	cpp-base64
	hash-library-sha1
)
