require 'mkmf'
$INCFLAGS << " -Iinclude/" if $extmk
append_cflags ['-std=gnu99']
create_makefile 'rbs_extension'
