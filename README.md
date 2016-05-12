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

    when_is_on --ip <path> --script_all <path> --script_none <path> --script_some <path> --t <unsigned integer> --no_concurrency --help
    --help              display this help message
    --ip arg            path to the file containing ip addresses
    --t arg             time in second between ip tests
    --no_concurrency    scripts are not launch concurrently
    --script_all arg     path to the file containing the list of script to launch when all ip answer to ping
    --script_none arg    path to the file containing the list of script to launch when all ip does not answer to ping
    --script_some arg   path to the file containing the list of script to launch when at least one ip answer to ping
    
    
