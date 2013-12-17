require 'google_drive'

class Spreadsheet
	def initialize(email, password, key)
		@session = GoogleDrive.login(email, password)
		@ws = @session.spreadsheet_by_key(key).add_worksheet("test")
		@row = 0; @col = 0
		print "created a new worksheet!\n"
	end

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

	def set_col(col)
		@col = col
	end

	def set_row(row)
		@row = row
	end
end
