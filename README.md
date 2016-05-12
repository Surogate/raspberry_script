# raspberry_script
small scripts for local server

scan-auto-fetch
==============
automatically fetch manga through the net, using configurations files

Allowed options:

    scan-auto-fetch --c <path> --help
    --help                produce help message
    --c arg               path to configuration file
    
when_is_on
==============
launch scripts/command line when a set of ip response to ping

Allowed options:

    when_is_on --ip <path> --script_on <path> --script_off <path> --script_once <path> --t <unsigned integer> --help
    --help              display this help message
    --ip arg            path to the file containing ip addresses
    --t arg             time in second between ip tests
    --script_on arg     path to the file containing the list of script to launch when all ip answer to ping
    --script_off arg    path to the file containing the list of script to launch when not all ip answer to ping
    --script_once arg   path to the file containing the list of script to launch when at least one ip answer to ping
    
    
