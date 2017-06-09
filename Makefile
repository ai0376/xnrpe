
subdir_plugins=./plugins
subdir_src=./src
subdir_test_server=./test_server

subsystem:
	$(MAKE) -C $(subdir_plugins)
	$(MAKE) -C $(subdir_test_server)
	$(MAKE) -C $(subdir_src)

clean:
	$(MAKE) -C $(subdir_plugins) clean
	$(MAKE) -C $(subdir_test_server) clean
	$(MAKE) -C $(subdir_src) clean
