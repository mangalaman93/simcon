# spreadsheet class to update data in google spreadsheet

require 'google_drive'
require 'net/http'

class Spreadsheet
	PHASES = 144

	def initialize(email, password, key, id)
		@session = GoogleDrive.login(email, password, Net::HTTP::Proxy('netmon.iitb.ac.in', 80, 'bkd.online', '2453419*'))
		if id.nil?
			@ws = @session.spreadsheet_by_key(key).worksheets[@session.spreadsheet_by_key(key).worksheets.size-1]
		else
			@ws = @session.spreadsheet_by_key(key).worksheets[id]
		end
		@row = 1; @col = 1;
	end

	def update_workloads(filename="input")
		set_col(2); set_row(2);
		begin
			file = File.new(filename, "r")
		    counter = 0; file.gets; file.gets; # skipping first 2 lines
			while (line = file.gets) && counter < PHASES
				tokens = line.split(" ")
				store_next_row(tokens)
				counter = counter + 1
			end
			save
			file.close
			puts "updated workload!"
		rescue => err
			puts "Exception occurred: #{err}"
		end
	end

	def update_utilizations(filename="results/mdp_util.txt")
		set_col(15); set_row(2);
		begin
			update_values(filename)
			puts "updated utilizations!"
		rescue => err
			puts "Exception occurred: #{err}"
		end
	end

	def update_mdp_policy(filename="results/mdp_policy.txt")
		set_col(28); set_row(2);
		update_policy(filename)
	end

	def update_khanna_policy(filename="results/khanna_policy.txt")
		set_col(58); set_row(2);
		update_policy(filename)
	end

	def get_workload(row, col)
		return @ws[@row+row, @col+col]
	end

  private
	def store_next_row(vals)
		vals.each_with_index { |val, col|
			@ws[@row, @col+col] = val
		}
		@row = @row + 1
	end

	def store_next_col(vals)
		vals.each_with_index { |val, row|
			@ws[@row+row, @col] = val
		}
		@col = @col + 1
	end

	def save
		@ws.save
		@ws.reload
	end

	def set_col(col)
		@col = col
	end

	def set_row(row)
		@row = row
	end

	def update_values(filename)
		file = File.new(filename, "r")
		while (line = file.gets)
			tokens = line.split(" ")
			store_next_row(tokens)
		end
		save
		file.close
	end

	def update_policy(filename)
		begin
			update_values(filename)
			puts "updated policy!"
		rescue => err
			puts "Exception occurred: #{err}"
		end
	end
end
