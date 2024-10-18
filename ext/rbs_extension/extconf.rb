require 'mkmf'

root_dir = File.expand_path('../../../', __FILE__)

$srcs = Dir.glob("#{root_dir}/src/*.c") +
        Dir.glob("#{root_dir}/ext/rbs_extension/*.c")

$INCFLAGS << " -I$(top_srcdir)" if $extmk

$VPATH << "#{root_dir}/src"
$VPATH << "#{root_dir}/ext/rbs_extension"

append_cflags ['-std=gnu99']
create_makefile 'rbs_extension'
