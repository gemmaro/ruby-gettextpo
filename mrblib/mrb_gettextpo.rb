# Copyright (C) 2026  gemmaro
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

module GettextPO
  Error = Class.new(StandardError)

  class File
    class << self
      # It seems that calling Proc with keyword arguments is not yet
      # supported.
      alias original_read read
      def read(filename, xerror: nil, xerror2: nil)
        original_read(filename,
                      xerror: xerror && Proc.new { |kwargs| xerror.(**kwargs) },
                      xerror2: xerror2 && Proc.new { |kwargs| xerror2.(**kwargs) })
      end
    end

    # It seems that calling Proc with keyword arguments is not yet
    # supported.
    alias original_check_all check_all
    def check_all(xerror: nil, xerror2: nil)
      original_check_all(xerror: xerror && Proc.new { |kwargs| xerror.(**kwargs) },
                         xerror2: xerror2 && Proc.new { |kwargs| xerror2.(**kwargs) })
    end
  end

  class MessageIterator
    def self.new
      raise NoMethodError,
            "please use other methods instead, such as GettextPO::File#message_iterator"
    end

    def each # yields: message
      while true
        begin
          yield self.next
        rescue StopIteration
          return self
        end
      end
    end

    include Enumerable
  end

  class Message
    def self.new
      raise NoMethodError,
            "please use other methods instead, such as GettextPO::MessageIterator#next"
    end

    # It seems that calling Proc with keyword arguments is not yet
    # supported.
    alias original_check_all check_all
    def check_all(iterator, xerror: nil, xerror2: nil)
      original_check_all(iterator,
                         xerror: xerror && Proc.new { |kwargs| xerror.(**kwargs) },
                         xerror2: xerror2 && Proc.new { |kwargs| xerror2.(**kwargs) })
    end

    # It seems that calling Proc with keyword arguments is not yet
    # supported.
    alias original_check_format check_format
    def check_format(xerror: nil, xerror2: nil)
      original_check_format(xerror: xerror && Proc.new { |kwargs| xerror.(**kwargs) },
                            xerror2: xerror2 && Proc.new { |kwargs| xerror2.(**kwargs) })
    end
  end

  class FilePos
    def self.new
      raise NoMethodError,
            "please use other methods instead, such as GettextPO::Message#filepos"
    end
  end
end
