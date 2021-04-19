require 'linux_stat'
TB = 10 ** 12
GB = 10 ** 9
MB = 10 ** 6
KB = 10 ** 3

def convert_bytes(n)
	if n >= TB
		"%06.2f".%(n.fdiv(TB)).split('.').join + ?4
	elsif n >= GB
		"%06.2f".%(n.fdiv(GB)).split('.').join + ?3
	elsif n >= MB
		"%06.2f".%(n.fdiv(MB)).split('.').join + ?2
	elsif n >= KB
		"%06.2f".%(n.fdiv(KB)).split('.').join + ?1
	else
		"%06.2f".%(n).split('.').join + ?0
	end
end

def convert_percent(n)
	"%06.2f".%(n).split('.').join
end

ROOT_PARTITION = IO.foreach('/proc/mounts').detect {
	|x| x.split[1] == ?/
}.to_s.split[0].to_s.split(?/).to_a[-1]

cpu_thread = Thread.new { }
mem_thread = Thread.new { }
swap_thread = Thread.new { }
io_thread = Thread.new { }
diskstat_thread = Thread.new { }
net_thread = Thread.new { }

cpu_u = mem_u = swap_u = iostat = net_upload = net_download = 0
mem_t = swap_t = 0

cpu_thread = Thread.new {
	while true
		cpu_u = LS::CPU.total_usage(0.125).to_f
	end
}

net_thread = Thread.new {
	while true
		netstat = LS::Net::current_usage(0.1)
		net_upload = convert_bytes(netstat[:transmitted])
		net_download = convert_bytes(netstat[:received])
	end
}

temp = "123456789012345678901234567890123456789012".chars

while true
	# cpu(01234) memUsed(999993) swapUsed(999992) io_active(0)
	# netUpload(999991) netDownload(999990)
	# disktotal(999990) diskused(999990)

	memstat = LS::Memory.stat
	mem_t = convert_bytes(memstat[:total].to_i.*(1000))
	mem_u = convert_bytes(memstat[:used].to_i.*(1000))
	mem_percent = convert_percent(memstat[:used].to_i.*(100).fdiv(memstat[:total].to_i))

	swapstat = LS::Swap.stat
	swap_t = convert_bytes(swapstat[:total].to_i.*(1000))
	swap_u = convert_bytes(swapstat[:used].to_i.*(1000))
	swap_percent = convert_percent(swapstat[:used].to_i.*(100).fdiv(swapstat[:total].to_i))

	iostat = IO.foreach('/proc/diskstats').find { |x|
		x.split[2] == ROOT_PARTITION
	} &.split.to_a[11]

	diskstat = LS::Filesystem.stat
	disk_total = convert_bytes(diskstat[:total])
	disk_used = convert_bytes(diskstat[:used])
	disk_used_percent = convert_percent(diskstat[:used].*(100).fdiv(diskstat[:total]))

	str = "#{convert_percent(cpu_u)}#{mem_u}#{swap_u}#{iostat}#{net_upload}#{net_download}#{disk_total}#{disk_used}#{mem_percent}#{swap_percent}#{disk_used_percent}"

	IO.write('/dev/ttyUSB0', str)
	sleep 0.25
end
