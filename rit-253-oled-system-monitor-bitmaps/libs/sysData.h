struct sysData {
	float cpu_usage = 0 ;
	float mem_usage = 0 ;
	float mem_percent = 0 ;
	float swap_usage = 0 ;
	float swap_percent = 0 ;
	float io_active = 0 ;
	unsigned long ps_count = 0 ;
	float net_upload = 0 ;
	float net_download = 0 ;
	float disk_total = 0 ;
	float disk_used = 0 ;
	float disk_avail ;
	float disk_used_percent = 0 ;

	unsigned int process_sleeping = 0 ;
	unsigned int process_idle = 0 ;
	unsigned int process_zombie = 0 ;
	unsigned int process_running = 0 ;
	unsigned int process_stopped = 0 ;

	char *mem_unit = "B" ;
	char *swap_unit = "B" ;
	char *net_upload_unit = "B" ;
	char *net_download_unit = "B" ;
	char *disk_total_unit = "B" ;
	char *disk_used_unit = "B" ;
	char *disk_avail_unit = "B" ;
} ;
