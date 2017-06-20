module PatchKit
  class BinaryReplace
    def initialize(opts)
      @stamp = opts.delete(:stamp) || raise('missing :stamp')
      @buffer_size = opts.delete(:buffer_size) || raise('missing :buffer_size')
      raise "unknown options: #{opts}" unless opts.empty?
    end

    def replace_in_str!(input_string, output_string)
      i = stamp_index(input_string)
      raise 'stamp not found' if i.nil?

      pre = input_string.byteslice(0, i)
      post = input_string.byteslice(i + @buffer_size, input_string.bytesize)

      size = output_string.bytesize
      data = [size].pack('v') + output_string
      remaining = @buffer_size - data.bytesize

      raise "replacement is too big (#{remaining})" if remaining < 0
      data += [0].pack('C') * remaining

      pre + data + post
    end

    def stamp_index(input_string)
      input_bytes = input_string.bytes
      stamp_bytes = @stamp.bytes

      input_bytes.each_index do |i|
        if input_bytes[i] == stamp_bytes[0]
          probe = input_bytes.slice(i, stamp_bytes.size)
          return i if probe == stamp_bytes
        end
      end

      nil
    end
  end
end

binary_file = ARGV[0] # file that will be searched & replaced
replacement_file = ARGV[1] # file which contents should be used as a replacement

br = PatchKit::BinaryReplace.new(
  stamp: 'OnceUponAMidnightDreary',
  buffer_size: 1024
)
puts br.replace_in_str!(IO.binread(binary_file), IO.binread(replacement_file))
