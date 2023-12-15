{
  "targets": [
    {
      "target_name": "worklet",
      "sources": [
           "src/worklet_entry.cpp",
        ],
      "include_dirs": [
            "<!(node -p \"require('node-addon-api').include_dir\")"
      ],
      "defines": [

      ],
      'link_settings': {
#         "libraries": [ "-Wl,-rpath,@loader_path/../../agil/heron/src/agil"],
      },
      "conditions": [
          ["OS==\"mac\"", {
              "libraries": [
                  "<(module_root_dir)/agil/third-party-ndk/v8jsi/libjscruntime.a",
                  "<(module_root_dir)/agil/third-party-ndk/v8jsi/libjsi.a",
                  "<(module_root_dir)/agil/third-party-ndk/folly/libfolly_futures.a",
                  "<(module_root_dir)/agil/third-party-ndk/folly/libfolly_runtime.a",
                  "<(module_root_dir)/agil/third-party-ndk/double-conversion/libdouble-conversion.a",
                  "<(module_root_dir)/agil/third-party-ndk/fmt/libfmt.a",
                  "<(module_root_dir)/agil/third-party-ndk/glog/libglog.a",
                  "<(module_root_dir)/agil/libnodeworklet.a",
              ],
              "include_dirs": [
                #homebrew
                "/opt/homebrew/opt/sdl2/include",

                #third_party
                "/usr/local/include",
                "../third-party-ndk/folly",
                "../third-party-ndk/double-conversion",
                "../third-party-ndk/fmt",
                "../third-party-ndk/glog/exported",
                "../third-party-ndk/boost/boost_1_76_0",
                "../third-party-ndk/v8jsi",
                "../third-party-ndk/jsi",

                #worklet
                "../worklet",
                "../worklet/reanimated"

              ],
              "xcode_settings": {
                "GCC_ENABLE_CPP_EXCEPTIONS": "YES",
                "OTHER_CFLAGS": [
                "-std=c++17",
                "'-DFOLLY_NO_CONFIG=1'",
                        "'-DFOLLY_HAVE_CLOCK_GETTIME=1'",
                        "'-DFOLLY_USE_LIBCPP=1'",
                        "'-DFOLLY_HAVE_PTHREAD=1'",
              ],
              }
          }]
        ],
    }
  ]
}