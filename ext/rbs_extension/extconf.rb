require 'mkmf'

$INCFLAGS << " -I$(top_srcdir)" if $extmk
$INCFLAGS << " -I$(srcdir)/../../include"

$VPATH << "$(srcdir)/../../src"
$VPATH << "$(srcdir)/ext/rbs_extension"

root_dir = File.expand_path('../../../', __FILE__)
$srcs = Dir.glob("#{root_dir}/src/*.c") +
        Dir.glob("#{root_dir}/ext/rbs_extension/*.c")

# Add debug flags for LLDB
append_cflags ['-std=gnu99', '-O0', '-g']

# Disable optimization to make debugging easier
$CFLAGS.gsub!(/-O[123]/, '')

create_makefile 'rbs_extension'
