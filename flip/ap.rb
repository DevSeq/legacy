
class APList
	attr_reader :path, :apHash

	def initialize(path)
		@path = path 
		@apHash = Hash.new;
		refresh
	end

	def refresh
		Dir.foreach(@path) { |entry| 
			# skip non MAC entries
			if not (/\S{6}/ =~ entry) then next; end

			ap = AccessPoint.new(@path, entry);
			apHash[ap.mac] = ap;
		}
	end

	def execute_local_all(cmd)
		ret = Hash.new;
		@apHash.each_value{ |ap|
			ret[ap.mac] = ap.execute_local(cmd);
		}
		return ret;
	end

	def execute_remote_all(cmd)
		ret = Hash.new;
		@apHash.each_value{ |ap|
			ret[ap.ip] = ap.execute_remote(cmd);
		}
		return ret;
	end

        def method_missing(sym,*args)
                @apHash.send sym,*args
        end
end


class AccessPoint 
	attr_reader :ip, :hostname_line, :mac, :connected_clients, :client

	def initialize(path, mac)
		@mac = mac;
		@path = path + "/" + mac;
		read_hostname
	end

	def read_hostname
		File.open( "#{@path}/etc/hostname.sis0" ) do |f|
			@hostname_line = f.readline

			rx = /^inet\s+((?:\d{1,3}\.){3}\d{1,3})/
			array = rx.match( @hostname_line );
			if not array.nil? then
				@ip = array[1]
			else 
				puts "Could not parse IP";
			end
		end
	end

	def get_clients
		return execute_remote( "brconfig bridge0 " ) 
	end
	
	def get_basic_information
		return execute_remote( "hostname; uptime;" );
	end

	def info
		return get_basic_information;
	end

	def clients
		return get_clients;
	end

	def collect_statistics
		stat = Hash.new;
		ret = execute_remote( "ifconfig -a ; brconfig bridge0 ; pfctl -s label" );

		# extract connected clients
		wi0Clients = Array.new;
		ath0Clients = Array.new;
		ath1Clients = Array.new;

		lines = ret.split("\n");
		wi0Re  = /^\s*(\S\S:\S\S:\S\S:\S\S:\S\S:\S\S) wi0 .*flags=/;
		ath0Re = /^\s*(\S\S:\S\S:\S\S:\S\S:\S\S:\S\S) ath0 .*flags=/;
		ath1Re = /^\s*(\S\S:\S\S:\S\S:\S\S:\S\S:\S\S) vlan0 .*flags=/;
		lines.each { |line|
			wi0Clients.push  m[1] if (m = wi0Re.match(line))
			ath0Clients.push m[1] if (m = ath0Re.match(line))
			ath1Clients.push m[1] if (m = ath1Re.match(line))
		}

		stat["wi0Clients"]  = wi0Clients;
		stat["ath0Clients"] = ath0Clients;
		stat["ath1Clients"] = ath1Clients;


		stat["pf"] = Hash.new if stat["pf"].nil?
		pf = stat["pf"];

		# pf log 
		pfRe = /^<PF> (.+on.+) (\d+) (\d+) (\d+) (\d+) (\d+) (\d+) (\d+)$/;
		lines.each { |line|
			if (m = pfRe.match(line)) then
				if pf[m[1]].nil?  then
					pf[m[1]] = Hash.new 
					pf[m[1]]["eval"]      = 0;
					pf[m[1]]["pkt"]       = 0;
					pf[m[1]]["bytes"]     = 0;
					pf[m[1]]["pkt_in"]    = 0;
					pf[m[1]]["bytes_in"]  = 0;
					pf[m[1]]["pkt_out"]   = 0;
					pf[m[1]]["bytes_out"] = 0;
				end
				
				pf[m[1]]["eval"]      += m[2].to_i; 
				pf[m[1]]["pkt"]       += m[3].to_i;
				pf[m[1]]["bytes"]     += m[4].to_i;
				pf[m[1]]["pkt_in"]    += m[5].to_i;
				pf[m[1]]["bytes_in"]  += m[6].to_i;
				pf[m[1]]["pkt_out"]   += m[7].to_i;
				pf[m[1]]["bytes_out"] += m[8].to_i;
			end
		}


		return stat
	end

	def build_config_tgz( dstpath )
		if !File.exists?( "#{dstpath}/#{@mac}" ) then
			 Dir.mkdir( "#{dstpath}/#{@mac}", 493 )    # KEINER VERSTEHT DIE ZAHL
		end

		execute_local( "tar -czf #{dstpath}/#{@mac}/config.tgz ." );
	end

	def execute_local( cmd )
		last = Dir.pwd
		Dir.chdir(@path)
		ret = `#{cmd}`
		Dir.chdir(last)
		return ret
	end

	def execute_remote( cmd )
		return `ssh -l root -i soekris.dsa #{ip} "#{cmd}"`;
	end
private
end

# 
# apList = APList.new( "tmp/aps" );
# 
# apHash = apList.apHash;
# 
# 
# apHash.each_value do  |ap|
# 	puts ap.info
# end
