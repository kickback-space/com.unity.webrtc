cmake . \
  -D CMAKE_C_COMPILER="clang-10" \
  -D CMAKE_CXX_COMPILER="clang++-10" \
  -D CMAKE_CXX_FLAGS="-stdlib=libc++" \
  -D CMAKE_BUILD_TYPE="Release" \
  -B build

cmake \
  --build build \
  --config Release \
  --target WebRTCPlugin

  cp "/LinuxData/Kickback/com.unity.webrtc/Runtime/Plugins/x86_64/libwebrtc.so" "/home/owais/kickback/RenderStreaming-HD/Packages/com.unity.webrtc@2.4.0-exp.5/Runtime/Plugins/x86_64/libwebrtc.so"