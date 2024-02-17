add_library(control
	low-level.c++
	velocity.c++
	steering.c++
)

target_link_libraries(control
	pico_stdlib
	pico_multicore
	rcc
)
