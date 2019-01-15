{
    "targets": [{
        "target_name": "musicData",
        "include_dirs" : [
            "src",
            "<!(node -e \"require('nan')\")"
        ],
        "libraries": ["-l avformat"],
        "sources": [
            "modules/musicData/index.cpp"
        ]
    }]
}
