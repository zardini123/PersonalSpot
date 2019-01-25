{
    "targets": [
        {
            "target_name": "<(module_name)",
            "sources": [
                "src/<(module_name)/index.cpp"
            ],
            "include_dirs" : [
                "libraries/include",
                "<!(node -e \"require('nan')\")"
            ],
            "link_settings": {
                "libraries": [
                    "<(module_root_dir)/libraries/lib/libboost_filesystem.dylib"
                ]
            },
            "libraries": [
                "-l avformat"
            ],
            # NOTE: Some of the libraries (boost) use C++ exceptions, and that's not
            # cool with clang.  These commands disable exception checking upon
            # compile.
            'cflags!': [ '-fno-exceptions' ],
            'cflags_cc!': [ '-fno-exceptions' ],
            'conditions': [
                ['OS=="mac"', {
                    'xcode_settings': {
                        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES'
                    }
                }]
            ]
            #####
        },
        # NOTE: 'module_name' and 'module_path' come from the 'binary' property in package.json
        # node-pre-gyp handles passing them down to node-gyp when you build from source
        {
            "target_name": "action_after_build",
            "type": "none",
            "dependencies": [ "<(module_name)" ],
            "copies": [
                {
                    "files": [
                        "<(PRODUCT_DIR)/<(module_name).node"
                    ],
                    "destination": "<(module_path)"
                }
            ]
        }
    ]
}
