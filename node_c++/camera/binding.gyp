
{
    "targets": [
    {
        "target_name": "camera",
        "sources": [ "camera.cc" ],
        "include_dirs": [ "<!(node -e \"require('nan')\")" ]
    }
  ]
}
